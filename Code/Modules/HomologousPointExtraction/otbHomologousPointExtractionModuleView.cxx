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

HomologousPointExtractionModuleView::HomologousPointExtractionModuleView(): m_Controller(), m_Model(), m_ImageView()
{
  m_Model = HomologousPointExtractionModuleModel::New();
  m_ImageView = ImageViewType::New();
  
}


HomologousPointExtractionModuleView::~HomologousPointExtractionModuleView()
{
  // Remove registered visualization components from the interface
//   gImageViewer->remove(m_ImageView->GetFullWidget());
//   gScroll->remove(m_ImageView->GetScrollWidget());
}

void HomologousPointExtractionModuleView::SetModel(HomologousPointExtractionModuleModel* model)
{
  m_Model = model;
  m_ImageView->SetModel(m_Model->GetVisualizationModel());
  m_Model->RegisterListener(this);
}

void HomologousPointExtractionModuleView::BuildInterface()
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
  HomologousPointExtractionViewGroup::CreateGUI();

  // Register controllers
   m_ImageView->SetController(m_WidgetsController);

   // Remove registered visualization components from the interface
//    gImageViewer->add(m_ImageView->GetFullWidget());
//    gScroll->add(m_ImageView->GetScrollWidget());

//    gImageViewer->resizable(m_ImageView->GetFullWidget());
//    gScroll->resizable(m_ImageView->GetScrollWidget());

//    m_ImageView->GetFullWidget()->resize(gImageViewer->x(),gImageViewer->y(),gImageViewer->w(),gImageViewer->h());
//    m_ImageView->GetScrollWidget()->resize(gScroll->x(),gScroll->y(),gScroll->w(),gScroll->h());

   // Show and refresh the interface
    this->wMainWindow->show();

//     m_ImageView->GetFullWidget()->show();
//     m_ImageView->GetScrollWidget()->show();
}

void HomologousPointExtractionModuleView::Notify()
{

}

void HomologousPointExtractionModuleView::Exit()
{
  MsgReporter::GetInstance()->Hide();
  wMainWindow->hide();
}



}// end namespace
