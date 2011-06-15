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
#include "otbDEMToImageGeneratorModule.h"

#include <FL/Fl_Text_Buffer.H>
#include <FL/fl_ask.H>
#include <FLU/Flu_File_Chooser.h>
#include <FL/Enumerations.H>

#include "otbMacro.h"
#include "otbPerBandVectorImageFilter.h"
#include "itkExceptionObject.h"

#include "otbMsgReporter.h"
#include "otbImageKeywordlist.h"

#include "itkContinuousIndex.h"
#include <ogr_spatialref.h>

#include "otbUtils.h"
#include "otbMapProjections.h"

namespace otb
{
/**
 * Constructor
 */
DEMToImageGeneratorView::DEMToImageGeneratorView()
{
  // Build the Gui
  this->CreateGUI();
  m_InputImageUnknown = false;
}

/**
 * Handle the notifications of the model
 */
void
DEMToImageGeneratorView::Notify()
{
#if 0
  if (m_Controller->GetModel()->GetTransformChanged())
    {
    // Update the cartographic parameters
    //this->UpdateMapParam();
    // Update the output
    //this->UpdateOutputParameters();
    }

  if (m_Controller->GetModel()->GetTempTransformChanged())
    {
    m_InputImageUnknown = false;
    //this->UpdateUnavailableLongLat();
    }
#endif
}


/**
 *
 */
void
DEMToImageGeneratorView
::Show()
{
  m_InputImageUnknown = m_Controller->GetModel()->GetUseInputImage();
  gOutputImage->show();

  try
    {
    if (m_InputImageUnknown)
      {
      this->UpdateOutputParameters();
      gOutputParam->deactivate();
      }

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
DEMToImageGeneratorView
::ShowGUI()
{
  m_ColorBarWidget = ColorBarWidgetType::New();
  m_ColorBarWidget->Init(oColorBar->x(), oColorBar->y(),
                   oColorBar->w(), oColorBar->h(),"Color Bar");
  oColorBar->add(m_ColorBarWidget);
  oColorBar->box(FL_NO_BOX);
  m_ColorBarWidget->SetColormap(m_Controller->GetModel()->GetColormap());
  m_ColorBarWidget->show();
  m_ColorBarWidget->redraw();

  guiMainWindow->show();
}

/**
 *
 */
void
DEMToImageGeneratorView
::SaveAndQuit()
{
  // tell the model to project the region edited in the GUI
  double       originX =  atof(oOriginX->value());
  double       originY =  atof(oOriginY->value());
  unsigned int sizeX    = atoi(oSizeX->value());
  unsigned int sizeY    = atoi(oSizeY->value());
  double       spacingX = atof(oSpacingX->value());
  double       spacingY = atof(oSpacingY->value());

  std::string srtmDirectory = std::string(iDEMPath->value());
  m_Controller->SetDEMDirectoryPath(srtmDirectory.c_str());
  m_Controller->GetModel()->GetDEMToImageGenerator()->InstanciateTransform();

  m_Controller->ProjectRegion(sizeX, sizeY, spacingX, spacingY, originX, originY);

  // Project the image in the selected map projection
  m_Controller->ReprojectImage();

  // Evaluate Hill shading in necessary
  if(oHillShadingAsOutput->value() == 1)
    {
    double       azimutAngle    = oAzimutLight->value();
    double       elevationAngle = oElevationLight->value();
    unsigned int radius         = oRadiusHillShading->value();


    m_Controller->ProcessHillShading( azimutAngle, elevationAngle, radius);

    // Display the color relief
    if(oColorRelief->value() == 1)
      {
      double min = oMinValue->value();
      double max = oMaxValue->value();
      m_Controller->ProcessColorRelief(min, max, oWithHillShading->value());
      }
    }

  // hide the GUI
  this->Hide();
}


void
DEMToImageGeneratorView::
Cancel()
{
  guiMainWindow->hide();
}


/**
 *
 */
void
DEMToImageGeneratorView
::Hide()
{
  // hide the GUI
  guiMainWindow->hide();
}


/**
 *
 */

void
DEMToImageGeneratorView
::UpdateOutputParameters()
{
  itk::OStringStream oss;

  oss.str("");
  oss << m_Controller->GetModel()->GetOutputOrigin()[0];
  oOriginX->value(oss.str().c_str());

  oss.str("");
  oss << m_Controller->GetModel()->GetOutputOrigin()[1];
  oOriginY->value(oss.str().c_str());

  oss.str("");
  oss << m_Controller->GetModel()->GetOutputSpacing()[0];
  oSpacingX->value(oss.str().c_str());

  oss.str("");
  oss << m_Controller->GetModel()->GetOutputSpacing()[1];
  oSpacingY->value(oss.str().c_str());

  oss.str("");
  oss << m_Controller->GetModel()->GetOutputSpacingMeter()[0];
  oSpacingXm->value(oss.str().c_str());

  oss.str("");
  oss << m_Controller->GetModel()->GetOutputSpacingMeter()[1];
  oSpacingYm->value(oss.str().c_str());

  oss.str("");
  oss << m_Controller->GetModel()->GetOutputSize()[0];
  oSizeX->value(oss.str().c_str());

  oss.str("");
  oss << m_Controller->GetModel()->GetOutputSize()[1];
  oSizeY->value(oss.str().c_str());
}


void
DEMToImageGeneratorView::UpdateProjectionRef()
{
  m_Controller->GetModel()->GetDEMToImageGenerator()->UpdateOutputInformation();
}


/**
 * Browse DEM
 */
void
DEMToImageGeneratorView::BrowseSRTMDEM()
{
  const char * srtmDirectory = NULL;

  // Choose file
  srtmDirectory = flu_dir_chooser(otbGetTextMacro("Choose the SRTM DEM directory ..."), "");
  
  if (srtmDirectory  == NULL)
    {
    otbMsgDebugMacro(<< "No directory !");
    return;
    }
  
  iDEMPath->value(srtmDirectory);
  m_Controller->SetDEMDirectoryPath(srtmDirectory);
}

void DEMToImageGeneratorView::UpdateHillShading()
{
  if (oHillShadingAsOutput->value() == 1)
    {
    oAzimutLight->activate();
    oElevationLight->activate();
    oRadiusHillShading->activate();
    oWithHillShading->activate();
    }
  else
    {
    oAzimutLight->deactivate();
    oElevationLight->deactivate();
    oRadiusHillShading->deactivate();
    oWithHillShading->deactivate();
    }
}

void DEMToImageGeneratorView::UpdateSpacing()
{
  if ( (oSizeX->value() == 0) || (oSizeY->value() == 0) )
    {
    return;
    }

  typedef TransformType::OutputPointType OutputPointType;
  OutputPointType latLongPointUL, latLongPointLR;
  latLongPointUL[0] = strtod(oOriginX->value(), NULL);
  latLongPointUL[1] = strtod(oOriginY->value(), NULL);

  int utmZoneUL = Utils::GetZoneFromGeoPoint(latLongPointUL[0], latLongPointUL[1]);

  // Project the new geo Point
  OutputPointType cartoPointUL, cartoPointLR;
  TransformType::Pointer rsTransform = TransformType::New();

  typedef UtmInverseProjection UtmProjectionType;
  UtmProjectionType::Pointer utmProjection = UtmProjectionType::New();

  std::string hem = "N";
  if(latLongPointUL[0] < 0)
    {
    hem = "S";
    }

  utmProjection->SetHemisphere(hem[0]);
  utmProjection->SetZone(utmZoneUL);
  std::string outputProjectionRef = utmProjection->GetWkt();

  char * wgs84Ref;
  OGRSpatialReference oSRS;
  oSRS.SetWellKnownGeogCS("WGS84");
  oSRS.exportToWkt(&wgs84Ref);
  std::string inputProjectionRef = wgs84Ref;

  // Build a Generic RS transform WGS84 -> UTM
  rsTransform->SetInputProjectionRef(inputProjectionRef);
  rsTransform->SetOutputProjectionRef(outputProjectionRef);
  rsTransform->InstanciateTransform();

  cartoPointUL = rsTransform->GetTransform()->GetSecondTransform()->TransformPoint(latLongPointUL);
  //std::cout << latLongPointUL <<" -> " << cartoPointUL <<std::endl;

  double sizeX = strtod(oSizeX->value(), NULL);
  double sizeY = strtod(oSizeY->value(), NULL);

  double spacingXm = strtod(oSpacingXm->value(), NULL);
  double spacingYm = strtod(oSpacingYm->value(), NULL);

  cartoPointLR[0] = cartoPointUL[0] + sizeX * spacingXm;
  cartoPointLR[1] = cartoPointUL[1] + sizeY * spacingYm;

  TransformType::Pointer rsInverseTransform = TransformType::New();
  rsTransform->GetInverse(rsInverseTransform);
  latLongPointLR = rsInverseTransform->GetTransform()->GetFirstTransform()->TransformPoint(cartoPointLR);

  //std::cout << cartoPointLR << " -> "<< latLongPointLR << std::endl;

  if (!Utils::IsLonLatValid(latLongPointLR[0], latLongPointLR[1]))
    {
    return;
    }
  int utmZoneLR = Utils::GetZoneFromGeoPoint(latLongPointLR[0], latLongPointLR[1]);
  if (utmZoneUL != utmZoneLR)
    {
    return;
    }

  double spacingX = (latLongPointLR[0] - latLongPointUL[0]) / sizeX;
  double spacingY = (latLongPointLR[1] - latLongPointUL[1]) / sizeY;

  itk::OStringStream oss;
  oss.str("");
  oss << spacingX;
  oSpacingX->value(oss.str().c_str());
  oss.str("");
  oss << spacingY;
  oSpacingY->value(oss.str().c_str());

}

void DEMToImageGeneratorView::UpdateSpacingMeter()
{
  if ( (atoi(oSizeX->value()) == 0) || (atoi(oSizeY->value()) == 0) )
    {
    return;
    }

  typedef TransformType::OutputPointType OutputPointType;

  OutputPointType latLongPointUL;
  latLongPointUL[0] = strtod(oOriginX->value(), NULL);
  latLongPointUL[1] = strtod(oOriginY->value(), NULL);

  OutputPointType latLongPointLR;
  if (latLongPointUL[0] > 0)
    {
    latLongPointLR[0] = latLongPointUL[0]+ strtod(oSizeX->value(), NULL) * strtod(oSpacingX->value(), NULL);
    }
  else
    {
    latLongPointLR[0] = latLongPointUL[0]+ -1.0 * strtod(oSizeX->value(), NULL) * strtod(oSpacingX->value(), NULL);
    }
  if (latLongPointUL[1] > 0)
    {
    latLongPointLR[1] = latLongPointUL[1] + strtod(oSizeY->value(), NULL) * strtod(oSpacingY->value(), NULL);
    }
  else
    {
    latLongPointLR[1] = latLongPointUL[1] + -1.0 * strtod(oSizeY->value(), NULL) * strtod(oSpacingY->value(), NULL);
    }

  int utmZoneUL = Utils::GetZoneFromGeoPoint(latLongPointUL[0], latLongPointUL[1]);
  int utmZoneLR = Utils::GetZoneFromGeoPoint(latLongPointLR[0], latLongPointLR[1]);

  if (utmZoneUL != utmZoneLR)
    {
    return;
    }

  // Project the new geo Point
  OutputPointType cartoPointUL, cartoPointLR;
  TransformType::Pointer rsTransform = TransformType::New();

  typedef UtmInverseProjection UtmProjectionType;
  UtmProjectionType::Pointer utmProjection = UtmProjectionType::New();

  std::string hem = "N";
  if(latLongPointUL[0] < 0)
    {
    hem = "S";
    }

  utmProjection->SetHemisphere(hem[0]);
  utmProjection->SetZone(utmZoneUL);
  std::string outputProjectionRef = utmProjection->GetWkt();

  char * wgs84Ref;
  OGRSpatialReference oSRS;
  oSRS.SetWellKnownGeogCS("WGS84");
  oSRS.exportToWkt(&wgs84Ref);
  std::string inputProjectionRef = wgs84Ref;

  // Build a Generic RS transform WGS84 -> UTM
  rsTransform->SetInputProjectionRef(inputProjectionRef);

  rsTransform->SetOutputProjectionRef(outputProjectionRef);
  rsTransform->InstanciateTransform();

  cartoPointUL = rsTransform->GetTransform()->GetSecondTransform()->TransformPoint(latLongPointUL);
  cartoPointLR = rsTransform->GetTransform()->GetSecondTransform()->TransformPoint(latLongPointLR);

  //std::cout << latLongPointUL <<" -> " << cartoPointUL <<std::endl;
  //std::cout << latLongPointLR <<" -> " << cartoPointLR <<std::endl;

  double sizeX = strtod(oSizeX->value(), NULL);
  double sizeY = strtod(oSizeY->value(), NULL);

  double spacingXm = (cartoPointLR[0] - cartoPointUL[0]) / sizeX;
  double spacingYm = (cartoPointLR[1] - cartoPointUL[1]) / sizeY;

  itk::OStringStream oss;
  oss.str("");
  oss << spacingXm;
  oSpacingXm->value(oss.str().c_str());
  oss.str("");
  oss << spacingYm;
  oSpacingYm->value(oss.str().c_str());

}

} // End namespace otb
