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
#include "otbPolarimetricSynthesisView.h"
#include <FL/Fl_File_Chooser.H>
#include "otbFltkFilterWatcher.h"
#include "otbMacro.h"
#include "otbMsgReporter.h"

namespace otb
{


PolarimetricSynthesisView::PolarimetricSynthesisView()
{
  m_VectorCastFilter = VectorCastFilterType::New();
  m_QuicklookVectorCastFilter = VectorCastFilterType::New();

  m_Model = PolarimetricSynthesisModel::GetInstance();
  m_Model->RegisterListener(this);

//  m_Controller = PolarimetricSynthesisControllerInterface::New();

}

void PolarimetricSynthesisView::Build()
{

  if(!m_Controller)
    {
    itkExceptionMacro(<<"Controller is not set, can not build view.");
    }

  if(!m_WidgetsController)
    {
    itkExceptionMacro(<<"Widgets controller is not set, can not build view.");
    }

  // Build the fltk code
  PolarimetricSynthesisGUI::Build();

  this->wMainWindow->show();

  m_InterfaceColor[0]=1;
  m_InterfaceColor[1]=0;
  m_InterfaceColor[2]=0;
  m_InterfaceColor[3]=1;

  m_PoincareSphereWidget = PoincareSphereWidgetType::New();
  m_PoincareSphereWidget->SetPsiI(m_Model->GetRedPsiI());
  m_PoincareSphereWidget->SetPsiR(m_Model->GetRedPsiR());
  m_PoincareSphereWidget->SetKhiI(m_Model->GetRedKhiI());
  m_PoincareSphereWidget->SetKhiR(m_Model->GetRedKhiR());
  bRGB->value(m_Model->GetRGB());
  if (m_Model->GetRGB())
  {
    m_PoincareSphereWidget->SetPsiI(m_Model->GetRedPsiI());
    m_PoincareSphereWidget->SetPsiR(m_Model->GetRedPsiR());
    m_PoincareSphereWidget->SetKhiI(m_Model->GetRedKhiI());
    m_PoincareSphereWidget->SetKhiR(m_Model->GetRedKhiR());
    gGrayTabs->hide();
    gRGBTabs->show();
  }
  else
  {
    m_PoincareSphereWidget->SetPsiI(m_Model->GetGrayPsiI());
    m_PoincareSphereWidget->SetPsiR(m_Model->GetGrayPsiR());
    m_PoincareSphereWidget->SetKhiI(m_Model->GetGrayKhiI());
    m_PoincareSphereWidget->SetKhiR(m_Model->GetGrayKhiR());
    gRGBTabs->hide();
    gGrayTabs->show();
  }

  m_PoincareSphereWidget->Init(gSphere->x(), gSphere->y(), gSphere->w(), gSphere->h(),"Poincare sphere");
  gSphere->add(m_PoincareSphereWidget);
  gSphere->box(FL_NO_BOX);
  m_PoincareSphereWidget->show();
  m_PoincareSphereWidget->redraw();

  //this->RefreshInterface();
}


void PolarimetricSynthesisView::SetController(PolarimetricSynthesisControllerInterface * controller)
{
  m_Controller = controller;
}


void PolarimetricSynthesisView::SetWidgetsController(ImageWidgetController * controller)
{
  m_WidgetsController = controller;
}


void PolarimetricSynthesisView::RefreshViewer()
{
  BoxType::Pointer box = BoxType::New();
  SizeType boxSize;
  IndexType boxIndex;

  ImageType::Pointer image, quicklook;

  if (m_Model->GetRGB())
  {
    image = m_Model->GetOutputVectorImage();
    image->UpdateOutputInformation();
    quicklook = m_Model->GetQuicklookOutputVectorImage();
    quicklook->Update();
  }
  else
  {
    m_VectorCastFilter->SetInput(m_Model->GetOutputImage());
    m_VectorCastFilter->GetOutput()->UpdateOutputInformation();
    image = m_VectorCastFilter->GetOutput();
    m_QuicklookVectorCastFilter->SetInput(m_Model->GetQuicklookOutputImage());
    m_QuicklookVectorCastFilter->Update();
    quicklook = m_QuicklookVectorCastFilter->GetOutput();
  }


  // Defining the full widget
  if (m_FullWidget.IsNull())
  {
    m_FullWidget  = FullWidgetType::New();
    m_FullWidget->SetInput(image);
    m_FullWidget->SetController(m_Controller);
    m_FullWidget->Init(gFullResolution->x(), gFullResolution->y(), gFullResolution->w(), gFullResolution->h(),"Full resolution");
    gFullResolution->box(FL_NO_BOX);
    gFullResolution->add(m_FullWidget);

    if (!m_Model->GetRGB() )
    {
      m_FullWidget->GetTransferFunctionList()->PushBack(m_FullWidget->GetTransferFunctionList()->GetNthElement(0));
      m_FullWidget->GetTransferFunctionList()->PushBack(m_FullWidget->GetTransferFunctionList()->GetNthElement(0));
    }
    else
    {
      m_FullWidget->GetTransferFunctionList()->SetNthElement(1, m_FullWidget->GetTransferFunctionList()->GetNthElement(0));
      m_FullWidget->GetTransferFunctionList()->SetNthElement(2, m_FullWidget->GetTransferFunctionList()->GetNthElement(0));
    }
    m_FullWidget->Show();
  }
  else
  {
    m_FullWidget->SetInput(image);
    m_FullWidget->ClearBufferedRegion();
  }

  // Defining the box of the viewed full region
  boxSize[0]=(m_FullWidget->GetViewedRegion().GetSize()[0]+2);
  boxSize[1]=(m_FullWidget->GetViewedRegion().GetSize()[1]+2);
  boxIndex[0]=(m_FullWidget->GetViewedRegion().GetIndex()[0]-1);
  boxIndex[1]=(m_FullWidget->GetViewedRegion().GetIndex()[1]-1);
  box->SetSize(boxSize);
  box->SetIndex(boxIndex);
  box->SetColor(m_InterfaceColor);


  // defining the scroll widget
  if (m_ScrollWidget.IsNull())
  {
    m_ScrollWidget  = ScrollWidgetType::New();
    m_ScrollWidget->SetController(m_Controller);
    m_ScrollWidget->SetInput(quicklook);
    m_ScrollWidget->Init(gScroll->x(), gScroll->y(), gScroll->w(), gScroll->h(),"Scroll");
    m_ScrollWidget->SetFormOverlayVisible(true);
    gScroll->add(m_ScrollWidget);
    gScroll->box(FL_NO_BOX);
    m_ScrollWidget->GetFormList()->PushBack(box);
    m_ScrollWidget->GetTransferFunctionList()->SetNthElement(0, m_FullWidget->GetTransferFunctionList()->GetNthElement(0));
    if (!m_Model->GetRGB())
    {
      m_ScrollWidget->GetTransferFunctionList()->PushBack(m_FullWidget->GetTransferFunctionList()->GetNthElement(0));
      m_ScrollWidget->GetTransferFunctionList()->PushBack(m_FullWidget->GetTransferFunctionList()->GetNthElement(0));
    }
    else
    {
      m_ScrollWidget->GetTransferFunctionList()->SetNthElement(1, m_FullWidget->GetTransferFunctionList()->GetNthElement(0));
      m_ScrollWidget->GetTransferFunctionList()->SetNthElement(2, m_FullWidget->GetTransferFunctionList()->GetNthElement(0));
    }
    m_ScrollWidget->Show();
  }
  else
  {
    m_ScrollWidget->GetFormList()->SetNthElement(0, box);
    m_ScrollWidget->SetInput(quicklook);
    m_ScrollWidget->ClearBufferedRegion();
  }
  m_ScrollWidget->SetSubSamplingRate(m_Model->GetShrinkFactor());

  m_FullWidget->GetTransferFunctionList()->GetNthElement(0)->SetLowerBound(m_Model->GetMin());
  m_FullWidget->GetTransferFunctionList()->GetNthElement(0)->SetUpperBound(m_Model->GetMax());

  if (m_Model->GetRGB())
  {

    m_FullWidget->SetViewModel(FullWidgetType::RGB);
    m_ScrollWidget->SetViewModel(ScrollWidgetType::RGB);

  }
  else
  {
    m_FullWidget->SetViewModel(FullWidgetType::GRAYSCALE);
    m_ScrollWidget->SetViewModel(ScrollWidgetType::GRAYSCALE);
  }

  m_ScrollWidget->redraw();
  m_FullWidget->redraw();

  //  if(m_HistogramWidget.IsNull())
//       {
//   m_HistogramWidget = HistogramWidgetType::New();
//   m_HistogramWidget->SetTransferFunction(m_FullWidget->GetTransferFunctionList()->GetNthElement(0));
//   m_HistogramWidget->SetHistogram(m_Model->GetOutputHistogram());
//   m_HistogramWidget->resize(gHistogram->x(), gHistogram->y(), gHistogram->w(), gHistogram->h());
//   gHistogram->add(m_HistogramWidget);
//   m_HistogramWidget->show();
//       }
//     m_HistogramWidget->SetHistogram(m_Model->GetOutputHistogram());
//     m_HistogramWidget->redraw();
}

void PolarimetricSynthesisView::Notify(const std::string & event)
{
  if(event == "Update")
    {
    this->UpdateInterface();
    if (m_Model->GetValid())
    {
      this->RefreshViewer();
    }
    Fl::flush();
    }
}



void PolarimetricSynthesisView::SaveAndQuitCallback()
{
  m_Controller->Save();
  MsgReporter::GetInstance()->Hide();
  wMainWindow->hide();
}


void PolarimetricSynthesisView::CancelCallback()
{
  wMainWindow->hide();
}


void PolarimetricSynthesisView::UpdateInterface()
{

  if (gGrayGroup->visible_r())
  {

    vGrayPsiI->value(m_Model->GetGrayPsiI());
    vGrayPsiR->value(m_Model->GetGrayPsiR());
    vGrayKhiI->value(m_Model->GetGrayKhiI());
    vGrayKhiR->value(m_Model->GetGrayKhiR());
    m_PoincareSphereWidget->SetPsiI(m_Model->GetGrayPsiI());
    m_PoincareSphereWidget->SetPsiR(m_Model->GetGrayPsiR());
    m_PoincareSphereWidget->SetKhiI(m_Model->GetGrayKhiI());
    m_PoincareSphereWidget->SetKhiR(m_Model->GetGrayKhiR());
    m_PoincareSphereWidget->redraw();
  }
  else if (gRedGroup->visible_r())
  {

    vRedPsiI->value(m_Model->GetRedPsiI());
    vRedPsiR->value(m_Model->GetRedPsiR());
    vRedKhiI->value(m_Model->GetRedKhiI());
    vRedKhiR->value(m_Model->GetRedKhiR());
    m_PoincareSphereWidget->SetPsiI(m_Model->GetRedPsiI());
    m_PoincareSphereWidget->SetPsiR(m_Model->GetRedPsiR());
    m_PoincareSphereWidget->SetKhiI(m_Model->GetRedKhiI());
    m_PoincareSphereWidget->SetKhiR(m_Model->GetRedKhiR());
    m_PoincareSphereWidget->redraw();
  }
  else if (gGreenGroup->visible_r())
  {
    vGreenPsiI->value(m_Model->GetGreenPsiI());
    vGreenPsiR->value(m_Model->GetGreenPsiR());
    vGreenKhiI->value(m_Model->GetGreenKhiI());
    vGreenKhiR->value(m_Model->GetGreenKhiR());
    m_PoincareSphereWidget->SetPsiI(m_Model->GetGreenPsiI());
    m_PoincareSphereWidget->SetPsiR(m_Model->GetGreenPsiR());
    m_PoincareSphereWidget->SetKhiI(m_Model->GetGreenKhiI());
    m_PoincareSphereWidget->SetKhiR(m_Model->GetGreenKhiR());
    m_PoincareSphereWidget->redraw();
  }
  else if (gBlueGroup->visible_r())
  {
    vBluePsiI->value(m_Model->GetBluePsiI());
    vBluePsiR->value(m_Model->GetBluePsiR());
    vBlueKhiI->value(m_Model->GetBlueKhiI());
    vBlueKhiR->value(m_Model->GetBlueKhiR());
    m_PoincareSphereWidget->SetPsiI(m_Model->GetBluePsiI());
    m_PoincareSphereWidget->SetPsiR(m_Model->GetBluePsiR());
    m_PoincareSphereWidget->SetKhiI(m_Model->GetBlueKhiI());
    m_PoincareSphereWidget->SetKhiR(m_Model->GetBlueKhiR());
    m_PoincareSphereWidget->redraw();
  }

/*PIO to delete ? */
/*
  if (m_Model->GetValid())
  {
    bSaveResult->activate();
  }
*/
}
void PolarimetricSynthesisView::RGBButtonCallback()
{
  m_Controller->ChangeRGB(bRGB->value());
  if (bRGB->value())
  {
    gGrayTabs->hide();
    gRGBTabs->show();
    this->RGBGroupCallback();
  }
  else
  {
    gRGBTabs->hide();
    gGrayTabs->show();
    this->GrayGroupCallback();
  }
}
void PolarimetricSynthesisView::DialGainCallback()
{
  m_Controller->ChangeGain(vGain->value());
  iGain->value(vGain->value());
}
void PolarimetricSynthesisView::InputGainCallback()
{
  m_Controller->ChangeGain(iGain->value());
  vGain->value(iGain->value());
}
void PolarimetricSynthesisView::GrayGroupCallback()
{


  m_Controller->ChangeGrayPsiI(vGrayPsiI->value());
  m_Controller->ChangeGrayKhiI(vGrayKhiI->value());
  m_Controller->ChangeGrayPsiR(vGrayPsiR->value());
  m_Controller->ChangeGrayKhiR(vGrayKhiR->value());

  // Update the sphere
  m_PoincareSphereWidget->SetPsiI(m_Model->GetGrayPsiI());
  m_PoincareSphereWidget->SetPsiR(m_Model->GetGrayPsiR());
  m_PoincareSphereWidget->SetKhiI(m_Model->GetGrayKhiI());
  m_PoincareSphereWidget->SetKhiR(m_Model->GetGrayKhiR());

}
void PolarimetricSynthesisView::RGBGroupCallback()
{

  if (gRGBTabs->value()==gRedGroup)
  {
    m_Controller->ChangeRedPsiI(vRedPsiI->value());
    m_Controller->ChangeRedKhiI(vRedKhiI->value());
    m_Controller->ChangeRedPsiR(vRedPsiR->value());
    m_Controller->ChangeRedKhiR(vRedKhiR->value());
    // Update the sphere
    m_PoincareSphereWidget->SetPsiI(m_Model->GetRedPsiI());
    m_PoincareSphereWidget->SetPsiR(m_Model->GetRedPsiR());
    m_PoincareSphereWidget->SetKhiI(m_Model->GetRedKhiI());
    m_PoincareSphereWidget->SetKhiR(m_Model->GetRedKhiR());

  }
  else if (gRGBTabs->value()==gGreenGroup)
  {

    m_Controller->ChangeGreenPsiI(vGreenPsiI->value());
    m_Controller->ChangeGreenKhiI(vGreenKhiI->value());
    m_Controller->ChangeGreenPsiR(vGreenPsiR->value());
    m_Controller->ChangeGreenKhiR(vGreenKhiR->value());
    // Update the sphere
    m_PoincareSphereWidget->SetPsiI(m_Model->GetGreenPsiI());
    m_PoincareSphereWidget->SetPsiR(m_Model->GetGreenPsiR());
    m_PoincareSphereWidget->SetKhiI(m_Model->GetGreenKhiI());
    m_PoincareSphereWidget->SetKhiR(m_Model->GetGreenKhiR());

  }
  else if (gRGBTabs->value()==gBlueGroup)
  {

    m_Controller->ChangeBluePsiI(vBluePsiI->value());
    m_Controller->ChangeBlueKhiI(vBlueKhiI->value());
    m_Controller->ChangeBluePsiR(vBluePsiR->value());
    m_Controller->ChangeBlueKhiR(vBlueKhiR->value());
    // Update the sphere
    m_PoincareSphereWidget->SetPsiI(m_Model->GetBluePsiI());
    m_PoincareSphereWidget->SetPsiR(m_Model->GetBluePsiR());
    m_PoincareSphereWidget->SetKhiI(m_Model->GetBlueKhiI());
    m_PoincareSphereWidget->SetKhiR(m_Model->GetBlueKhiR());

  }
}

// -------------------------- RED  -------------------------- //
void PolarimetricSynthesisView::RedPsiICallback()
{
  m_Controller->ChangeRedPsiI(vRedPsiI->value());
}
void PolarimetricSynthesisView::RedPsiRCallback()
{
  m_Controller->ChangeRedPsiR(vRedPsiR->value());
}
void PolarimetricSynthesisView::RedKhiICallback()
{
  m_Controller->ChangeRedKhiI(vRedKhiI->value());
}
void PolarimetricSynthesisView::RedKhiRCallback()
{
  m_Controller->ChangeRedKhiR(vRedKhiR->value());
}
void PolarimetricSynthesisView::RedCoPolarizationCallback()
{
  bRedCoPolarization->set();
  bRedCrossPolarization->clear();
  bRedAnyPolarization->clear();
  if (!m_Controller->IsHEmissionMode() | !m_Controller->IsVEmissionMode())
  {
    vRedPsiR->deactivate();
    vRedKhiR->deactivate();
  }
  else
  {
    vRedPsiR->activate();
    vRedKhiR->activate();
  }
  m_Controller->ChangeRedPolarizationMode(CO_POLAR);
}
void PolarimetricSynthesisView::RedCrossPolarizationCallback()
{
  bRedCrossPolarization->set();
  bRedCoPolarization->clear();
  bRedAnyPolarization->clear();
  if (!m_Controller->IsVEmissionMode() | !m_Controller->IsHEmissionMode())
  {
    vRedPsiR->deactivate();
    vRedKhiR->deactivate();
  }
  else
  {
    vRedPsiR->activate();
    vRedKhiR->activate();
  }
  m_Controller->ChangeRedPolarizationMode(CROSS_POLAR);
}
void PolarimetricSynthesisView::RedAnyPolarizationCallback()
{
  bRedAnyPolarization->set();
  bRedCrossPolarization->clear();
  bRedCoPolarization->clear();
  vRedPsiR->activate();
  vRedKhiR->activate();
  m_Controller->ChangeRedPolarizationMode(ANY_POLAR);
}

// -------------------------- GREEN  -------------------------- //
void PolarimetricSynthesisView::GreenPsiICallback()
{
  m_Controller->ChangeGreenPsiI(vGreenPsiI->value());
}
void PolarimetricSynthesisView::GreenPsiRCallback()
{
  m_Controller->ChangeGreenPsiR(vGreenPsiR->value());
}
void PolarimetricSynthesisView::GreenKhiICallback()
{
  m_Controller->ChangeGreenKhiI(vGreenKhiI->value());
}
void PolarimetricSynthesisView::GreenKhiRCallback()
{
  m_Controller->ChangeGreenKhiR(vGreenKhiR->value());
}
void PolarimetricSynthesisView::GreenCoPolarizationCallback()
{
  bGreenCoPolarization->set();
  bGreenCrossPolarization->clear();
  bGreenAnyPolarization->clear();
  if (!m_Controller->IsVEmissionMode() | !m_Controller->IsHEmissionMode())
  {
    vGreenPsiR->deactivate();
    vGreenKhiR->deactivate();
  }
  else
  {
    vGreenPsiR->activate();
    vGreenKhiR->activate();
  }
  m_Controller->ChangeGreenPolarizationMode(CO_POLAR);
}
void PolarimetricSynthesisView::GreenCrossPolarizationCallback()
{
  bGreenCrossPolarization->set();
  bGreenCoPolarization->clear();
  bGreenAnyPolarization->clear();
  if (!m_Controller->IsVEmissionMode() | !m_Controller->IsHEmissionMode())
  {
    vGreenPsiR->deactivate();
    vGreenKhiR->deactivate();
  }
  else
  {
    vGreenPsiR->activate();
    vGreenKhiR->activate();
  }
  m_Controller->ChangeGreenPolarizationMode(CROSS_POLAR);
}
void PolarimetricSynthesisView::GreenAnyPolarizationCallback()
{
  bGreenAnyPolarization->set();
  bGreenCrossPolarization->clear();
  bGreenCoPolarization->clear();
  vGreenPsiR->activate();
  vGreenKhiR->activate();
  m_Controller->ChangeGreenPolarizationMode(ANY_POLAR);
}

// -------------------------- BLUE  -------------------------- //
void PolarimetricSynthesisView::BluePsiICallback()
{
  m_Controller->ChangeBluePsiI(vBluePsiI->value());
}
void PolarimetricSynthesisView::BluePsiRCallback()
{
  m_Controller->ChangeBluePsiR(vBluePsiR->value());
}
void PolarimetricSynthesisView::BlueKhiICallback()
{
  m_Controller->ChangeBlueKhiI(vBlueKhiI->value());
}
void PolarimetricSynthesisView::BlueKhiRCallback()
{
  m_Controller->ChangeBlueKhiR(vBlueKhiR->value());
}
void PolarimetricSynthesisView::BlueCoPolarizationCallback()
{
  bBlueCoPolarization->set();
  bBlueCrossPolarization->clear();
  bBlueAnyPolarization->clear();
  if (!m_Controller->IsVEmissionMode() | !m_Controller->IsHEmissionMode())
  {
    vBluePsiR->deactivate();
    vBlueKhiR->deactivate();
  }
  else
  {
    vBluePsiR->activate();
    vBlueKhiR->activate();
  }
  m_Controller->ChangeBluePolarizationMode(CO_POLAR);
}
void PolarimetricSynthesisView::BlueCrossPolarizationCallback()
{
  bBlueCrossPolarization->set();
  bBlueCoPolarization->clear();
  bBlueAnyPolarization->clear();
  if (!m_Controller->IsVEmissionMode() | !m_Controller->IsHEmissionMode())
  {
    vBluePsiR->deactivate();
    vBlueKhiR->deactivate();
  }
  else
  {
    vBluePsiR->activate();
    vBlueKhiR->activate();
  }
  m_Controller->ChangeBluePolarizationMode(CROSS_POLAR);
}
void PolarimetricSynthesisView::BlueAnyPolarizationCallback()
{
  bBlueAnyPolarization->set();
  bBlueCrossPolarization->clear();
  bBlueCoPolarization->clear();
  vBluePsiR->activate();
  vBlueKhiR->activate();
  m_Controller->ChangeBluePolarizationMode(ANY_POLAR);
}

// -------------------------- GRAYSCALE  -------------------------- //
void PolarimetricSynthesisView::GrayPsiICallback()
{
  m_Controller->ChangeGrayPsiI(vGrayPsiI->value());
}
void PolarimetricSynthesisView::GrayPsiRCallback()
{
  m_Controller->ChangeGrayPsiR(vGrayPsiR->value());
}
void PolarimetricSynthesisView::GrayKhiICallback()
{
  m_Controller->ChangeGrayKhiI(vGrayKhiI->value());
}
void PolarimetricSynthesisView::GrayKhiRCallback()
{
  m_Controller->ChangeGrayKhiR(vGrayKhiR->value());
}
void PolarimetricSynthesisView::GrayCoPolarizationCallback()
{
  bGrayCoPolarization->set();
  bGrayCrossPolarization->clear();
  bGrayAnyPolarization->clear();
  if (!m_Controller->IsVEmissionMode() | !m_Controller->IsHEmissionMode())
  {
    vGrayPsiR->deactivate();
    vGrayKhiR->deactivate();
  }
  else
  {
    vGrayPsiR->activate();
    vGrayKhiR->activate();
  }
  m_Controller->ChangeGrayPolarizationMode(CO_POLAR);
}
void PolarimetricSynthesisView::GrayCrossPolarizationCallback()
{
  bGrayCrossPolarization->set();
  bGrayCoPolarization->clear();
  bGrayAnyPolarization->clear();
  if (!m_Controller->IsVEmissionMode() | !m_Controller->IsHEmissionMode())
  {
    vGrayPsiR->deactivate();
    vGrayKhiR->deactivate();
  }
  else
  {
    vGrayPsiR->activate();
    vGrayKhiR->activate();
  }
  m_Controller->ChangeGrayPolarizationMode(CROSS_POLAR);
}
void PolarimetricSynthesisView::GrayAnyPolarizationCallback()
{
  bGrayAnyPolarization->set();
  bGrayCrossPolarization->clear();
  bGrayCoPolarization->clear();
  vGrayPsiR->activate();
  vGrayKhiR->activate();
  m_Controller->ChangeGrayPolarizationMode(ANY_POLAR);
}

}
