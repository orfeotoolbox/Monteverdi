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

#include "elevation/ossimElevManager.h"
#include "base/ossimFilename.h"
#include "base/ossimDirectory.h"

#include "otbMsgReporter.h"

#include "itkContinuousIndex.h"
#include <ogr_spatialref.h>

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
  m_projectionRefBuffer = new Fl_Text_Buffer();
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
  gDEM->show();
  iSRTMDEMPath->activate();
  try
    {
    if (m_InputImageUnknown)
      {
      this->UpdateOutputParameters();
      }
    else
      {
      this->UpdateProjectionRef();
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
  m_Controller->GetModel()->GetDEMToImageGenerator()->GetTransform()->InstanciateTransform();

  m_Controller->ProjectRegion(sizeX, sizeY, spacingX, spacingY, originX, originY);

  // Project the image in the selected map projection
  m_Controller->ReprojectImage();

  // Evaluate Hill shading in necessary
  if(oHillShadingAsOutput->value() == 1)
    {
    double       azimutAngle = oAzimutLight->value();
    double       elevationAngle = oElevationLight->value();

    m_Controller->ProcessHillShading(azimutAngle,elevationAngle);

    // Display the color relief
    if(oColorRelief->value() == 1)
      {
      m_Controller->ProcessColorRelief();
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

  IndexType index;
  PointType point, geoPoint;

  /** Origin value */
  index.Fill(0);

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
  oss << m_Controller->GetModel()->GetOutputSize()[0];
  oSizeX->value(oss.str().c_str());

  oss.str("");
  oss << m_Controller->GetModel()->GetOutputSize()[1];
  oSizeY->value(oss.str().c_str());


  this->UpdateProjectionRef();
}


void
DEMToImageGeneratorView::UpdateProjectionRef()
{
  std::string projectionRef;
  projectionRef = m_Controller->GetModel()->GetDEMToImageGenerator()->GetInputProjectionRef();
  std::cout << "DEMToImageGeneratorView::UpdateProjectionRef() : " << projectionRef << std::endl;
  m_projectionRefBuffer->text(projectionRef.c_str());

  oOutputProjectionRef->buffer(m_projectionRefBuffer);
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

void
DEMToImageGeneratorView::UpdateHillShading()
{
   if(oHillShadingAsOutput->value() == 1)
     {
       oAzimutLight->activate();
       oElevationLight->activate();
       oColorRelief->activate();
     }
   else
     {
     oAzimutLight->deactivate();
     oElevationLight->deactivate();
     oColorRelief->deactivate();
     }

}




} // End namespace otb
