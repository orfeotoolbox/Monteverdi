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

#include "otbHomologousPointExtractionModuleView.h"

#include "otbMsgReporter.h"

#include <FL/Fl_File_Chooser.H>
#include <FL/fl_draw.H>

namespace otb
{

HomologousPointExtractionModuleView
::HomologousPointExtractionModuleView(): m_Controller(), m_Model(), m_FirstImageView(), m_SecondImageView()
{
  m_Model = HomologousPointExtractionModuleModel::New();
  m_FirstImageView = ImageViewType::New();
  m_SecondImageView = ImageViewType::New();
}


HomologousPointExtractionModuleView
::~HomologousPointExtractionModuleView()
{
  // Remove registered visualization components from the interface
//   gImageViewer->remove(m_ImageView->GetFullWidget());
//   gScroll->remove(m_ImageView->GetScrollWidget());
}

void
HomologousPointExtractionModuleView
::SetModel(HomologousPointExtractionModuleModel* model)
{
  m_Model = model;
  m_FirstImageView->SetModel(m_Model->GetVisualizationModel(0)); 
  m_SecondImageView->SetModel(m_Model->GetVisualizationModel(1));
  m_Model->RegisterListener(this);
}

void
HomologousPointExtractionModuleView
::BuildInterface()
{
  std::cout<<"************HomologousPointExtractionModuleView::BuildInterface()************"<<std::endl;

  if(!m_Controller)
    {
    itkExceptionMacro(<<"Controller is not set, can not build view.");
    }

  if(!m_FirstWidgetsController || !m_SecondWidgetsController)
    {
    itkExceptionMacro(<<"Widgets controller is not set, can not build view.");
    }
  // Build the fltk code
  HomologousPointExtractionViewGroup::CreateGUI();

  // Register controllers
   m_FirstImageView->SetController(m_FirstWidgetsController);
   m_SecondImageView->SetController(m_SecondWidgetsController);

   // Remove registered visualization components from the interface
    gFull1->add(m_FirstImageView->GetFullWidget());
    gScroll1->add(m_FirstImageView->GetScrollWidget());
    gZoom1->add(m_FirstImageView->GetZoomWidget());
    gFull1->resizable(m_FirstImageView->GetFullWidget());
    gScroll1->resizable(m_FirstImageView->GetScrollWidget());
    gZoom1->resizable(m_FirstImageView->GetZoomWidget());

    m_FirstImageView->GetFullWidget()->resize(gFull1->x(),gFull1->y(),gFull1->w(),gFull1->h());
    m_FirstImageView->GetScrollWidget()->resize(gScroll1->x(),gScroll1->y(),gScroll1->w(),gScroll1->h());
    m_FirstImageView->GetZoomWidget()->resize(gZoom1->x(),gZoom1->y(),gZoom1->w(),gZoom1->h());

  // Remove registered visualization components from the interface
    gFull2->add(m_SecondImageView->GetFullWidget());
    gScroll2->add(m_SecondImageView->GetScrollWidget());
    gZoom2->add(m_SecondImageView->GetZoomWidget());
    gFull2->resizable(m_SecondImageView->GetFullWidget());
    gScroll2->resizable(m_SecondImageView->GetScrollWidget());
    gZoom2->resizable(m_SecondImageView->GetZoomWidget());

    m_SecondImageView->GetFullWidget()->resize(gFull2->x(),gFull2->y(),gFull2->w(),gFull2->h());
    m_SecondImageView->GetScrollWidget()->resize(gScroll2->x(),gScroll2->y(),gScroll2->w(),gScroll2->h());
    m_SecondImageView->GetZoomWidget()->resize(gZoom2->x(),gZoom2->y(),gZoom2->w(),gZoom2->h());

   // Show and refresh the interface
    this->wMainWindow->show();

     m_FirstImageView->GetFullWidget()->show();
     m_FirstImageView->GetScrollWidget()->show();
     m_FirstImageView->GetZoomWidget()->show();
     m_SecondImageView->GetFullWidget()->show();
     m_SecondImageView->GetScrollWidget()->show();
     m_SecondImageView->GetZoomWidget()->show();

     // Link pixel descriptors (not do before because widgets have to be instanciated)
     m_Controller->LinkPixelDescriptors();
}


void 
HomologousPointExtractionModuleView
::AddPoints()
{
  int x1 = vX1->value();
  int y1 = vY1->value();
  int x2 = vX2->value();
  int y2 = vY2->value();
  m_Controller->AddPoints( x1, y1, x2, y2 );
}

void
HomologousPointExtractionModuleView
::Notify()
{

}

void
HomologousPointExtractionModuleView
::Quit()
{
  MsgReporter::GetInstance()->Hide();
  wMainWindow->hide();
}



}// end namespace
