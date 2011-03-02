/*=========================================================================

  Program:   ORFEO Toolbox
  Language:  C++
  Date:      $Date$
  Version:   $Revision$


  Copyright (c) Centre National d'Etudes Spatiales. All rights reserved.
  See OTBCopyright.txt for details.

  Copyright (c) CS Systemes d'information. All rights reserved.
  See CSCopyright.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "otbSpectrumModuleView.h"

namespace otb {

SpectrumModuleView
::SpectrumModuleView()
{
  /** Views */
  m_ImageView = ImageViewType::New();
  m_PixelSpectrumView = PixelSpectrumViewType::New();
  m_Ylength = 0;
  m_MonoChannelException = false;

}

SpectrumModuleView
::~SpectrumModuleView()
{
  // Remove registered visualization components from the interface
  gImageViewer->remove(m_ImageView->GetFullWidget());
  gScroll->remove(m_ImageView->GetScrollWidget());
  gZoom->remove(m_ImageView->GetZoomWidget());
  this->Exit();
}

void
SpectrumModuleView
::SetSpectrumModuleModel(SpectrumModuleModelType * m)
{

  m_SpectrumModuleModel = m;
  m_SpectrumModuleModel->RegisterListener(this);

  m_ImageView->SetModel(m_SpectrumModuleModel->GetImageModel());
  m_PixelSpectrumView->SetModel(m_SpectrumModuleModel->GetPixelSpectrumModel());
}

void
SpectrumModuleView
::SetSpectrumModuleController(SpectrumModuleControllerType *c)
{
  m_SpectrumModuleController = c;
  m_PixelSpectrumView->SetController(m_SpectrumModuleController->GetPixelSpectrumController());
}

void
SpectrumModuleView
::Build()
{
  if (!m_SpectrumModuleController)
    {
    itkExceptionMacro(<< "Controller is not set, can not build view.");
    }

  if (!m_SpectrumModuleController->GetImageWidgetController())
    {
    itkExceptionMacro(<< "Widgets controller is not set, can not build view.");
    }
  // Build the fltk code
  SpectrumModuleViewGroup::Build();

  wMainWindow->label("test");

  // Register controllers
  m_ImageView->SetController(m_SpectrumModuleController->GetImageWidgetController());

  // Remove registered visualization components from the interface
  gImageViewer->add(m_ImageView->GetFullWidget());
  gScroll->add(m_ImageView->GetScrollWidget());
  gZoom->add(m_ImageView->GetZoomWidget());
  gCurves->add(m_PixelSpectrumView->GetCurveWidget());
  gIndex->add(m_PixelSpectrumView->GetIndexBox());

  gImageViewer->resizable(m_ImageView->GetFullWidget());
  gScroll->resizable(m_ImageView->GetScrollWidget());
  gZoom->resizable(m_ImageView->GetZoomWidget());
  gCurves->resizable(m_PixelSpectrumView->GetCurveWidget());
  gIndex->resizable(m_PixelSpectrumView->GetIndexBox());

  m_ImageView->GetFullWidget()->resize(gImageViewer->x(), gImageViewer->y(), gImageViewer->w(), gImageViewer->h());
  m_ImageView->GetScrollWidget()->resize(gScroll->x(), gScroll->y(), gScroll->w(), gScroll->h());
  m_ImageView->GetZoomWidget()->resize(gZoom->x(), gZoom->y(), gZoom->w(), gZoom->h());
  m_PixelSpectrumView->GetCurveWidget()->resize(gCurves->x(), gCurves->y(), gCurves->w(), gCurves->h());
  m_PixelSpectrumView->GetIndexBox()->resize(gIndex->x(), gIndex->y(), gIndex->w(), gIndex->h());

  if (!m_MonoChannelException)
    {
    // set layer opacity to the value set in fluid
    this->OpacityCallBack();

    // Show and refresh the interface
    this->wInfo->hide();
    this->wNotMultispectral->hide();
    this->wMainWindow->show();
    this->wCurveDisplayWindow->show();

    m_ImageView->GetFullWidget()->show();
    m_ImageView->GetScrollWidget()->show();
    m_ImageView->GetZoomWidget()->show();

    double numberOfCompPerPixel = (double) m_SpectrumModuleModel->GetInputImage()->GetNumberOfComponentsPerPixel() - 1;

    RedSlider->maximum(numberOfCompPerPixel);
    GreenSlider->maximum(numberOfCompPerPixel);
    BlueSlider->maximum(numberOfCompPerPixel);

    IntensitySlider->minimum(m_SpectrumModuleModel->GetOriginalDynamic());
    IntensitySlider->value(IntensitySlider->minimum());

    bZoomRoulette->bounds(0, 10000);
    bZoomRoulette->value(m_SpectrumModuleController->GetPixelSpectrumController()->GetYAxisLenghtControl());

    bStepRoulette->bounds(1, 800);
    bStepRoulette->value(m_SpectrumModuleController->GetPixelSpectrumController()->GetYGridStep());
    bStepRoulette->step(3);

    sCurveOptionPanSub2->add(m_PixelSpectrumView->GetScrollCurveList());
    m_PixelSpectrumView->GetScrollCurveList()->resize(sCurveOptionPanSub2->x(),
                                                      sCurveOptionPanSub2->y(),
                                                      sCurveOptionPanSub2->w(), sCurveOptionPanSub2->h());

    sCurveSpectralAnglePanSub1->add(m_PixelSpectrumView->GetScrollSpectralAngleCurveList());
    m_PixelSpectrumView->GetScrollSpectralAngleCurveList()->resize(
      sCurveSpectralAnglePanSub1->x(), sCurveSpectralAnglePanSub1->y(),
      sCurveSpectralAnglePanSub1->w(), sCurveSpectralAnglePanSub1->h());

    }
  else
    {
    this->wNotMultispectral->show();
    }
}

void
SpectrumModuleView
::Notify(const std::string& event)
{
  Fl::flush();
}

void
SpectrumModuleView
::RefreshVisualization()
{

  m_ImageView->Update();
}

void
SpectrumModuleView
::Show()
{

  this->wMainWindow->show();
  m_ImageView->GetFullWidget()->show();
  m_ImageView->GetScrollWidget()->show();
  m_ImageView->GetZoomWidget()->show();
}

void
SpectrumModuleView
::ShowInfo()
{
  std::ostringstream helpContent;

  helpContent << "Right Click on a pixel: add a new spectrum to the list "<< std::endl;
  helpContent << "Middle Click: reset all the list"<< std::endl;
  helpContent << "H/S: Hide or Show the current curve"<< std::endl;
  helpContent << "+/-: Change the representation scale"<< std::endl;

  this->wHelpText->value(helpContent.str().c_str());
  wInfo->position(wMainWindow->x() + wMainWindow->w(),wMainWindow->y());
  wInfo->show();
}

void
SpectrumModuleView
::Hide()
{
  wMainWindow->hide();
  wCurveDisplayWindow->hide();
  wNotMultispectral->hide();
  wInfo->hide();
}

void
SpectrumModuleView
::SaveAndQuit()
{
  m_SpectrumModuleController->SaveAndQuit();
  MsgReporter::GetInstance()->Hide();
  this->Hide();
}

void
SpectrumModuleView
::Exit()
{
  m_SpectrumModuleController->Quit();
  MsgReporter::GetInstance()->Hide();
  this->Hide();
}

void
SpectrumModuleView
::ZoomPlusCallBack()
{
  m_SpectrumModuleController->GetPixelSpectrumController()->SetYAxisLenghtControl(1 / (1.2), false);
  m_SpectrumModuleController->GetPixelSpectrumController()->UpdateWidgetView();
}

void
SpectrumModuleView
::ZoomLessCallBack()
{
  m_SpectrumModuleController->GetPixelSpectrumController()->SetYAxisLenghtControl(1.2, false);
  m_SpectrumModuleController->GetPixelSpectrumController()->UpdateWidgetView();

}

void
SpectrumModuleView
::GridStep1CallBack()
{
}

void
SpectrumModuleView
::GridStep2CallBack()
{
}

void
SpectrumModuleView
::GridStep3CallBack()
{
}

void
SpectrumModuleView
::GridStep4CallBack()
{
}

void
SpectrumModuleView
::GridStep5CallBack()
{
}

void
SpectrumModuleView
::AutoScaleCallBack()
{
}

void
SpectrumModuleView
::AutoGridCallBack()
{
  int checkValue = (int) bAutoGridStep->value();

  if (checkValue)
    {
    m_SpectrumModuleController->GetPixelSpectrumController()->SetAutomaticGridStep(true);
    bStepRoulette->deactivate();
    }
  else
    {
    m_SpectrumModuleController->GetPixelSpectrumController()->SetAutomaticGridStep(false);
    bStepRoulette->activate();
    }
}

void
SpectrumModuleView
::ResetListCallBack()
{

  m_SpectrumModuleController->GetPixelSpectrumController()->RemoveAllSpectrums();

}

void
SpectrumModuleView
::ZoomRouletteCallBack()
{
  float lengthWidget;

  if (m_Ylength == 0)
    {
    bZoomRoulette->maximum(m_SpectrumModuleModel->GetPixelSpectrumModel()->GetMaxValue());
    m_Ylength = m_PixelSpectrumView->GetCurveWidget()->GetAxisLength()[1];
    }

  if ((m_Ylength - bZoomRoulette->value()) <= bZoomRoulette->minimum())
    {
    lengthWidget = bZoomRoulette->minimum();
    }
  else
    {
    lengthWidget = m_Ylength - bZoomRoulette->value();
    }

  bZoomRoulette->step(lengthWidget / 40);
  m_SpectrumModuleController->GetPixelSpectrumController()->SetYAxisLenghtControl(lengthWidget, true);
}

void
SpectrumModuleView
::StepRouletteCallBack()
{
  m_SpectrumModuleController->GetPixelSpectrumController()->SetAutomaticGridStep(false);
  m_SpectrumModuleController->GetPixelSpectrumController()->SetGridStepY(bStepRoulette->value());
  m_SpectrumModuleController->GetPixelSpectrumController()->UpdateWidgetView();

}

void
SpectrumModuleView
::HideRollOverCurveCallBack()
{

  int checkValue = (int) mHideRollOverCurveButton->value();

  if (checkValue) m_SpectrumModuleController->GetPixelSpectrumController()->SetCurentCurveAlpha(0);
  else m_SpectrumModuleController->GetPixelSpectrumController()->SetCurentCurveAlpha(1);

  m_SpectrumModuleController->GetPixelSpectrumController()->UpdateWidgetView();

}

void
SpectrumModuleView
::DisplaySpectralAngleCallBack()
{
  int selectedCurveID = -1;
  int size = m_PixelSpectrumView->GetSpectralRadioLabels().size();

  for (int i = 0; i < size; i++)
    {
    if ((int) m_PixelSpectrumView->GetSpectralRadioLabels().at(i)->value() == 1)
      {
      selectedCurveID = (int) m_PixelSpectrumView->GetSpectralRadioLabels().at(i)->GetId();
      break;
      }

    }

  m_SpectrumModuleController->GenerateSpectralAngle(selectedCurveID);

  if ((int) cColorSpectralAngle->value() == 0) IntensitySlider->show();

}

void
SpectrumModuleView
::ClearSpectralAngleCallBack()
{
  m_SpectrumModuleController->ClearSpectralAngle();
}

void
SpectrumModuleView
::ViewerSetupOkCallback()
{
  m_SpectrumModuleController->UpdateViewerDisplay();
}

void
SpectrumModuleView
::OpacityCallBack()
{
  m_SpectrumModuleController->SetSpectralLayerOpacity(1 - OpacitySlider->value());
}

void
SpectrumModuleView
::ExportSpectralImageCallBack()
{

}

void
SpectrumModuleView
::ExportSpecralSpreadsheet()
{
  m_PixelSpectrumView->Update();

  unsigned int id = m_PixelSpectrumView->GetIdOfSelectedSpectralDisplay();

  if (id != PixelSpectrumModel::NoneID)
    {

    const char * filename = NULL;

    filename = fl_file_chooser("Save Spectrum as CSV", "*.*",".");

    if (filename == NULL)
      {
      otbMsgDebugMacro(<<"Empty file name!");
      return ;
      }

    m_SpectrumModuleController->WriteSpreadsheetFile(id, filename);
    }
}

void
SpectrumModuleView
::IntensityCallBack()
{
  m_SpectrumModuleController->ForceSpectralContrast(IntensitySlider->value());
}

void
SpectrumModuleView
::NotMultispectralCallBack()
{
  wNotMultispectral->hide();
}

void
SpectrumModuleView
::TriggerMonoChannelException()
{
  m_MonoChannelException = true;
}

void
SpectrumModuleView
::ColorChannelCallBack()
{
  m_SpectrumModuleController->UpdateViewerDisplay();
}

void
SpectrumModuleView
::ColorOrBWSpectralAngleCallBack()
{
  m_SpectrumModuleController->UseColorMap((int) cColorSpectralAngle->value());
}

}
