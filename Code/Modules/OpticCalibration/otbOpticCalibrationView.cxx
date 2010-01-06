/*=========================================================================

  Program:   ORFEO Toolbox
  Language:  C++
  Date:      $Date$
  Version:   $Revision$


  Copyright (c) Centre National d'Etudes Spatiales. All rights reserved.
See OTBCopyright.txt for details.


    This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE,  See the above copyright notices for more information.

=========================================================================*/

#include "otbOpticCalibrationView.h"
#include <FL/Fl_File_Chooser.H>
#include <FL/fl_ask.H>
#include <FL/Fl_Text_Buffer.H>
#include "base/ossimFilename.h"
#include "base/ossimDirectory.h"
#include "otbMacro.h"
#include "itkExceptionObject.h"
#include "otbMsgReporter.h"
#include "otbAtmosphericRadiativeTerms.h"
#include "otbAtmosphericCorrectionParameters.h"

namespace otb
{

OpticCalibrationView
::OpticCalibrationView()
{
  // Instanstiate the views ( "R" is for "Result" view )
  m_VisuView = VisuViewType::New();
  m_RVisuView = VisuViewType::New();
 
  // Build the curve widget list
  m_CurvesWidgetList    = CurvesWidgetListType::New();
}


OpticCalibrationView
::~OpticCalibrationView()
{
}


void
OpticCalibrationView
::BuildInterface()
{
  if(!m_OpticCalibrationController)
    {
      itkExceptionMacro(<<"Controller is not set, can not build view.");
    }
 
  this->Build();

  // Init aerosol model
  guiAerosolModel->value(0);
  // Hide Pixel description for result ilmage : first shown image is the input
  m_RPixelDescriptionGroup->hide();
  // Init Badn selection
  guiBandSelection->value(0);
  
  m_OpticCalibrationController->LinkPixelDescriptors();
  
  this->Show();
}

void
OpticCalibrationView
::Show()
{
  m_Window->show();
}

void
OpticCalibrationView
::InitWidgets()
{
  assert(m_OpticCalibrationController.IsNotNull() && "The controller is not created");

  // Scroll
  m_ScrollGroup->add(m_VisuView->GetScrollWidget());
  m_ScrollGroup->resizable(m_VisuView->GetScrollWidget());
  m_ScrollGroup->box(FL_NO_BOX);
  m_VisuView->GetScrollWidget()->show();
  m_VisuView->GetScrollWidget()->resize(m_ScrollGroup->x(),m_ScrollGroup->y(),m_ScrollGroup->w(),m_ScrollGroup->h() );

  // Full
  m_FullGroup->add(m_VisuView->GetFullWidget());
  m_FullGroup->resizable(m_VisuView->GetFullWidget());
  m_FullGroup->box(FL_NO_BOX);
  m_VisuView->GetFullWidget()->show();
  m_VisuView->GetFullWidget()->resize(m_FullGroup->x(),m_FullGroup->y(),m_FullGroup->w(),m_FullGroup->h() );

  // Result Full
  m_RFullGroup->add(m_RVisuView->GetFullWidget());
  m_RFullGroup->resizable(m_RVisuView->GetFullWidget());
  m_RFullGroup->box(FL_NO_BOX);
  m_RVisuView->GetFullWidget()->resize(m_RFullGroup->x(),m_RFullGroup->y(),m_RFullGroup->w(),m_RFullGroup->h() );

  // Zoom
  m_ZoomGroup->add(m_VisuView->GetZoomWidget());
  m_ZoomGroup->resizable(m_VisuView->GetZoomWidget());
  m_ZoomGroup->box(FL_NO_BOX);
  m_VisuView->GetZoomWidget()->show();
  m_VisuView->GetZoomWidget()->resize(m_ZoomGroup->x(),m_ZoomGroup->y(),m_ZoomGroup->w(),m_ZoomGroup->h() );

  // Result Zoom
  m_RZoomGroup->add(m_RVisuView->GetZoomWidget());
  m_RZoomGroup->resizable(m_RVisuView->GetZoomWidget());
  m_RZoomGroup->box(FL_NO_BOX);
  m_RVisuView->GetZoomWidget()->show();
  m_RVisuView->GetZoomWidget()->resize(m_RZoomGroup->x(),m_RZoomGroup->y(),m_RZoomGroup->w(),m_RZoomGroup->h() );

  if( m_CurvesWidgetList->Size() > static_cast<unsigned int>(INPUT_IMAGE) )
  {
    // Curves
    m_HistogramsGroup->add(m_CurvesWidgetList->GetNthElement(INPUT_IMAGE));
    m_HistogramsGroup->resizable(m_CurvesWidgetList->GetNthElement(INPUT_IMAGE));
    m_CurvesWidgetList->GetNthElement(INPUT_IMAGE)->show();
    m_CurvesWidgetList->GetNthElement(INPUT_IMAGE)->resize(m_HistogramsGroup->x(),m_HistogramsGroup->y(),m_HistogramsGroup->w(),m_HistogramsGroup->h());
  }

  bInput->value(1);
}



void
OpticCalibrationView
::UpdateHistograms(EnumImageType pType)
{
  if( m_CurvesWidgetList->Size() > static_cast<unsigned int>(pType) )
    {
      // Suppress the existing curves and redraw the widget with the new curves
      m_HistogramsGroup->remove(0);
      // Result Curve
      m_HistogramsGroup->add(m_CurvesWidgetList->GetNthElement(pType));
      m_HistogramsGroup->resizable(m_CurvesWidgetList->GetNthElement(pType));
      m_CurvesWidgetList->GetNthElement(pType)->show();
      m_CurvesWidgetList->GetNthElement(pType)->resize(m_HistogramsGroup->x(),m_HistogramsGroup->y(),m_HistogramsGroup->w(),m_HistogramsGroup->h());
      m_HistogramsGroup->redraw();
    }
}


void
OpticCalibrationView
::Notify()
{
  if(m_OpticCalibrationModel->GetHasNewInput())
    {
      // Build the curve widget list
      m_CurvesWidgetList    = CurvesWidgetListType::New();

      this->RenderCurveWidget(INPUT_IMAGE);
      this->RenderCurveWidget(LUMINANCE);
      this->RenderCurveWidget(REFLECTANCE_TOA);
      this->RenderCurveWidget(REFLECTANCE_TOC);
      this->RenderCurveWidget(DIFF_REFL);
      this->InitWidgets();
      this->SwitchCallback(INPUT_IMAGE);
      this->UpdateInformation();
      this->UpdateViewerSetup();
      this->UpdateCoefSetup();
      this->UpdateRadiativeTermsCallback();

      bLuminance->activate();
      bReflectanceTOA->activate();
      bReflectanceTOC->activate();
      bDiffRefl->activate();
      bViewerSetup->activate();
      bCoefSetup->activate();

      this->UpdateParamDisplay();

      Fl_Text_Buffer *buff = new Fl_Text_Buffer();
      buff->text("");
      tdAtmoParam->buffer(buff);
      tdAtmoParam->redraw();
    }
}

void
OpticCalibrationView
::Init()
{
  // Build the curve widget list
  m_CurvesWidgetList    = CurvesWidgetListType::New();
  
  this->RenderCurveWidget(INPUT_IMAGE);
  this->RenderCurveWidget(LUMINANCE);
  this->RenderCurveWidget(REFLECTANCE_TOA);
  this->RenderCurveWidget(REFLECTANCE_TOC);
  this->RenderCurveWidget(DIFF_REFL);
  this->InitWidgets();
  this->SwitchCallback(INPUT_IMAGE);
  this->UpdateInformation();
  this->UpdateViewerSetup();
  this->UpdateCoefSetup();
  this->UpdateRadiativeTermsCallback();
  
  bLuminance->activate();
  bReflectanceTOA->activate();
  bReflectanceTOC->activate();
  bDiffRefl->activate();
  bViewerSetup->activate();
  bCoefSetup->activate();
  
  this->UpdateParamDisplay();
  
  Fl_Text_Buffer *buff = new Fl_Text_Buffer();
  buff->text("");
  tdAtmoParam->buffer(buff);
  tdAtmoParam->redraw();
}


/**
* Update atmospheric parameters display in the m_CoefSetupWindow
*/
void
OpticCalibrationView
::UpdateParamDisplay()
{
   itk::OStringStream oss;
   oss.str("");
   AtmosphericRadiativeTerms::Pointer atmoTerms = m_OpticCalibrationModel->GetAtmosphericRadiativeTerms();
   oss<<atmoTerms;
   Fl_Text_Buffer *buff = new Fl_Text_Buffer();
   buff->text( oss.str().c_str() );
   // Erase the first line (class name + pointer adress)
   int start = buff->line_start(0);
   int stop = buff->line_end(0);
   buff->remove(start, stop+1);
   tdParam->buffer(buff);
   tdParam->redraw();

   itk::OStringStream oss2;  
   oss2.str("");
   AtmosphericCorrectionParameters::Pointer atmoPar = m_OpticCalibrationModel->GetAtmosphericCorrectionParameters();
   oss2<<atmoPar;
   Fl_Text_Buffer *buff2 = new Fl_Text_Buffer();
   buff2->text( oss2.str().c_str() );
   // Erase the first line (class name + pointer adress)
   start = buff2->line_start(0);
   stop = buff2->line_end(0);
   buff2->remove(start, stop+1);
   tdAtmoParam->buffer(buff2);
   tdAtmoParam->redraw();

   std::string aeroFile = teAeronetFile->value();
   if(aeroFile != "")
     {
       AtmosphericCorrectionParameters::Pointer param = m_OpticCalibrationModel->GetReflectanceToSurfaceReflectanceFilter()->GetCorrectionParameters();
       guiWater->value(param->GetWaterVaporAmount());
       guiAeroTh->value(param->GetAerosolOptical());
       guiWater->redraw();
       guiAeroTh->redraw(); 
     }
}

/**
 * Update the title of the window with the name, the number of channels and the size
 * of the input image.
 */
void
OpticCalibrationView
::UpdateInformation()
{
  itk::OStringStream oss;
  oss.str("");
  oss<<"Optic Calibration Application";
  oss<<" ("<<m_OpticCalibrationModel->GetInputImage()->GetNumberOfComponentsPerPixel();
  if(m_OpticCalibrationModel->GetInputImage()->GetNumberOfComponentsPerPixel() != 1)
    oss<<" bands , ";
  else
    oss<<" band , ";

  oss<<m_OpticCalibrationModel->GetInputImage()->GetLargestPossibleRegion().GetSize()<<")";
  m_Window->label(oss.str().c_str());
}

/**
 * Compute RGB histograms
 */
void
OpticCalibrationView
::RenderCurveWidget(EnumImageType pType)
{
  if( m_OpticCalibrationModel->GetImageLayerList()->Size() < static_cast<unsigned int>(pType) )
    return;

  ImageLayerPointerType lImageLayer;
  lImageLayer = m_OpticCalibrationModel->GetImageLayerList()->GetNthElement(pType);

  RenderingFunctionPointerType lRenderingFunction;
  lRenderingFunction = lImageLayer->GetRenderingFunction();

  ChannelListType channels = lRenderingFunction->GetChannelList();

  // Colors
  HistogramCurveType::ColorType red,green,blue,grey;
  red.Fill(0);
  red[0]=1.;
  red[3]=0.5;

  green.Fill(0);
  green[1]=1.;
  green[3]=0.5;

  blue.Fill(0);
  blue[2]=1.;
  blue[3]=0.5;

  grey.Fill(0.5);

  CurvesWidgetPointerType curveWidget = CurvesWidgetType::New();
  curveWidget->ClearAllCurves();

  if (lRenderingFunction->GetPixelRepresentationSize() >=3)
  {
   HistogramCurveType::Pointer bhistogram = HistogramCurveType::New();
   bhistogram->SetHistogramColor(blue);
   bhistogram->SetLabelColor(blue);
   bhistogram->SetHistogram(lImageLayer->GetHistogramList()->GetNthElement(channels[2]));
   curveWidget->AddCurve(bhistogram);
  }

  if (lRenderingFunction->GetPixelRepresentationSize() >=2)
  {
   HistogramCurveType::Pointer ghistogram = HistogramCurveType::New();
   ghistogram->SetHistogramColor(green);
   ghistogram->SetLabelColor(green);
   ghistogram->SetHistogram(lImageLayer->GetHistogramList()->GetNthElement(channels[1]));
   curveWidget->AddCurve(ghistogram);
  }

  HistogramCurveType::Pointer rhistogram = HistogramCurveType::New();
  if (lRenderingFunction->GetPixelRepresentationSize() == 1)
  {
   rhistogram->SetHistogramColor(grey);
   rhistogram->SetLabelColor(grey);
  }
  else
  {
   rhistogram->SetHistogramColor(red);
   rhistogram->SetLabelColor(red);
  }
  rhistogram->SetHistogram(lImageLayer->GetHistogramList()->GetNthElement(channels[0]));
  curveWidget->AddCurve(rhistogram);

  curveWidget->SetXAxisLabel("Pixels");
  curveWidget->SetYAxisLabel("Frequency");
  curveWidget->redraw();
  // Save the curve widget
  m_CurvesWidgetList->PushBack(curveWidget);


}


void
OpticCalibrationView
::SaveAndQuitCallback()
{
  m_OpticCalibrationController->QuitModel();
  this->QuitCallback();
}

void
OpticCalibrationView
::QuitCallback()
{
  m_Window->hide();
  m_ViewerSetupWindow->hide();
  m_CoefSetupWindow->hide();
}


void
OpticCalibrationView
::HideAll()
{
  this->QuitCallback();
}


void
OpticCalibrationView
::ViewerSetupCallback()
{
  m_ViewerSetupWindow->show();
}

void
OpticCalibrationView
::ViewerSetupOkCallback()
{
  if (guiViewerSetupColorMode->value())
    {
      m_OpticCalibrationController->UpdateRGBChannelOrder(atoi(guiRedChannelChoice->value())-1,
								atoi(guiGreenChannelChoice->value())-1,
								atoi(guiBlueChannelChoice->value())-1);
      
    }
  else if (guiViewerSetupGrayscaleMode->value())
    {
      m_OpticCalibrationController->UpdateGrayscaleChannel(atoi(guiGrayscaleChannelChoice->value())-1);
    }
}

void
OpticCalibrationView
::ViewerSetupCancelCallback()
{
  m_ViewerSetupWindow->hide();
}

/**
  * GrayscaleSet();
  */
void
OpticCalibrationView
::GrayscaleSetCallback()
{
   guiViewerSetupGrayscaleMode->set();
   guiViewerSetupColorMode->clear();

   guiRedChannelChoice->deactivate();
   guiGreenChannelChoice->deactivate();
   guiBlueChannelChoice->deactivate();
   guiGrayscaleChannelChoice->activate();

   unsigned int lNbComponent = m_OpticCalibrationModel->GetInputImage()->GetNumberOfComponentsPerPixel();
   RenderingFunctionPointerType lRenderingFunction = m_OpticCalibrationModel->GetImageLayerList()->GetNthElement(INPUT_IMAGE)->GetRenderingFunction();
   guiGrayscaleChannelChoice->value(std::min(lRenderingFunction->GetChannelList()[0],lNbComponent-1));

}

 /**
  * RGBSet();
  */
void
OpticCalibrationView
::RGBSetCallback()
{
   guiViewerSetupColorMode->set();
   guiViewerSetupGrayscaleMode->clear();
   guiGrayscaleChannelChoice->deactivate();
 
   guiRedChannelChoice->activate();
   guiGreenChannelChoice->activate();
   guiBlueChannelChoice->activate();


   unsigned int nbComponent = m_OpticCalibrationModel->GetInputImage()->GetNumberOfComponentsPerPixel();

   RenderingFunctionPointerType renderingFunction = m_OpticCalibrationModel->GetImageLayerList()->GetNthElement(INPUT_IMAGE)->GetRenderingFunction();
   ChannelListType channels = renderingFunction->GetChannelList();
   unsigned int i=0;
   while (channels.size() < 3)
   {
     channels.push_back(i);
     ++i;
   }

   guiRedChannelChoice->value(std::min(channels[0],nbComponent-1));
   guiGreenChannelChoice->value(std::min(channels[1],nbComponent-1));
   guiBlueChannelChoice->value(std::min(channels[2],nbComponent-1));

}



/**
 * Update Viewer Setup
 */
void
OpticCalibrationView
::UpdateViewerSetup()
{
  unsigned int lNbComponent = m_OpticCalibrationModel->GetInputImage()->GetNumberOfComponentsPerPixel();
  itk::OStringStream oss;
  oss.str("");

  //Clear all the choices
  guiGrayscaleChannelChoice->clear();
  guiRedChannelChoice->clear();
  guiGreenChannelChoice->clear();
  guiBlueChannelChoice->clear();
  guiBandSelection->clear();

  for (unsigned int i = 0;i<lNbComponent;++i)
  {
    oss.str("");
    oss<<i+1;
    guiGrayscaleChannelChoice->add(oss.str().c_str());
    guiRedChannelChoice->add(oss.str().c_str());
    guiGreenChannelChoice->add(oss.str().c_str());
    guiBlueChannelChoice->add(oss.str().c_str());
    guiBandSelection->add(oss.str().c_str());
  }
  m_ViewerSetupWindow->redraw();


  switch(lNbComponent){
  case 1 :
    this->GrayscaleSetCallback();
    break;
  case 4 :
    this->RGBSetCallback();
    break;
  case 3 :
    this->RGBSetCallback();
    break;
  default :
    break;
  }
}


/**
 * Update Coef Setup
 */
void
OpticCalibrationView
::UpdateCoefSetup()
{
  unsigned int lNbComponent = m_OpticCalibrationModel->GetInputImage()->GetNumberOfComponentsPerPixel();
  itk::OStringStream oss;
  oss.str("");

  guiBandSelection->clear();

  for (unsigned int i = 0;i<lNbComponent;++i)
  {
    oss.str("");
    oss<<i;
    guiBandSelection->add(oss.str().c_str());
  }

  tdAtmoParam->activate();

  AtmosphericCorrectionParameters::Pointer param = m_OpticCalibrationModel->GetAtmosphericCorrectionParameters();
  guiOzoneAmount->value(param->GetOzoneAmount());
  guiAtmoPressure->value(param->GetAtmosphericPressure());	
  guiAerosolModel->value(param->GetAerosolModel());
  guiWater->value(param->GetWaterVaporAmount());
  guiAeroTh->value(param->GetAerosolOptical());
  guiAerosolModel->redraw();
  guiOzoneAmount->redraw();
  guiAtmoPressure->redraw();
  guiWater->redraw();
  guiAeroTh->redraw();
}
 

/**
  * Switch Views
  */
void
OpticCalibrationView
::SwitchCallback(EnumImageType pType)
{

  bInput->value(0);
  bLuminance->value(0);
  bReflectanceTOA->value(0);
  bReflectanceTOC->value(0);
  bDiffRefl->value(0);

  switch(pType)
  {
    case INPUT_IMAGE:
      bInput->value(1);
      this->ShowInputGroup();
      break;
    case LUMINANCE:
      bLuminance->value(1);
      this->ShowResultGroup();
      break;
    case REFLECTANCE_TOA:
      bReflectanceTOA->value(1);
      this->ShowResultGroup();
      break;
    case REFLECTANCE_TOC:
      bReflectanceTOC->value(1);
      this->ShowResultGroup();
      break;
    case DIFF_REFL:
      bDiffRefl->value(1);
      this->ShowResultGroup();
      break;
    default:
      return;
  }

  m_OpticCalibrationController->UpdateDisplay(pType);
  this->UpdateHistograms(pType);
}

void
OpticCalibrationView
::ShowInputGroup()
{
  m_FullGroup->show();
  m_ZoomGroup->show();
  m_VisuView->GetFullWidget()->show();
  m_VisuView->GetZoomWidget()->show();
  m_PixelDescriptionGroup->show();

  m_RFullGroup->hide();
  m_RZoomGroup->hide();
  m_RVisuView->GetFullWidget()->hide();
  m_RVisuView->GetZoomWidget()->hide();
  m_RPixelDescriptionGroup->hide();
}

void
OpticCalibrationView
::ShowResultGroup()
{
  m_RFullGroup->show();
  m_RZoomGroup->show();
  m_RVisuView->GetFullWidget()->show();
  m_RVisuView->GetZoomWidget()->show();
  m_RPixelDescriptionGroup->show();

  m_FullGroup->hide();
  m_ZoomGroup->hide();
  m_VisuView->GetFullWidget()->hide();
  m_VisuView->GetZoomWidget()->hide();
  m_PixelDescriptionGroup->hide();
}

void
OpticCalibrationView
::OpenAeronetFileCallback()
{
  const char * cfname = fl_file_chooser("Pick an Aeronet file", "*.*",m_LastPath.c_str());
  Fl::check();
  if (cfname == NULL || strlen(cfname)<1)
  {
    return ;
  }
  ossimFilename fname(cfname);
  m_LastPath = fname.path();
  teAeronetFile->value(cfname);
  teAeronetFile->redraw();
}

void
OpticCalibrationView
::OpenFFVFileCallback()
{
  const char * cfname = fl_file_chooser("Pick an Filter Function Values file", "*.*",m_LastPath.c_str());
  Fl::check();
  if (cfname == NULL || strlen(cfname)<1)
  {
    return ;
  }
  ossimFilename fname(cfname);
  m_LastPath = fname.path();
  teFFVFile->value(cfname);
  teFFVFile->redraw();
}


void
OpticCalibrationView
::CoefSetupOkCallback()
{ 
  if( tCorrParam->visible() == 1)
    {
      m_OpticCalibrationController->UpdateCorrectionParameters();
      tdAtmoParam->show();
      this->UpdateDisplayResult();
      // update the displayed channel radiative terms value
      this->UpdateRadiativeTermsCallback();
    }
  else if( tRadTerm->visible() == 1 )
    {
      this->ReloadChannelTermsCallback( true );
      this->UpdateDisplayResult();
    }
  else
    return;
}

void
OpticCalibrationView
::ReloadChannelTermsCallback( bool updateIm )
{
  m_OpticCalibrationController->UpdateRadiativeTerms( updateIm );
  
  Fl_Text_Buffer *buff = new Fl_Text_Buffer();
  buff->text("");
  tdAtmoParam->buffer(buff);
  tdAtmoParam->redraw();
}

void
OpticCalibrationView
::UpdateRadiativeTermsCallback()
{
  unsigned int ch = static_cast<unsigned int>(atoi(guiBandSelection->value()));

  AtmosphericRadiativeTerms::Pointer terms = m_OpticCalibrationModel->GetAtmosphericRadiativeTerms();

  gIntrinsicRef->value( terms->GetIntrinsicAtmosphericReflectance(ch) );
  guiAlbedo->value( terms->GetSphericalAlbedo(ch) );
  guiGasT->value( terms->GetTotalGaseousTransmission(ch) );
  guiDT->value( terms->GetDownwardTransmittance(ch) );
  guiUT->value( terms->GetUpwardTransmittance(ch) );
  guiUDiffT->value( terms->GetUpwardDiffuseTransmittance(ch) );
  guiUDirT->value( terms->GetUpwardDirectTransmittance(ch) );
  guiUDTR->value( terms->GetUpwardDiffuseTransmittanceForRayleigh(ch) );
  guiUDTA->value( terms->GetUpwardDiffuseTransmittanceForAerosol(ch) );
  
  gIntrinsicRef->redraw();
  guiAlbedo->redraw();
  guiGasT->redraw();
  guiDT->redraw();
  guiUT->redraw();
  guiUDiffT->redraw();
  guiUDirT->redraw();
  guiUDTR->redraw();
  guiUDTA->redraw();

  gRadTerms->redraw();
}


void
OpticCalibrationView
::UpdateDisplayResult()
{
  // Update display
  if( bInput->value() == 1 )
    this->SwitchCallback(INPUT_IMAGE);
  else if( bLuminance->value()==1 )
    this->SwitchCallback(LUMINANCE);
  else if( bReflectanceTOA->value() == 1 )
    this->SwitchCallback(REFLECTANCE_TOA);
  else if( bReflectanceTOC->value() == 1 )
    this->SwitchCallback(REFLECTANCE_TOC);
  else if( bDiffRefl->value() == 1)
    this->SwitchCallback(DIFF_REFL);
}
} // end namesapce otb

