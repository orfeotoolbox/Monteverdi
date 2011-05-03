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
#include "otbBCOInterpolateImageFunction.h"
#include "otbMsgReporter.h"
#include "otbUtmMapProjection.h"
#include "otbMapProjections.h"

#include "itkContinuousIndex.h"
#include <ogr_spatialref.h>

// compute the projected image quicklook
#include "otbImageToGenericRSOutputParameters.h"
#include "otbVectorData.h"
#include "otbVectorDataGlComponent.h"
#include "otbVectorDataProjectionFilter.h"
#include "otbFltkFilterWatcher.h"
#include "otbVectorDataFileWriter.h"

#include "otbStreamingShrinkImageFilter.h"

namespace otb
{
/**
 * Constructor
 */
ProjectionView::ProjectionView()
{
  // Build the Gui
  this->CreateGUI();
  pBusyBar->minimum(0);
  pBusyBar->maximum(1);

  // Initiliaze the GUI values
  guiUTMZone->value("31");
  guiTRANSMERCATOREast->value("0");
  guiTRANSMERCATORNorth->value("0");
  guiTRANSMERCATORScale->value("1");
  iTRANSMERCATOREast->value("0");
  iTRANSMERCATORNorth->value("0");
  iTRANSMERCATORScale->value("1");
  guiCenterPixel->value(1);
  guiULPixel->value(0);
  guiUserDefinition->value(0);
  m_InterpType = MAP_LINEAR_;
  m_MapType = MAP_UTM;
  m_PreviousMapType = MAP_UNKOWN;
  m_InputProjectionUnknown = false;
 
  m_QLImageLayerGeneratorComputed = false;
  m_QLImageLayerGenerator = LayerGeneratorType::New();
  m_QLImageRenderingModel = VisuModelType::New();
  m_HaveUpdatePreview = true;

  m_PreviewWidget =  ImageWidgetType::New();
  //init the previewWindow
  m_PreviewWidget->label("PreviewWidget");
  gPreviewWindow->add(m_PreviewWidget);
  gPreviewWindow->box(FL_NO_BOX);
  gPreviewWindow->resizable(gPreviewWindow);
  m_PreviewWidget->resize(gPreviewWindow->x(), gPreviewWindow->y(), gPreviewWindow->w(), gPreviewWindow->h() );
  m_Transform = ModelType::ResampleFilterType::New();
}

// Activate all the map parameters
void ProjectionView::ActivateMapParamaters()
{
  // activate all the MapType parameters in the GUI
  guiLongSelection->activate();
  guiLatSelection->activate();
  
  //   guiUTMZone->activate();
  //   guiUTMNorth->activate();
  //   guiUTMSouth->activate();

  //   guiEastSelection->activate();
  //   guiNorthSelection->activate();

  //   guiTRANSMERCATOREast->activate();
  //   guiTRANSMERCATORNorth->activate();
  //   guiTRANSMERCATORScale->activate();
  guiNorthSelection->activate();
  guiEastSelection->activate();
}

// Deactivate all the map parameters
void ProjectionView::DeactivateMapParamaters()
{
  // Deactivate all the MapType parameters in the GUI
  guiLongSelection->deactivate();
  guiLatSelection->deactivate();

  //   guiUTMZone->deactivate();
  //   guiUTMNorth->deactivate();
  //   guiUTMSouth->deactivate();

  //   guiEastSelection->deactivate();
  //   guiNorthSelection->deactivate();

  //   guiTRANSMERCATOREast->deactivate();
  //   guiTRANSMERCATORNorth->deactivate();
  //   guiTRANSMERCATORScale->deactivate();
  guiNorthSelection->deactivate();
  guiEastSelection->deactivate();
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
    // Update output whole origin image long/lat
    this->UpdateLongLat();
    // Update the origin cartographic coordinates
    this->UpdateOriginGeographicalCoordinates();
    // Update the output region
    this->UpdateOutputRegion();
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

  // If the Projection is not empty: Map Projection
  OGRSpatialReference oSRS;

  // Else perhaps a model sensor
  if (!inputProjRef.empty())
    {
    // From std::string to char*: Needed for the importFromWkt(char** projRef)
    char * inputProjchar = new char[inputProjRef.length() + 1];
    strcpy(inputProjchar, inputProjRef.c_str());

    // Import OGRSpatial Reference object from projectionRef
    // Warning: importFromWkt modifies the passed pointer !
    char* inputProjcharForImport = inputProjchar;
    oSRS.importFromWkt(&inputProjcharForImport);

    // First test if the node Projection exists: Avoid Segfaults when
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

  // Sensor Model:: Test if KeyWorldList is not empty
  if (m_Controller->GetModel()->GetInputImage()->GetImageKeywordlist().GetSize() > 0 && inputProjRef.empty())
    {
    // Select the Sensor Model in the InputMapType
    iMapSelection->value(3);
    iUTM->hide();
    iLambert2->hide();
    iTRANSMERCATOR->hide();
    }

  // Default Case: Kwl & ProjRef  empty: Give the user the possibility to fill the proj he needs
  if (m_Controller->GetModel()->GetInputImage()->GetImageKeywordlist().GetSize() == 0 && inputProjRef.empty())
    {
    iMapType->activate();
    iMapSelection->activate();
    m_InputProjectionUnknown = true;
    }
}

/**
 * In the OGR_SRSNode: Find the value of the parameter inParam if exists
 *
 */
bool
ProjectionView
::FindParameter(OGRSpatialReference oSRS, const char * inParam, double * paramValue)
{
  // Get all the nodes of the Projection Ref Tree
  // and search for a child named: "scale_factor"
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
    // First deactivate all the map paramters : default behavior
    this->DeactivateMapParamaters();
    // Notify the controller that the GUI is going to be shown
    // Note that call is useful for transform initialization and paramters initiaization
    // Get the projection initial parameters
    this->RetrieveInputProjection();
    // Default: utm is the output projection proposed
    this->UpdateUTMTransform();
    // Edit the GUI following the projection computed when UpdateUTMTransform Called
    this->InitializeAction();
    // Update the map param (UTM)
    this->UpdateMapParam();
    
    // finally show the GUI
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
 * Show the main gui
 */
void
ProjectionView
::UpdateOutputRegion()
{
 m_HaveUpdatePreview = true;

  // tell the model to project the region edited in the GUI
  unsigned int sizeX    = atoi(guiSizeX->value());
  unsigned int sizeY    = atoi(guiSizeY->value());
  double       spacingX = atof(guiSpacingX->value());
  double       spacingY = atof(guiSpacingY->value());
  double       originX =  atof(guiLongSelection->value());
  double       originY =  atof(guiLatSelection->value());

  m_Controller->ProjectRegion(sizeX, sizeY, spacingX, spacingY, originX, originY);
}


/**
 *
 */
void
ProjectionView
::OK()
{
  // Tell the model to update its region
  this->UpdateOutputRegion();

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
 m_HaveUpdatePreview = true;

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
    guiNorthSelection->value(oss.str().c_str());
    oss.str("");
    oss << newCartoPoint[0];
    guiEastSelection->value(oss.str().c_str());
    break;
    }
    case MAP_LAMBERT2:
    {
    newCartoPoint = rsTransform->GetTransform()->GetSecondTransform()->TransformPoint(geoPoint);
    oss.str("");
    oss << newCartoPoint[1];
    guiNorthSelection->value(oss.str().c_str());
    oss.str("");
    oss << newCartoPoint[0];
    guiEastSelection->value(oss.str().c_str());
    break;
    }
    case MAP_TRANSMERCATOR:
    {
    newCartoPoint = rsTransform->GetTransform()->GetSecondTransform()->TransformPoint(geoPoint);
    oss.str("");
    oss << newCartoPoint[1];
    guiNorthSelection->value(oss.str().c_str());
    oss.str("");
    oss << newCartoPoint[0];
    guiEastSelection->value(oss.str().c_str());
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
  
  // Tell the model that the output region has changed since the
  // output geographical coordinates changes the cartographic orign in
  // the  user defined mode
  if(guiUserDefinition->value())
    {
    this->UpdateOutputRegion();
    }
}

/**
 *
 */
void
ProjectionView
::UpdateLongLat()
{
  m_HaveUpdatePreview = true;

  if (!m_InputProjectionUnknown)
    {
    PointType point, geoPoint;

    // Get the transform from the model
    TransformType::Pointer rsTransform = m_Controller->GetModel()->GetInverseTransform();

    // Do that if the Mode User Definition is not set
    if (guiUserDefinition->value() == 0)
      {
      // Set the index to be the center of the ROI selected by the user
      if (guiCenterPixel->value() == 1 && guiULPixel->value() == 0)
        {
        // Get the cartographic middle point
        point[0] = m_Controller->GetModel()->GetWholeOutputOrigin()[0]
          + m_Controller->GetModel()->GetWholeOutputSize()[0]* m_Controller->GetModel()->GetWholeOutputSpacing()[0]/2;
      
        point[1] = m_Controller->GetModel()->GetWholeOutputOrigin()[1]
          + m_Controller->GetModel()->GetWholeOutputSize()[1]* m_Controller->GetModel()->GetWholeOutputSpacing()[1]/2;
      
        // Get the cartographic coordinates of the UL point of the
        // region centered on the output middle point
        point[0] -= atoi(guiSizeX->value())*atof(guiSpacingX->value()) / 2.;
        point[1] -= atoi(guiSizeY->value())*atof(guiSpacingY->value()) / 2.;
        }
      else if (guiULPixel->value() == 1 && guiCenterPixel->value() == 0)
        {
        // Get the output Cartographic origin
        point[0] = m_Controller->GetModel()->GetWholeOutputOrigin()[0];
        point[1] = m_Controller->GetModel()->GetWholeOutputOrigin()[1];
        }
    
      // Transform to geo
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
    else
      {
      this->UpdateOriginGeographicalCoordinates();
      }
    }
  
  // Common to both modes
  // Update the output origin in the model
  this->UpdateOutputRegion();
}

/**
 * In case of unknown input map:
 */
void
ProjectionView
::UpdateUnavailableLongLat()
{
 m_HaveUpdatePreview = true;

  PointType point, geoPoint;

  // Apply the transform to the middle point of the image
  if (guiCenterPixel->value() == 1 && guiULPixel->value() == 0)
    {
    // Get the cartographic middle point
    point[0] = m_Controller->GetModel()->GetWholeOutputOrigin()[0]
      + m_Controller->GetModel()->GetWholeOutputSize()[0]* m_Controller->GetModel()->GetWholeOutputSpacing()[0]/2;
    
    point[1] = m_Controller->GetModel()->GetWholeOutputOrigin()[1]
      + m_Controller->GetModel()->GetWholeOutputSize()[1]* m_Controller->GetModel()->GetWholeOutputSpacing()[1]/2;
    
    // Get the cartographic coordinates of the UL point of the
    // region centered on the output middle point
    point[0] -= atoi(guiSizeX->value())*atof(guiSpacingX->value()) / 2;
    point[1] -= atoi(guiSizeY->value())*atof(guiSpacingY->value()) / 2;
    }
  else if (guiULPixel->value() == 1 && guiCenterPixel->value() == 0)
    {
    //index = m_Controller->GetModel()->GetInputImage()->GetLargestPossibleRegion().GetIndex();
    point[0] = m_Controller->GetModel()->GetWholeOutputOrigin()[0];
    point[1] = m_Controller->GetModel()->GetWholeOutputOrigin()[1];
    }

  // From index to Physical Point
  //m_Controller->GetModel()->GetInputImage()->TransformIndexToPhysicalPoint(index, point);

  // Transform to geo and to Choosen projection
  geoPoint    = m_Controller->GetModel()->GetInverseTransform()->GetTransform()->GetFirstTransform()->TransformPoint(point);

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
 m_HaveUpdatePreview = true;

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
 m_HaveUpdatePreview = true;

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
    typedef itk::NearestNeighborInterpolateImageFunction<ImageType, double> NearestType;
    NearestType::Pointer interp = NearestType::New();
    m_Controller->GetModel()->GetResampler()->SetInterpolator(interp);
    break;
    }
    case MAP_BCO:
    {
    typedef BCOInterpolateImageFunction<ImageType, double> BCOType;
    BCOType::Pointer interp = BCOType::New();
    interp->SetRadius(static_cast<unsigned int>(guiBCORadius->value()));
    m_Controller->GetModel()->GetResampler()->SetInterpolator(interp);
    break;
    }
//     case MAP_SINC:
//       {
//       itk::OStringStream oss;
//       oss.str("");
//       oss << guiSincRadius->value();
//       if (strcmp("", oss.str().c_str()) != 0)
//         {
//         oss.str("");
//         oss << guiSincWindow->value();
//         switch (guiSincWindow->value())
//           {
//           case 0:
//             {
//             typedef WindowedSincInterpolateImageBlackmanFunction<SingleImageType> BlackmanType;
//             BlackmanType::Pointer interp = BlackmanType::New();
//             interp->SetRadius(static_cast<unsigned int>(guiSincRadius->value()));
//             //interp->Initialize();
//             //m_Controller->GetModel()->GetResampler()->SetInterpolator(interp);
//             break;
//             }
//           case 1:
//             {

//             typedef WindowedSincInterpolateImageCosineFunction<SingleImageType> CosineType;
//             CosineType::Pointer interp = CosineType::New();
//             interp->SetRadius(static_cast<unsigned int>(guiSincRadius->value()));
//             //interp->Initialize();
//             //m_Controller->GetModel()->GetResampler()->SetInterpolator(interp);
//             break;
//             }
//           case 2:
//             {
//             typedef WindowedSincInterpolateImageGaussianFunction<SingleImageType> GaussianType;
//             GaussianType::Pointer interp = GaussianType::New();
//             interp->SetRadius(static_cast<unsigned int>(guiSincRadius->value()));
//             //interp->Initialize();
//             //m_Controller->GetModel()->GetResampler()->SetInterpolator(interp);
//             break;
//             }
//           case 3:
//             {
//             typedef WindowedSincInterpolateImageHammingFunction<SingleImageType> HammingType;
//             HammingType::Pointer interp = HammingType::New();
//             interp->SetRadius(static_cast<unsigned int>(guiSincRadius->value()));
//             //interp->Initialize();
//             //m_Controller->GetModel()->GetResampler()->SetInterpolator(interp);
//             break;
//             }
//           case 4:
//             {
//             typedef WindowedSincInterpolateImageLanczosFunction<SingleImageType> LanczosType;
//             LanczosType::Pointer interp = LanczosType::New();
//             interp->SetRadius(static_cast<unsigned int>(guiSincRadius->value()));
//             //interp->Initialize();
//             //m_Controller->GetModel()->GetResampler()->SetInterpolator(interp);
//             break;
//             }
//           case 5:
//             {
//             typedef WindowedSincInterpolateImageWelchFunction<SingleImageType> WelchType;
//             WelchType::Pointer interp = WelchType::New();
//             interp->SetRadius(static_cast<unsigned int>(guiSincRadius->value()));
//             //interp->Initialize();
//             //m_Controller->GetModel()->GetResampler()->SetInterpolator(interp);
//             break;
//             }
//           default:
//             fl_alert("Problem with interpolator type, please contact developpers");
//             break;
//           }
//         }
//       else
//         {
//         fl_alert("Invalid ionterpolator Radius...");
//         }
//       break;
//       }
//     case MAP_SPLINES:
//       {
//       typedef otb::BSplineInterpolateImageFunction<SingleImageType, double, double> SplineType;
//       SplineType::Pointer interp = SplineType::New();
//       interp->SetSplineOrder(static_cast<unsigned int>(guiSplineOrder->value()));
//       //m_Controller->GetModel()->GetResampler()->SetInterpolator(interp);
//       break;
//       }
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
  PointType middlePoint, geoPoint;

  // Get the transform from the model
  TransformType::Pointer rsTransform = m_Controller->GetModel()->GetInverseTransform();

  // Get the cartographic middle middlePoint
  middlePoint[0] = m_Controller->GetModel()->GetWholeOutputOrigin()[0]
    + m_Controller->GetModel()->GetWholeOutputSize()[0]* m_Controller->GetModel()->GetWholeOutputSpacing()[0]/2;
  
  middlePoint[1] = m_Controller->GetModel()->GetWholeOutputOrigin()[1]
    + m_Controller->GetModel()->GetWholeOutputSize()[1]* m_Controller->GetModel()->GetWholeOutputSpacing()[1]/2;

  // Get the UL corner cartographic coordinates of the
  // region centered on the whole output middlePoint
  middlePoint[0] -= atoi(guiSizeX->value())*atof(guiSpacingX->value()) / 2;
  middlePoint[1] -= atoi(guiSizeY->value())*atof(guiSpacingY->value()) / 2;

  // Transform to geo projection
  geoPoint = rsTransform->GetTransform()->GetFirstTransform()->TransformPoint(middlePoint);

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
    // Get the utm Zone && hemisphere  from a long, lat point in the image
    UtmForwardProjection::Pointer utmProjection = UtmForwardProjection::New();


    int zone = Utils::GetZoneFromGeoPoint(geoPoint[0], geoPoint[1]);
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
 m_HaveUpdatePreview = true; 

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
 m_HaveUpdatePreview = true;

  m_Controller->InitializeLambertIITransform();
}

/**
 * Tell the model to compute the LambertII transform
 * Done in the module cause the user is not allowed to modify this
 */
void
ProjectionView::UpdateWGS84Transform()
{
 m_HaveUpdatePreview = true;

  m_Controller->InitializeWGS84Transform();
}

/**
 *
 */
void
ProjectionView::UpdateTMTransform()
{
 m_HaveUpdatePreview = true;

  m_Controller->UpdateTMTransform(atof(guiTRANSMERCATORScale->value()),
                                  atof(guiTRANSMERCATOREast->value()),
                                  atof(guiTRANSMERCATORNorth->value()));
}

void
ProjectionView::UpdateInputUTMTransform()
{
 m_HaveUpdatePreview = true;

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
 m_HaveUpdatePreview = true;

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
 m_HaveUpdatePreview = true;

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
 m_HaveUpdatePreview = true;

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
 m_HaveUpdatePreview = true;

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


void
ProjectionView::UpdateRpcEstimation()
{
 m_HaveUpdatePreview = true;

  // Update following the choiceDEM radio button
  // value
  if(guiRPCEstimator->value())
    m_Controller->GetModel()->SetEstimateInputRPCModel(true);
  else
    m_Controller->GetModel()->SetEstimateInputRPCModel(false);
}

void
ProjectionView::DisplayPreviewWidget()
{
  if(!m_Controller->GetModel()->GetInputImage()->GetProjectionRef().empty()
     ||m_Controller->GetModel()->GetInputImage()->GetImageKeywordlist().GetSize()> 0)
    {
    // Get the current output projectionRef computed in the model part
    std::string outputMap = m_Controller->GetModel()->GetOutputProjectionRef();
       
    // Get the previewWidget size
    SizeType previewSize;
    previewSize[0] = gPreviewWindow->w();
    previewSize[1] = gPreviewWindow->h();
    
    // Estimate the output parmaters by forcing the output size to the
    // widget size
    typedef otb::ImageToGenericRSOutputParameters<ImageType>   OutputParametersEstimatorType;
    OutputParametersEstimatorType::Pointer  estimator = OutputParametersEstimatorType::New();
    
    estimator->SetInput(m_Controller->GetModel()->GetInputImage());
    estimator->SetOutputProjectionRef(outputMap);
    estimator->ForceSizeTo(previewSize);
    estimator->Compute();
    
    // Clear the previous GlComponents
    m_PreviewWidget->ClearGlComponents();

    // Display the preview image only if the check box is selected
    if(bDispalyPreview->value())
      {
        if(  m_HaveUpdatePreview == true )
          {
            // Clear the previous GlComponents
            //m_PreviewWidget->ClearGlComponents();
            // Clear the previous widget buffer
            m_PreviewWidget->ClearBuffer();

            // Compute input rendering function
            if( m_QLImageLayerGeneratorComputed == false )
              {
                // Show busy bar
                this->pBusyBar->copy_label("Compute input histogram");
                this->pBusyBar->value(1);
                this->pBusyBar->show();
                Fl::check();
                
                m_QLImageLayerGenerator = LayerGeneratorType::New();
                m_QLImageLayerGenerator->SetImage(m_Controller->GetModel()->GetInputImage());
                m_QLImageLayerGenerator->GenerateLayer();
                
                m_QLImageRenderingModel->AddLayer(m_QLImageLayerGenerator->GetLayer());
                m_QLImageRenderingModel->Update();
                
                m_QLImageLayerGenerator->GetRenderingFunction()->SetAutoMinMax(false);
                m_QLImageLayerGeneratorComputed = true;
                
                // Show busy bar
                this->pBusyBar->value(1);
                this->pBusyBar->show();
                this->pBusyBar->copy_label("Computing Quicklook");
                Fl::check();
              }
            
            // Two cases when the Display button is set to ON :
            // - The mapType changed : reproject
            // - The mapType does not changed : use the previous projected
            // image
            if(m_PreviousMapType != this->GetMapType())
              {
                // Reproject the image with a final size equal to the previewWidget
                // size  and relative spacing
                m_Transform->SetInput(m_Controller->GetModel()->GetInputImage());
                m_Transform->SetOutputProjectionRef(outputMap);
                m_Transform->SetOutputOrigin(estimator->GetOutputOrigin());
                m_Transform->SetOutputSpacing(estimator->GetOutputSpacing());
                m_Transform->SetOutputSize(estimator->GetOutputSize());
                m_Transform->SetDeformationFieldSpacing(10.*estimator->GetOutputSpacing());
                m_Transform->SetEdgePaddingValue(0);
                
                // Update the MapType
                m_PreviousMapType = this->GetMapType();
              }
            
            // Streaming Shrink : Persistent class, will process tile by tile
            // to avoid memory allocation issues.
            typedef StreamingShrinkImageFilter<ImageType, ImageType>   StreamingShrinkType;
            StreamingShrinkType::Pointer shrinker = StreamingShrinkType::New();
            shrinker->SetInput(m_Transform->GetOutput());
            shrinker->SetShrinkFactor(1);
            shrinker->GetStreamer()->SetAutomaticStrippedStreaming(0);
            
            // Show busy bar
            this->pBusyBar->value(1);
            this->pBusyBar->show();
            Fl::check();
            
            shrinker->Update();
            
            // build the rendering model
            // Generate the layer
            LayerGeneratorType::Pointer layerGenerator = LayerGeneratorType::New();
            layerGenerator->SetImage(shrinker->GetOutput());
            layerGenerator->SetRenderingFunction(m_QLImageLayerGenerator->GetRenderingFunction() );
            FltkFilterWatcher qlwatcher(layerGenerator->GetProgressSource(), 0, 0, 200, 20,"Generating QuickLook ...");
            layerGenerator->GenerateLayer();
            
            // Render
            VisuModelType::Pointer rendering = VisuModelType::New();
            rendering->AddLayer(layerGenerator->GetLayer());
            rendering->Update();
            
            // Hide busy bar
            this->pBusyBar->value(0);
            this->pBusyBar->hide();
            Fl::check();
            
            // Fill the previewWidget with the quicklook of the projected
            // image
            ViewerImageType * quickLook = rendering->GetRasterizedQuicklook();
            m_PreviewWidget->ReadBuffer(quickLook, quickLook->GetLargestPossibleRegion());
            
            m_HaveUpdatePreview = false;
         }
        else
          {
            m_PreviewWidget->show();
            gPreviewWindow->show();
            bDispalyPreview->show();
          }
      }
    else
      {
        // Clear the previous widget buffer
        m_PreviewWidget->ClearBuffer();
        
        // Create a temp image : cause no vectordataGlComponent
        // rendering without an image layer.
        ViewerImageType::Pointer  tempImage  = ViewerImageType::New();
        ImageType::RegionType            tempRegion;
        ImageType::RegionType::IndexType index;
        index.Fill(0);
        tempRegion.SetIndex(index);
        tempRegion.SetSize(estimator->GetOutputSize());
        
        tempImage->SetRegions(tempRegion);
        tempImage->Allocate();
        tempImage->FillBuffer(0.);
        
        // Fill the previewWidget with the temp black image
        m_PreviewWidget->ReadBuffer(tempImage, tempRegion);
      }
    
    // Compose the VectorDataGLComponent (Region Selected by the user)
    // Build the selected polygon vector data
    typedef otb::VectorData<>              VectorDataType;
    typedef VectorDataType::DataNodeType   DataNodeType;
    typedef VectorDataType::PolygonType    PolygonType;
    
    VectorDataType::Pointer    userROI   = VectorDataType::New();
    DataNodeType::Pointer document = DataNodeType::New();
    DataNodeType::Pointer folder = DataNodeType::New();
    DataNodeType::Pointer polygonNode = DataNodeType::New();
    
    document->SetNodeType(otb::DOCUMENT);
    folder->SetNodeType(otb::FOLDER);
    DataNodeType::Pointer root = userROI->GetDataTree()->GetRoot()->Get();
    userROI->GetDataTree()->Add(document, root);
    userROI->GetDataTree()->Add(folder, document);
    userROI->GetDataTree()->Add(polygonNode, folder);
    userROI->SetProjectionRef(outputMap);

    // build the 4 corners of the selected roi
    PolygonType::VertexType  p1, p2, p3, p4;
    p1[0] = m_Controller->GetModel()->GetOutputOrigin()[0];
    p1[1] = m_Controller->GetModel()->GetOutputOrigin()[1];

    p2[0] = m_Controller->GetModel()->GetOutputOrigin()[0] + atoi(guiSizeX->value())*atof(guiSpacingX->value());
    p2[1] = m_Controller->GetModel()->GetOutputOrigin()[1];

    p3[0] = m_Controller->GetModel()->GetOutputOrigin()[0] + atoi(guiSizeX->value())*atof(guiSpacingX->value());
    p3[1] = m_Controller->GetModel()->GetOutputOrigin()[1] + atoi(guiSizeY->value())*atof(guiSpacingY->value());

    p4[0] = m_Controller->GetModel()->GetOutputOrigin()[0];
    p4[1] = m_Controller->GetModel()->GetOutputOrigin()[1] + atoi(guiSizeY->value())*atof(guiSpacingY->value());

    //Build the VectorData
    PolygonType::Pointer polygon = PolygonType::New();
    polygon->AddVertex(p1);
    polygon->AddVertex(p2);
    polygon->AddVertex(p3);
    polygon->AddVertex(p4);
    polygonNode->SetPolygonExteriorRing(polygon);
    
    // Build the VectorDataGlComponent
    typedef VectorDataGlComponent<VectorDataType>   VectorDataGlComponentType;
    VectorDataGlComponentType::Pointer glComp = VectorDataGlComponentType::New();
    glComp->SetVectorData(userROI);
    glComp->SetVisible(true);
    glComp->SetRenderPolygonBoundariesOnly(true);
    glComp->SetSpacing(estimator->GetOutputSpacing());
    glComp->SetOrigin(estimator->GetOutputOrigin());
    
    // Set the color to green
    VectorDataGlComponentType::ColorType color = glComp->GetColor();
    color[0]= 0.;
    color[1]= 225.;
    color[2]=0.;
    glComp->SetColor(color);
    
    // Add the GlComponent to the PreviewWidget
    m_PreviewWidget->AddGlComponent(glComp);

    // Show the preview image
    m_PreviewWidget ->show();
    m_PreviewWidget->redraw();
    }
  else
    {
    gPreviewWindow->hide();
    }
}

void ProjectionView::TabPositionHandler()
{
  if( m_TabsMode->value() == gQuickLook )
    {
    gPreviewWindow->show();
    bDispalyPreview->show();
    this->DisplayPreviewWidget();
    }
  else
    {
    m_PreviewWidget->hide();
    gPreviewWindow->hide();
    bDispalyPreview->hide();
    }
}

// Used to update the geographic origin of the output
// image when changing the cartographic coordinates
void ProjectionView::UpdateOriginGeographicalCoordinates()
{
 m_HaveUpdatePreview = true;

  ModelType::OutputPointType  cartoPoint;
  ModelType::OutputPointType  geoPoint;
  
  // Get the Transform from the model in order to project the new
  // carto point in WGS84 and update the Long/Lat fields
  TransformType::Pointer rsTransform  = m_Controller->GetModel()->GetInverseTransform();

  itk::OStringStream oss;
  oss << setiosflags(std::ios_base::fixed);
  oss.str("");

  switch (this->GetMapType())
    {
    case MAP_UTM:
    {
    cartoPoint[1] = atof(guiNorthSelection->value());
    cartoPoint[0] = atof(guiEastSelection->value());
    break;
    }
    case MAP_LAMBERT2:
    {
    cartoPoint[1] = atof(guiNorthSelection->value());
    cartoPoint[0] = atof(guiEastSelection->value());
    break;
    }
    case MAP_TRANSMERCATOR:
    {
    cartoPoint[1] = atof(guiNorthSelection->value());
    cartoPoint[0] = atof(guiEastSelection->value());
    break;
    }
    case MAP_WGS84:
    {
    // Since no WGS84 param are available in the GUI,
    //
    cartoPoint[1] = atof(guiLatSelection->value());
    cartoPoint[0] = atof(guiLongSelection->value());
    break;
    }
    default:
      fl_alert("Problem with map projection type, please contact developpers");
      break;
    }
  
  // Project the carto point in WGS84 using the transform set in
  // the model
  geoPoint = rsTransform->GetTransform()->GetFirstTransform()->TransformPoint(cartoPoint);
  
  // Update the GUI
  oss.str("");
  oss << geoPoint[1];
  guiLatSelection->value(oss.str().c_str());
  oss.str("");
  oss << geoPoint[0];
  guiLongSelection->value(oss.str().c_str());

  // Update output region
  // tell the model that the output region has been modified
  unsigned int sizeX    = atoi(guiSizeX->value());
  unsigned int sizeY    = atoi(guiSizeY->value());
  double       spacingX = atof(guiSpacingX->value());
  double       spacingY = atof(guiSpacingY->value());
  m_Controller->ProjectRegion(sizeX, sizeY, spacingX, spacingY, geoPoint[0], geoPoint[1]);
}


}// End namespace otb
