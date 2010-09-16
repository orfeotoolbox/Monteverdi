/*=========================================================================

  Program:   ORFEO Toolbox
  Language:  C++
  Date:      $Date$
  Version:   $Revision$


  Copyright (c) Centre National d'Etudes Spatiales. All rights reserved.
  See OTBCopyright.txt for details.


  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "otbProjectionModule.h"

#include <FL/Fl_Text_Buffer.H>
#include <FL/fl_ask.H>
#include <FLU/Flu_File_Chooser.h>
#include <FL/Enumerations.H>

#include "otbMacro.h"
#include "otbPerBandVectorImageFilter.h"
#include "itkExceptionObject.h"

#include "elevation/ossimElevManager.h"
#include "base/ossimFilename.h"
#include "base/ossimDirectory.h"

// Maps include
#include "otbMapProjections.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkNearestNeighborInterpolateImageFunction.h"
#include "otbWindowedSincInterpolateImageBlackmanFunction.h"
#include "otbWindowedSincInterpolateImageHammingFunction.h"
#include "otbWindowedSincInterpolateImageCosineFunction.h"
#include "otbWindowedSincInterpolateImageWelchFunction.h"
#include "otbWindowedSincInterpolateImageLanczosFunction.h"
#include "otbBSplineInterpolateImageFunction.h"
#include "otbMsgReporter.h"
#include "otbUtmMapProjection.h"

#include "itkContinuousIndex.h"
#include <ogr_spatialref.h>

namespace otb
{
/**
 * Constructor
 */
ProjectionView::ProjectionView()
{
  // Build the Gui
  this->CreateGUI();

  // Initiliaze the GUI values
  guiUTMZone->value("31");
  guiTRANSMERCATOREast->value("0");
  guiTRANSMERCATORNorth->value("0");
  guiTRANSMERCATORScale->value("1");
  iTRANSMERCATOREast->value("0");
  iTRANSMERCATORNorth->value("0");
  iTRANSMERCATORScale->value("1");
  m_InterpType = MAP_LINEAR_;
  m_MapType = MAP_UTM;
  m_InputProjectionUnknown = false;
}

/**
 * Handle the notifications of the model
 */
void
ProjectionView::Notify()
{
  if (m_Controller->GetModel()->GetTransformChanged())
    {
    // Update the cartographic parameters
    this->UpdateMapParam();
    // Update the output
    this->UpdateOutputParameters();
    }

  if (m_Controller->GetModel()->GetTempTransformChanged())
    {
    m_InputProjectionUnknown = false;
    this->UpdateUnavailableLongLat();
    }
}

/**
 * Guess what is the Projection of the Origin if any
 */
void
ProjectionView
::RetrieveInputProjection()
{
  // The discriminative parameter to distinguish UTM and TransverseMercator
  double scale_factor = 0.;

  // Get the projection Ref from the input Image
  std::string inputProjRef = m_Controller->GetModel()->GetInputImage()->GetProjectionRef();

  // If the Projection is not empty : Map Projection
  OGRSpatialReference oSRS;

  // Else perhaps a model sensor
  if (!inputProjRef.empty())
    {
    // From std::string to char* : Needed for the importFromWkt(char** projRef)
    char * inputProjchar = new char[inputProjRef.length() + 1];
    strcpy(inputProjchar, inputProjRef.c_str());

    // Import OGRSpatial Reference object from projectionRef
    // Warning : importFromWkt modifies the passed pointer !
    char* inputProjcharForImport = inputProjchar;
    oSRS.importFromWkt(&inputProjcharForImport);

    // First test if the node Projection exists : Avoid Segfaults when
    // uses strcmp with null values.
    if (oSRS.GetAttrValue("PROJECTION") != NULL)
      {
      // Get the value of the node PROJECTION
      const char * inputMap = oSRS.GetAttrValue("PROJECTION");

      // Test the different combination we want
      if (strcmp(inputMap, "Lambert_Conformal_Conic_2SP") == 0)
        {
        // Fill the GUI With the  LAMBERT 2 parameters
        iMapSelection->value(1);
        iLambert2->show();
        iUTM->hide();
        iTRANSMERCATOR->hide();
        }
      else if (strcmp(inputMap, "Transverse_Mercator") == 0)
        {
        // Get the value of the parameter scale_factor
        if (this->FindParameter(oSRS, "scale_factor", &scale_factor))
          {

          if (scale_factor == 0.9996 /** Value Specific to UTM */)
            {
            iMapSelection->value(0);
            iUTM->show();
            iLambert2->hide();
            iTRANSMERCATOR->hide();

            // Get the number of the utm zone
            int north = 0;
            int zone = oSRS.GetUTMZone(&north);

            // Fill the UTM Parameters in the GUI
            itk::OStringStream oss;
            oss << zone;
            iUTMZone->value(oss.str().c_str());

            if (north)
              {
              iUTMNorth->value(1);
              iUTMSouth->value(0);
              }
            else
              {
              iUTMNorth->value(0);
              iUTMSouth->value(1);
              }
            }
          else
            {
            //Fill the TransverseMercator Parameters
            iMapSelection->value(2);
            iUTM->hide();
            iLambert2->hide();
            iTRANSMERCATOR->show();
            }
          }
        }
      delete inputProjchar;
      }
    }

  // Sensor Model :: Test if KeyWorldList is not empty
  if (m_Controller->GetModel()->GetInputImage()->GetImageKeywordlist().GetSize() > 0 && inputProjRef.empty())
    {
    // Select the Sensor Model in the InputMapType
    iMapSelection->value(3);
    iUTM->hide();
    iLambert2->hide();
    iTRANSMERCATOR->hide();
    }

  // Default Case : Kwl & ProjRef  empty : Give the user the possibility to fill the proj he needs
  if (m_Controller->GetModel()->GetInputImage()->GetImageKeywordlist().GetSize() == 0 && inputProjRef.empty())
    {
    iMapType->activate();
    iMapSelection->activate();
    m_InputProjectionUnknown = true;
    }
}

/**
 * In the OGR_SRSNode : Find the value of the parameter inParam if exists
 *
 */
bool
ProjectionView
::FindParameter(OGRSpatialReference oSRS, const char * inParam, double * paramValue)
{
  // Get all the nodes of the Projection Ref Tree
  // and search for a child named : "scale_factor"
  OGR_SRSNode* node = oSRS.GetAttrNode("PROJCS");

  int nbChild  = node->GetChildCount();
  for (int i = 0; i < nbChild; i++)
    {
    OGR_SRSNode* curChild = node->GetChild(i);
    int          res      = curChild->FindChild(inParam);
    if (res == 0)
      {
      // scale_factor parameter found
      *paramValue = strtod(curChild->GetChild(1)->GetValue(), NULL);
      return true;
      }
    }
  return false;
}

/**
 *
 */
void
ProjectionView
::Show()
{
  try
    {
    // Notify the controller that the GUI is going to be shown
    // Note that call is useful for transform initialization and paramters initiaization
    // Get the projection initial parameters
    this->RetrieveInputProjection();
    // Default : utm is the output projection proposed
    this->UpdateUTMTransform();
    // Edit the GUI following the projection computed when UpdateUTMTransform Called
    this->InitializeAction();

    // test if the good parameters are available
    // int resCheckImageParameters = this->CheckImageParameters();
    int resCheckImageParameters = 0;

    if (resCheckImageParameters == 1)
      {
      itkExceptionMacro(<< "Invalid image parameters");
      }

    this->ShowGUI();
    }
  catch (itk::ExceptionObject& err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    }
}

/**
 * Show the main gui
 */
void
ProjectionView
::ShowGUI()
{
  guiMainWindow->show();
}

/**
 *
 */
void
ProjectionView
::OK()
{
  // tell the model to project the region edited in the GUI
  unsigned int sizeX    = atoi(guiSizeX->value());
  unsigned int sizeY    = atoi(guiSizeY->value());
  double       spacingX = atof(guiSpacingX->value());
  double       spacingY = atof(guiSpacingY->value());
  double       originX =  atof(guiLongSelection->value());
  double       originY =  atof(guiLatSelection->value());
  bool         isUl    =  guiCenterPixel->value();

  m_Controller->ProjectRegion(sizeX, sizeY, spacingX, spacingY, originX, originY, isUl);

  // Project the image in the selected map projection
  m_Controller->ReprojectImage();

  // hide the GUI
  this->Hide();
}

/**
 *
 */
void
ProjectionView
::Hide()
{
  // hide the GUI
  guiMainWindow->hide();
}

/**
 *
 */
void
ProjectionView
::UpdateMapParam()
{
  ModelType::OutputPointType geoPoint, newCartoPoint;
  // Get the new values of Long/Lat from the gui
  geoPoint[0] = atof(guiLongSelection->value());
  geoPoint[1] = atof(guiLatSelection->value());

  // Project the new geo Point
  TransformType::Pointer rsTransform        = m_Controller->GetModel()->GetTransform();

  itk::OStringStream oss;
  oss << setiosflags(std::ios_base::fixed);
  oss.str("");

  switch (this->GetMapType())
    {
    case MAP_UTM:
      {
      newCartoPoint = rsTransform->GetTransform()->GetSecondTransform()->TransformPoint(geoPoint);
      oss.str("");
      oss << newCartoPoint[1];
      guiUTMNorthSelection->value(oss.str().c_str());
      oss.str("");
      oss << newCartoPoint[0];
      guiUTMEastSelection->value(oss.str().c_str());
      break;
      }
    case MAP_LAMBERT2:
      {
      newCartoPoint = rsTransform->GetTransform()->GetSecondTransform()->TransformPoint(geoPoint);
      oss.str("");
      oss << newCartoPoint[1];
      guiLambertNorthSelection->value(oss.str().c_str());
      oss.str("");
      oss << newCartoPoint[0];
      guiLambertEastSelection->value(oss.str().c_str());
      break;
      }
    case MAP_TRANSMERCATOR:
      {
      newCartoPoint = rsTransform->GetTransform()->GetSecondTransform()->TransformPoint(geoPoint);
      oss.str("");
      oss << newCartoPoint[1];
      guiTransmercatorNorthSelection->value(oss.str().c_str());
      oss.str("");
      oss << newCartoPoint[0];
      guiTransmercatorEastSelection->value(oss.str().c_str());
      break;
      }
    case MAP_WGS84:
      {
      break;
      }
    default:
      fl_alert("Problem with map projection type, please contact developpers");
      break;
    }
}

/**
 *
 */
void
ProjectionView
::UpdateLongLat()
{
  if (!m_InputProjectionUnknown)
    {

    IndexType index;
    PointType point, geoPoint;

    index.Fill(0);
    // Get the transform from the model
    TransformType::Pointer rsTransform = m_Controller->GetModel()->GetTransform();

    // Apply the transform to the middle point of the image
    if (guiCenterPixel->value() == 1 && guiULPixel->value() == 0)
      {
      index[0] = m_Controller->GetModel()->GetInputImage()->GetLargestPossibleRegion().GetSize()[0] / 2 + 1;
      index[1] = m_Controller->GetModel()->GetInputImage()->GetLargestPossibleRegion().GetSize()[1] / 2 + 1;

      }
    else if (guiULPixel->value() == 1 && guiCenterPixel->value() == 0)
      {
      index = m_Controller->GetModel()->GetInputImage()->GetLargestPossibleRegion().GetIndex();
      }

    // From index to Physical Point
    m_Controller->GetModel()->GetInputImage()->TransformIndexToPhysicalPoint(index, point);

    // Transform to geo and to Choosen projection
    geoPoint    = rsTransform->GetTransform()->GetFirstTransform()->TransformPoint(point);

    // Fill the datas in the GUI
    itk::OStringStream oss;
    oss << setiosflags(std::ios_base::fixed);
    oss.str("");
    oss << geoPoint[0];
    guiLongSelection->value(oss.str().c_str());
    oss.str("");
    oss << geoPoint[1];
    guiLatSelection->value(oss.str().c_str());

    //Update the Map parameters
    this->UpdateMapParam();
    }
}

/**
 * In case of unknown input map :
 */
void
ProjectionView
::UpdateUnavailableLongLat()
{
  IndexType index;
  PointType point, geoPoint;
  index.Fill(0);

  // Apply the transform to the middle point of the image
  if (guiCenterPixel->value() == 1 && guiULPixel->value() == 0)
    {
    index[0] = m_Controller->GetModel()->GetInputImage()->GetLargestPossibleRegion().GetSize()[0] / 2 + 1;
    index[1] = m_Controller->GetModel()->GetInputImage()->GetLargestPossibleRegion().GetSize()[1] / 2 + 1;

    }
  else if (guiULPixel->value() == 1 && guiCenterPixel->value() == 0)
    {
    index = m_Controller->GetModel()->GetInputImage()->GetLargestPossibleRegion().GetIndex();
    }

  // From index to Physical Point
  m_Controller->GetModel()->GetInputImage()->TransformIndexToPhysicalPoint(index, point);

  // Transform to geo and to Choosen projection
  geoPoint    = m_Controller->GetModel()->GetTransform()->GetTransform()->GetFirstTransform()->TransformPoint(point);

  // Fill the datas in the GUI
  itk::OStringStream oss;
  oss << setiosflags(std::ios_base::fixed);
  oss.str("");
  oss << geoPoint[0];
  guiLongSelection->value(oss.str().c_str());
  oss.str("");
  oss << geoPoint[1];
  guiLatSelection->value(oss.str().c_str());
}

/**
 *
 */

void
ProjectionView
::UpdateOutputParameters()
{
  itk::OStringStream oss;
  oss.str("");
  oss << m_Controller->GetModel()->GetOutputSpacing()[0];
  guiSpacingX->value(oss.str().c_str());
  oss.str("");
  oss << m_Controller->GetModel()->GetOutputSpacing()[1];
  guiSpacingY->value(oss.str().c_str());

  oss.str("");
  oss << m_Controller->GetModel()->GetOutputSize()[0];
  guiSizeX->value(oss.str().c_str());
  oss.str("");
  oss << m_Controller->GetModel()->GetOutputSize()[1];
  guiSizeY->value(oss.str().c_str());
}

int
ProjectionView
::UpdateInterpolator()
{
  int res = 0;

  switch (this->GetInterpolatorType())
    {
    case MAP_LINEAR_:
      {
      typedef itk::LinearInterpolateImageFunction<ImageType, double> LinearType;
       LinearType::Pointer interp = LinearType::New();
       m_Controller->GetModel()->GetResampler()->SetInterpolator(interp);
      break;
      }
    case MAP_NEAREST:
      {
//       typedef itk::NearestNeighborInterpolateImageFunction<SingleImageType, double> NearestType;
//       NearestType::Pointer interp = NearestType::New();
//       m_Controller->GetModel()->GetResampler()->SetInterpolator(interp);
      break;
      }
    case MAP_SINC:
      {
      itk::OStringStream oss;
      oss.str("");
      oss << guiSincRadius->value();
      if (strcmp("", oss.str().c_str()) != 0)
        {
        oss.str("");
        oss << guiSincWindow->value();
        switch (guiSincWindow->value())
          {
          case 0:
            {
            typedef WindowedSincInterpolateImageBlackmanFunction<SingleImageType> BlackmanType;
            BlackmanType::Pointer interp = BlackmanType::New();
            interp->SetRadius(static_cast<unsigned int>(guiSincRadius->value()));
            //interp->Initialize();
            //m_Controller->GetModel()->GetResampler()->SetInterpolator(interp);
            break;
            }
          case 1:
            {

            typedef WindowedSincInterpolateImageCosineFunction<SingleImageType> CosineType;
            CosineType::Pointer interp = CosineType::New();
            interp->SetRadius(static_cast<unsigned int>(guiSincRadius->value()));
            //interp->Initialize();
            //m_Controller->GetModel()->GetResampler()->SetInterpolator(interp);
            break;
            }
          case 2:
            {
            typedef WindowedSincInterpolateImageGaussianFunction<SingleImageType> GaussianType;
            GaussianType::Pointer interp = GaussianType::New();
            interp->SetRadius(static_cast<unsigned int>(guiSincRadius->value()));
            //interp->Initialize();
            //m_Controller->GetModel()->GetResampler()->SetInterpolator(interp);
            break;
            }
          case 3:
            {
            typedef WindowedSincInterpolateImageHammingFunction<SingleImageType> HammingType;
            HammingType::Pointer interp = HammingType::New();
            interp->SetRadius(static_cast<unsigned int>(guiSincRadius->value()));
            //interp->Initialize();
            //m_Controller->GetModel()->GetResampler()->SetInterpolator(interp);
            break;
            }
          case 4:
            {
            typedef WindowedSincInterpolateImageLanczosFunction<SingleImageType> LanczosType;
            LanczosType::Pointer interp = LanczosType::New();
            interp->SetRadius(static_cast<unsigned int>(guiSincRadius->value()));
            //interp->Initialize();
            //m_Controller->GetModel()->GetResampler()->SetInterpolator(interp);
            break;
            }
          case 5:
            {
            typedef WindowedSincInterpolateImageWelchFunction<SingleImageType> WelchType;
            WelchType::Pointer interp = WelchType::New();
            interp->SetRadius(static_cast<unsigned int>(guiSincRadius->value()));
            //interp->Initialize();
            //m_Controller->GetModel()->GetResampler()->SetInterpolator(interp);
            break;
            }
          default:
            fl_alert("Problem with interpolator type, please contact developpers");
            break;
          }
        }
      else
        {
        fl_alert("Invalid ionterpolator Radius...");
        }
      break;
      }
    case MAP_SPLINES:
      {
      typedef otb::BSplineInterpolateImageFunction<SingleImageType, double, double> SplineType;
      SplineType::Pointer interp = SplineType::New();
      interp->SetSplineOrder(static_cast<unsigned int>(guiSplineOrder->value()));
      //m_Controller->GetModel()->GetResampler()->SetInterpolator(interp);
      break;
      }
    default:
      fl_alert("Problem with map projection type, please contact developpers");
      res = 1;
      break;
    }
  return 0;
}

void
ProjectionView::InitializeAction()
{
  IndexType index;
  PointType middlePoint, geoPoint, outputPoint, outputPoint1;

  // Get the transform from the model
  TransformType::Pointer rsTransform = m_Controller->GetModel()->GetTransform();

  // Apply the transform to the middle point of the image
  index[0] = m_Controller->GetModel()->GetInputImage()->GetLargestPossibleRegion().GetSize()[0] / 2 + 1;
  index[1] = m_Controller->GetModel()->GetInputImage()->GetLargestPossibleRegion().GetSize()[1] / 2 + 1;

  // From index to Physical Point
  m_Controller->GetModel()->GetInputImage()->TransformIndexToPhysicalPoint(index, middlePoint);

  // Transform to geo projection
  geoPoint    = rsTransform->GetTransform()->GetFirstTransform()->TransformPoint(middlePoint);

  // Fill the datas in the GUI
  itk::OStringStream oss;
  oss << setiosflags(std::ios_base::fixed);
  oss.str("");
  oss << geoPoint[0];
  guiLongSelection->value(oss.str().c_str());
  oss.str("");
  oss << geoPoint[1];
  guiLatSelection->value(oss.str().c_str());

  // Update map parameters if inputTransform known
  if (!m_InputProjectionUnknown)
    {
    // Get the utm Zone && hemisphere  from a long,lat point in the image
    typedef UtmMapProjection<otb::FORWARD> utmMapProjectionType;
    utmMapProjectionType::Pointer utmProjection  =   utmMapProjectionType::New();
    int                           zone = utmProjection->GetZoneFromGeoPoint(geoPoint);
    oss.str("");
    oss << zone;
    guiUTMZone->value(oss.str().c_str());
    if (geoPoint[1] >= 0)
      {
      guiUTMNorth->value(1);
      guiUTMSouth->value(0);
      }
    else
      {
      guiUTMSouth->value(1);
      guiUTMNorth->value(0);
      }
    this->UpdateUTMTransform();
    // End of filling the utm parameters
    }

  // Transform middle point to carto projection : it is done here cause we wait for the
  // update of the transform following the utm zone found
  outputPoint = rsTransform->GetTransform()->GetSecondTransform()->TransformPoint(geoPoint);

  oss.str("");
  oss << outputPoint[0];
  guiUTMEastSelection->value(oss.str().c_str());
  oss.str("");
  oss << outputPoint[1];
  guiUTMNorthSelection->value(oss.str().c_str());

  // Update the output parameters
  this->UpdateOutputParameters();
}

void
ProjectionView::SetMapType(ProjectionMapType map)
{
  m_MapType = map;
  this->Modified();
}

ProjectionMapType
ProjectionView::GetMapType()
{
  return m_MapType;
}

void
ProjectionView::SetInterpolatorType(ProjectionInterpolatorType interp)
{
  m_InterpType = interp;
  this->UpdateInterpolator();
  this->Modified();
}

ProjectionInterpolatorType
ProjectionView::GetInterpolatorType()
{
  return m_InterpType;
}

void
ProjectionView::UpdateUTMTransform()
{
  //Get the projection parameters
  int  utmZone = atoi(guiUTMZone->value());
  bool north = guiUTMNorth->value();

  // Update the transform
  m_Controller->UpdateUTMTransform(utmZone, north);
}

/**
 * Tell the model to compute the LambertII transform
 * Done in the module cause the user is not allowed to modify this
 */
void
ProjectionView::UpdateLambertIITransform()
{
  m_Controller->InitializeLambertIITransform();
}

/**
 * Tell the model to compute the LambertII transform
 * Done in the module cause the user is not allowed to modify this
 */
void
ProjectionView::UpdateWGS84Transform()
{
  m_Controller->InitializeWGS84Transform();
}

/**
 *
 */
void
ProjectionView::UpdateTMTransform()
{
  m_Controller->UpdateTMTransform(atof(guiTRANSMERCATORScale->value()),
                                  atof(guiTRANSMERCATOREast->value()),
                                  atof(guiTRANSMERCATORNorth->value()));
}

void
ProjectionView::UpdateInputUTMTransform()
{
  //Get the projection parameters
  int  utmZone = atoi(iUTMZone->value());
  bool north = iUTMNorth->value();

  // Update the transform
  m_Controller->UpdateInputUTMTransform(utmZone, north);

  // Update the whole transformation
  this->UpToDateTransform();
}

/**
 * Tell the model to compute the LambertII transform
 * Done in the module cause the user is not allowed to modify this
 */
void
ProjectionView::UpdateInputLambertIITransform()
{
  m_Controller->InitializeInputLambertIITransform();

  // Update the whole transformation
  this->UpToDateTransform();
}

/**
 *
 */
void
ProjectionView::UpdateInputTMTransform()
{
  m_Controller->UpdateInputTMTransform(atof(iTRANSMERCATORScale->value()),
                                       atof(iTRANSMERCATOREast->value()),
                                       atof(iTRANSMERCATORNorth->value()));

  // Update the whole transformation
  this->UpToDateTransform();
}

/**
 * Update the output Information and the second transform
 */
void
ProjectionView::UpToDateTransform()
{
  switch (this->GetMapType())
    {
    case MAP_UTM:
      {
      this->UpdateUTMTransform();
      break;
      }
    case MAP_LAMBERT2:
      {
      this->UpdateLambertIITransform();
      break;
      }
    case MAP_TRANSMERCATOR:
      {
      this->UpdateTMTransform();
      break;
      }
    default:
      break;
    }
}

/**
 * Browse DEM
 */
void
ProjectionView::Browse()
{
  const char * demName = NULL;

  // Choose file
  demName = flu_dir_chooser(otbGetTextMacro("Choose the DEM dir..."), "");
  
  if (demName == NULL)
    {
    otbMsgDebugMacro(<< "Empty directory!");
    return;
    }
  
  vDEMPath->value(demName);
  std::string DEMfile = demName;
  
  m_Controller->SetDEMPath(DEMfile);
}

void
ProjectionView::UpdateDEMUse()
{
  // Update following the choiceDEM radio button
  // value
  if(choiceDEM->value())
    {
    vDEMPath->activate();
    bBrowse->activate();
    m_Controller->GetModel()->SetUseDEM(true);
    } 
  else
    {
    m_Controller->GetModel()->SetUseDEM(false);
    vDEMPath->deactivate();
    bBrowse->deactivate();
    }
}


} // End namespace otb
