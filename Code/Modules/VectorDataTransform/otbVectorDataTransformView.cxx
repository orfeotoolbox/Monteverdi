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

#include "otbVectorDataTransformView.h"

#include "otbMsgReporter.h"

#include <FL/Fl_File_Chooser.H>
#include <FL/fl_draw.H>

// VectorData Iterator
#include "itkPreOrderTreeIterator.h"

namespace otb
{

VectorDataTransformView::VectorDataTransformView() : m_Controller(),
                                                           m_Model(), m_ImageView(),
                                                           m_PixelView()
{
  m_ImageView = ImageViewType::New();
  m_PixelView = PixelViewType::New();

  // set the green color
  m_Green.Fill(0.);
  m_Green[1] = 1.;
  m_Green[3] = 1.;
}

VectorDataTransformView::~VectorDataTransformView()
{
  // Remove registered visualization components from the interface
  gFullResolution->remove(m_ImageView->GetFullWidget());
  gScroll->remove(m_ImageView->GetScrollWidget());
  gZoom->remove(m_ImageView->GetZoomWidget());
  gPixelDescription->remove(m_PixelView->GetPixelDescriptionWidget());
}

void VectorDataTransformView::Build()
{
  // Build the visualization part
  m_ImageView->SetModel(m_Model->GetVisualizationModel());
  m_PixelView->SetModel(m_Model->GetPixelDescriptionModel());
  
  if(!m_Controller)
    {
    itkExceptionMacro(<<"Controller is not set, can not build view.");
    }

  if(!m_WidgetsController)
    {
    itkExceptionMacro(<<"Widgets controller is not set, can not build view.");
    }

  // Build the fltk code for Main GUI and DEM
  this->BuildGUI();
  
  // Register controllers
  m_ImageView->SetController(m_WidgetsController);

  // Remove registered visualization components from the interface
  gFullResolution->add(m_ImageView->GetFullWidget());
  gScroll->add(m_ImageView->GetScrollWidget());
  gZoom->add(m_ImageView->GetZoomWidget());
  gPixelDescription->add(m_PixelView->GetPixelDescriptionWidget());
  
  gFullResolution->resizable(m_ImageView->GetFullWidget());
  gScroll->resizable(m_ImageView->GetScrollWidget());
  gZoom->resizable(m_ImageView->GetZoomWidget());
  gPixelDescription->resizable(m_PixelView->GetPixelDescriptionWidget());
  
  m_ImageView->GetZoomWidget()->resize(gZoom->x(),
                                       gZoom->y(),
                                       gZoom->w(),
                                       gZoom->h());
  
  m_ImageView->GetFullWidget()->resize(gFullResolution->x(),
                                       gFullResolution->y(),
                                       gFullResolution->w(),
                                       gFullResolution->h());
  
  m_ImageView->GetScrollWidget()->resize(gScroll->x(),
                                         gScroll->y(),
                                         gScroll->w(),
                                         gScroll->h());
  
  m_PixelView->GetPixelDescriptionWidget()->resize(gPixelDescription->x(),
                                                   gPixelDescription->y(),
                                                   gPixelDescription->w(),
                                                   gPixelDescription->h());

  // Visualize the vectorData
  VectorDataGlComponentType::Pointer glComp = VectorDataGlComponentType::New();
  glComp->SetVectorData(m_Model->GetTransformedVectorData());
  if(guiRenderBoundariesOnly->value())
    glComp->RenderPolygonBoundariesOnlyOn();
  m_ImageView->GetFullWidget()->AddGlComponent(glComp);
  m_ImageView->GetScrollWidget()->AddGlComponent(glComp);
  m_ImageView->GetZoomWidget()->AddGlComponent(glComp);
  
  // Show and refresh the interface
  this->wMainWindow->show();
  
  m_ImageView->GetZoomWidget()->show();
  m_ImageView->GetFullWidget()->show();
  m_ImageView->GetScrollWidget()->show();
  m_PixelView->GetPixelDescriptionWidget()->show();
}

void VectorDataTransformView::SetController(VectorDataTransformControllerInterface * controller)
{
  m_Controller = controller;
}

void VectorDataTransformView::SetWidgetsController(ImageWidgetController * controller)
{
  m_WidgetsController = controller;
}

void VectorDataTransformView::Notify(const std::string & event)
{
  if (event == "VectorDataUpdated")
   {
   this->DisplayVectorData();
   }

  if (event == "InputsOpened")
   {
   this->InitializeGui();
   }

  if(event == "Quit")
    {
    this->Hide();
    }
}

// Done in a specific method cause one need the input
// image to be opened to get its size information
void VectorDataTransformView::InitializeGui()
{
  // Get the size and the origin of the input vectorImage
  VectorDataTransformModel::VectorImageType::SizeType size =
    m_Model->GetVectorImage()->GetLargestPossibleRegion().GetSize();
  
  // Initialize the parameters of the transform in the GUI
  guiScale->value(1.);
  guiRotationAngle->value(0.);
  guiTranslationX->value(0.);
  guiTranslationY->value(0.);
  guiCenterX->value((int)(size[0]/2));
  guiCenterY->value((int)(size[1]/2));
}

void VectorDataTransformView::Exit()
{
  this->m_Controller->Exit();
}


void VectorDataTransformView::DisplayVectorData()
{
  VectorDataGlComponentType::Pointer glComp =
    dynamic_cast<VectorDataGlComponentType*>(m_ImageView->GetFullWidget()->GetNthGlComponent(1));
    
  // Set Visible to true cause in HideOSMVectorData it is turned to off
  glComp->SetVisible(true);

  // Update with the new transformed vector data
  glComp->SetVectorData(m_Model->GetTransformedVectorData());

  // Rendering boundaries only
  if(guiRenderBoundariesOnly->value() == 1)
    glComp->RenderPolygonBoundariesOnlyOn();
  else
    glComp->RenderPolygonBoundariesOnlyOff();

  // Set the color
  glComp->SetColor(m_Green);
  
  // Show the OSM VectorData
  m_ImageView->Update();
}

void VectorDataTransformView::HideVectorData()
{
  // Get the OSM VectorDataGlComponent
  VectorDataGlComponentType::Pointer glComp =
    dynamic_cast<VectorDataGlComponentType*>(m_ImageView->GetFullWidget()->GetNthGlComponent(1));

  // hiding the OSM VectorData means to set Visible to false
  glComp->SetVisible(false);

  // refresh the dispaly
  m_ImageView->Update();
}

void VectorDataTransformView::ApplyTransformToVectorData()
{
  // Get the values from the GUI
  double scale =guiScale->value();
  double angle = guiRotationAngle->value();
  double translationX = guiTranslationX->value();
  double translationY = guiTranslationY->value();
  double centerX = guiCenterX->value();
  double centerY = guiCenterY->value();
  
  m_Controller->ApplyTransformToVectorData(scale, angle, translationX,
                                           translationY, centerX, centerY);
}

void VectorDataTransformView::Hide()
{
  // Hide the main window
  this->wMainWindow->hide();
}

void
VectorDataTransformView::Show()
{
  try
    {
    // Hide previously transformed VectorData
    this->HideVectorData();
    // Rebuild the image view
    this->wMainWindow->show();
    m_ImageView->GetZoomWidget()->show();
    m_ImageView->GetFullWidget()->show();
    m_ImageView->GetScrollWidget()->show();
    m_PixelView->GetPixelDescriptionWidget()->show();
    // Initial State for VectorData (identity transform)
    this->ApplyTransformToVectorData();
    }
  catch (itk::ExceptionObject& err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    }
}

 
} //end namespace otb
