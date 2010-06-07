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

#include "otbVectorizationView.h"
#include "otbMsgReporter.h"

#include <FLU/Flu_File_Chooser.h>
#include <FL/Fl.H>
#include <FL/fl_draw.H>

namespace otb
{

VectorizationView
::VectorizationView() :  m_Controller(),
                         m_WidgetController(),
                         m_Model(),
                         m_ImageView()
{
  m_ImageView = ImageViewType::New();
}


VectorizationView
::~VectorizationView()
{
  // Remove registered visualization components from the interface
  m_ImageView->GetFullWidget()->ClearGlComponents();
  m_ImageView->GetScrollWidget()->ClearGlComponents();
  m_ImageView->GetZoomWidget()->ClearGlComponents();

  gFull->remove(m_ImageView->GetFullWidget());
  gScroll->remove(m_ImageView->GetScrollWidget());
  gZoom->remove(m_ImageView->GetZoomWidget());
}

void
VectorizationView
::SetModel(VectorizationModel* model)
{
  m_Model = model;
  m_ImageView->SetModel(m_Model->GetVisualizationModel());
  m_Model->RegisterListener(this);
}

void
VectorizationView
::BuildInterface()
{
  if(!m_Controller)
    {
      itkExceptionMacro(<<"Controller is not set, can not build view.");
    }

  if(!m_WidgetController)
    {
      itkExceptionMacro(<<"Widgets controller is not set, can not build view.");
    }
    
  // Build the fltk code
  this->CreateGUI();

  // Register controllers
  m_ImageView->SetController(m_WidgetController);
  // Show
  this->Show();
}

void
VectorizationView
::RedrawWidgets()
{
  m_ImageView->GetFullWidget()->redraw();
  m_ImageView->GetScrollWidget()->redraw();
  m_ImageView->GetZoomWidget()->redraw();
}

void
VectorizationView
::Show()
{
  wMainWindow->position(0, 45);
  wMainWindow->show();
  
  // Add registered visualization components from the interface
  gFull->add(m_ImageView->GetFullWidget());
  gScroll->add(m_ImageView->GetScrollWidget());
  gZoom->add(m_ImageView->GetZoomWidget());
  gFull->resizable(m_ImageView->GetFullWidget());
  gScroll->resizable(m_ImageView->GetScrollWidget());
  gZoom->resizable(m_ImageView->GetZoomWidget());

  m_ImageView->GetFullWidget()->resize(gFull->x(),gFull->y(),gFull->w(),gFull->h());
  m_ImageView->GetScrollWidget()->resize(gScroll->x(),gScroll->y(),gScroll->w(),gScroll->h());
  m_ImageView->GetZoomWidget()->resize(gZoom->x(),gZoom->y(),gZoom->w(),gZoom->h());

  m_ImageView->GetFullWidget()->show();
  m_ImageView->GetScrollWidget()->show();
  m_ImageView->GetZoomWidget()->show();
}

void
VectorizationView
::HideAll()
{
 wMainWindow->hide();
}

void VectorizationView
::Notify()
{
  // Nothing done for now
}
} // end namespace
