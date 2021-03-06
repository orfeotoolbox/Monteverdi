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

#include <FL/Fl.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Color_Chooser.H>

namespace otb
{

VectorizationView
::VectorizationView() :  m_Controller(),
  m_WidgetController(),
  m_Model(),
  m_ImageView(),
  m_VectorDataGlComponent(),
  m_VectorDataTreeBrowser(),
  m_IsHide(false)
{
  m_ImageView = ImageViewType::New();
  m_VectorDataGlComponent = VectorDataGlComponentType::New();
  m_VectorDataTreeBrowser = VectorDataTreeBrowserType::New();
  m_ImageView->GetFullWidget()->AddGlComponent(m_VectorDataGlComponent);
  m_ImageView->GetScrollWidget()->AddGlComponent(m_VectorDataGlComponent);
  m_ImageView->GetZoomWidget()->AddGlComponent(m_VectorDataGlComponent);

  // Init with red
  m_Color.Fill(0);
  m_Color[0]=1.;
  m_Color[3]=1;
  

  //
  m_SelectedPolygonGLComponent = VectorDataGlComponentType::New();
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
  guiVectorDataTreeGroup->remove(m_VectorDataTreeBrowser);
}

void
VectorizationView
::SetModel(VectorizationModel* model)
{
  m_Model = model;
  m_ImageView->SetModel(m_Model->GetVisualizationModel());
  m_VectorDataGlComponent->SetVectorData(m_Model->GetVectorDataModel()->GetVectorData());
  m_VectorDataTreeBrowser->SetVectorData(m_Model->GetVectorDataModel()->GetVectorData());
  m_Model->RegisterListener(this);
  m_Model->GetVectorDataModel()->RegisterListener(m_ImageView);
}


void
VectorizationView
::UpdateModel()
{
  m_VectorDataGlComponent->SetVectorData(m_Model->GetVectorDataModel()->GetVectorData());
  m_VectorDataTreeBrowser->SetVectorData(m_Model->GetVectorDataModel()->GetVectorData());
  m_Model->GetVectorDataModel()->RegisterListener(m_ImageView);
}


void
VectorizationView
::SetController(VectorizationControllerInterface * controller)
{
  m_Controller = controller;
  m_VectorDataTreeBrowser->SetController(controller);
  this->Modified();
}

void
VectorizationView
::BuildInterface()
{
  if (!m_Controller)
    {
    itkExceptionMacro(<< "Controller is not set, can not build view.");
    }

  if (!m_WidgetController)
    {
    itkExceptionMacro(<< "Widgets controller is not set, can not build view.");
    }

  // Build the fltk code
  this->CreateGUI();

  // Display navigation modes
  vNavigationMode->add(
    "Mouse left: add point, mouse middle: navigate, mouse right: end geometry");
  vNavigationMode->add(
    "Mouse left: navigate, mouse middle: add point, mouse right: end geometry");
  vNavigationMode->value(0);
  vNavigationMode->redraw();

  // Register controllers
  m_ImageView->SetController(m_WidgetController);
  
  // Show
  this->Show();
  this->InitColor();
}


void
VectorizationView
::InitColor()
{
  Fl::check();
  m_Color =  m_VectorDataGlComponent->GetColor();

  // Code commented to avoid segfault with crontab and dora nightly
  /*fl_color(static_cast<unsigned char>((double)(255) * m_Color[0]),
           static_cast<unsigned char>((double)(255) * m_Color[1]),
           static_cast<unsigned char>((double)(255) * m_Color[2])); */
  Fl::check();
  
  // Init the color of the text
  //bColor->color(fl_color());
  bColor->color(FL_RED); // set directly to red color
  bColor->redraw();
  
  vAlpha->value(m_Color[3]);
  vAlpha->redraw();
  sAlpha->value(m_Color[3]);
  sAlpha->redraw();
}

void
VectorizationView
::RedrawWidgets()
{
  m_ImageView->GetFullWidget()->redraw();
  m_ImageView->GetScrollWidget()->redraw();
  m_ImageView->GetZoomWidget()->redraw();
  m_VectorDataTreeBrowser->Update();
 
  // Change the current used method for segmentation
  if(bAutomaticMode->value())
    {
    Fl_Text_Buffer * currentAlgoBuffer = new Fl_Text_Buffer();
    int currentAlgorithm = m_Model->GetActualLayerNumber();
    TCurrentAlgo->buffer(currentAlgoBuffer);
    TCurrentAlgo->insert(m_Model->GetAlgorithmsNameList()[currentAlgorithm].c_str());
    }
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

  m_ImageView->GetFullWidget()->resize(gFull->x(), gFull->y(), gFull->w(), gFull->h());
  m_ImageView->GetScrollWidget()->resize(gScroll->x(), gScroll->y(), gScroll->w(), gScroll->h());
  m_ImageView->GetZoomWidget()->resize(gZoom->x(), gZoom->y(), gZoom->w(), gZoom->h());

  m_ImageView->GetFullWidget()->show();
  m_ImageView->GetScrollWidget()->show();
  m_ImageView->GetZoomWidget()->show();

  // Add the vector data tree browser to his group
  guiVectorDataTreeGroup->add(m_VectorDataTreeBrowser);
  guiVectorDataTreeGroup->resizable(m_VectorDataTreeBrowser);
  m_VectorDataTreeBrowser->resize(guiVectorDataTreeGroup->x(),
                                  guiVectorDataTreeGroup->y(),
                                  guiVectorDataTreeGroup->w(),
                                  guiVectorDataTreeGroup->h());

  
  // Add a gl component for the polygon candidate to be added to the
  // final vector data
  m_SelectedPolygonGLComponent->SetVectorData(m_Model->GetSelectedVectorData());
  m_SelectedPolygonGLComponent->RenderPolygonBoundariesOnlyOn();
  m_ImageView->GetFullWidget()->AddGlComponent(m_SelectedPolygonGLComponent);
  m_ImageView->GetScrollWidget()->AddGlComponent(m_SelectedPolygonGLComponent);
  m_ImageView->GetZoomWidget()->AddGlComponent(m_SelectedPolygonGLComponent);
  
  // Update the tree
  m_VectorDataTreeBrowser->show();
}

void
VectorizationView
::HideAll()
{
  wMainWindow->hide();
  m_IsHide = true;
  m_Model->Notify();
  m_IsHide = false;
}

void VectorizationView
::Notify()
{
  // If the extracted region has changed compute the new one and
  // reprocess the segmentation
  if(!m_Model->GetExtractRegionUpdated() && bAutomaticMode->value())
    {
    m_Controller->ExtractRegion();
    }
  
  // Nothing done for now
  this->RedrawWidgets();
}

void VectorizationView
::NextGeometryIsPointCallback()
{
  m_Model->GetVectorDataModel()->SetCurrentNodeType(FEATURE_POINT);
}

void VectorizationView
::NextGeometryIsLineCallback()
{
  m_Model->GetVectorDataModel()->SetCurrentNodeType(FEATURE_LINE);
}

void VectorizationView
::NextGeometryIsPolygonExtCallback()
{
  m_Model->GetVectorDataModel()->SetCurrentNodeType(FEATURE_POLYGON);
}

void VectorizationView
::NextGeometryIsPolygonIntCallback()
{
  m_Model->GetVectorDataModel()->SetCurrentNodeType(FEATURE_POLYGON);
}

void VectorizationView
::ChangeNavigationModeCallback()
{
  m_Controller->ChangeNavigationMode();
}

void VectorizationView
::UpdateColorCallback()
{

  double r = (double) m_Color[0];
  double g = (double) m_Color[1];
  double b = (double) m_Color[2];
  
  int ok = fl_color_chooser("Changed class color", r, g, b);
  
  if (ok)
    {
      m_Color[0] = (float) r;
      m_Color[1] = (float) g;
      m_Color[2] = (float) b;
      
      fl_color(static_cast<unsigned char>(255 * m_Color[0]),
               static_cast<unsigned char>(255 * m_Color[1]),
               static_cast<unsigned char>(255 * m_Color[2]));

      // Change the color of the text
      bColor->color(fl_color());
      bColor->redraw();

      m_VectorDataGlComponent->SetColor(m_Color);
      m_ImageView->Update();
    }
}

 void VectorizationView
::UpdateAlphaCallback()
{
  m_Color[3] = static_cast<double>(vAlpha->value());
  m_VectorDataGlComponent->SetColor(m_Color);
  m_ImageView->Update();
}

 void VectorizationView
::OKCallback()
{
  m_Controller->OK();
  wMainWindow->hide();
}

/**
 * The automatic and manual vectorization mode is defined by the
 * action used in the handler. The  method switches from manuel
 * vectordata handler to automatic
 */
void
VectorizationView::ButtonAutomaticCallbackOn()
{
  m_Controller->ButtonAutomaticCallbackOn();
  
  //guiVectorDataTreeGroup->remove(m_VectorDataTreeBrowser);
  // Add the vectorDataTreeBrowser to the automatic tree
  guiFinalVectorDataTreeGroup->add(m_VectorDataTreeBrowser);
  guiFinalVectorDataTreeGroup->resizable(m_VectorDataTreeBrowser);
  m_VectorDataTreeBrowser->resize(guiFinalVectorDataTreeGroup->x(),
                                  guiFinalVectorDataTreeGroup->y(),
                                  guiFinalVectorDataTreeGroup->w(),
                                  guiFinalVectorDataTreeGroup->h());

  // Show the automatic computed polygons
  m_SelectedPolygonGLComponent->SetVisible(true);
  
  // Set the used segmentation method list
  Fl_Text_Buffer* algoListBuffer = new Fl_Text_Buffer();
  TAlgolist->buffer(algoListBuffer);
  for (unsigned int i=0; i<m_Model->GetAlgorithmsNameList().size(); i++)
    {
    TAlgolist->insert(m_Model->GetAlgorithmsNameList()[i].c_str());
    }
}

void
VectorizationView::ButtonAutomaticCallbackOff()
{
  m_Controller->ButtonAutomaticCallbackOff();

  guiVectorDataTreeGroup->add(m_VectorDataTreeBrowser);
  guiVectorDataTreeGroup->resizable(m_VectorDataTreeBrowser);
  m_VectorDataTreeBrowser->resize(guiVectorDataTreeGroup->x(),
                                  guiVectorDataTreeGroup->y(),
                                  guiVectorDataTreeGroup->w(),
                                  guiVectorDataTreeGroup->h());

  // Hide the automatic computed polygons
  m_SelectedPolygonGLComponent->SetVisible(false);
}
} // end namespace
