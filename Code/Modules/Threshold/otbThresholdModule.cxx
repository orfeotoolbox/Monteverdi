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
#include "otbThresholdModule.h"
#include "otbMsgReporter.h"
#include "otbFltkFilterWatcher.h"

namespace otb
{
/** Constructor */
ThresholdModule::ThresholdModule()
{
  // Build a view
  m_View                 = ViewType::New();
  
  // Build a controller
  m_Controller           = WidgetControllerType::New();
  
  // Build a new rendering model
  m_RenderingModel       = RenderingModelType::New();

  // Wire the MVC
  m_View->SetModel(m_RenderingModel);
  m_View->SetController(m_Controller);
  
//   // Add the resizing handler
//   ResizingHandlerType::Pointer resizingHandler = ResizingHandlerType::New();
//   resizingHandler->SetModel(m_RenderingModel);
//   resizingHandler->SetView(m_View);
//   m_Controller->AddActionHandler(resizingHandler);
   
//   // Add the change extract region handler
//   ChangeRegionHandlerType::Pointer changeHandler =ChangeRegionHandlerType::New();
//   changeHandler->SetModel(m_RenderingModel);
//   changeHandler->SetView(m_View);
//   m_Controller->AddActionHandler(changeHandler);
   
//   // Add the change scaled region handler
//   ChangeScaledRegionHandlerType::Pointer changeScaledHandler =ChangeScaledRegionHandlerType::New();
//   changeScaledHandler->SetModel(m_RenderingModel);
//   changeScaledHandler->SetView(m_View);
//   m_Controller->AddActionHandler(changeScaledHandler);
  
//   // Add the change scaled handler
//   ChangeScaleHandlerType::Pointer changeScaleHandler =ChangeScaleHandlerType::New();
//   changeScaleHandler->SetModel(m_RenderingModel);
//   changeScaleHandler->SetView(m_View);
//   m_Controller->AddActionHandler(changeScaleHandler);

//   // Add the action handler for the arrow key
//   ArrowKeyMoveActionHandlerType::Pointer arrowKeyMoveHandler = ArrowKeyMoveActionHandlerType::New();
//   arrowKeyMoveHandler->SetModel(m_RenderingModel);
//   arrowKeyMoveHandler->SetView(m_View);
//   m_Controller->AddActionHandler(arrowKeyMoveHandler);

  // Build GUI
  this->CreateGUI();
    
  //m_ThresholdFilter = ThresholdFilterType::New();
  //Describe inputs
  this->AddInputDescriptor<ImageType>("InputImage","Image to threshold");
}

/** Destructor */
ThresholdModule::~ThresholdModule()
{}

/** PrintSelf method */
void ThresholdModule::PrintSelf(std::ostream& os, itk::Indent indent) const
{
  // Call superclass implementation
  Superclass::PrintSelf(os,indent);
}

/** The custom run command */
void ThresholdModule::Run()
{
  // Get input image
  m_InputImage = this->GetInputData<ImageType>("InputImage");
  
  // Generate the layer
  ImageLayerGeneratorPointerType generator = ImageLayerGeneratorType::New();
  generator->SetImage(m_InputImage);
  FltkFilterWatcher qlwatcher(generator->GetResampler(),0,0,200,20,"Generating QuickLook ...");
  generator->GenerateLayer();
  m_InputImageLayer = generator->GetLayer();
  
  // Add the generated layer to the rendering model
  m_RenderingModel->AddLayer(generator->GetLayer());
  m_RenderingModel->Update();
  
  // Add the widget to the GUI
  gFull->add(m_View->GetFullWidget());
  gFull->resizable(m_View->GetFullWidget());
  m_View->GetFullWidget()->resize(gFull->x(),gFull->y(),
				  gFull->w(),gFull->h());
  
  gScroll->add(m_View->GetScrollWidget());
  gScroll->resizable(m_View->GetScrollWidget());
  m_View->GetScrollWidget()->resize(gScroll->x(),gScroll->y(),
				    gScroll->w(),gScroll->h()); 
  
  // Show the main window
  wMainWindow->show();
  gScroll->show();
  gFull->show();
}

void ThresholdModule::OK()
{

}


} // End namespace otb


