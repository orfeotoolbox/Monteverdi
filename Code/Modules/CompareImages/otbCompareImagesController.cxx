/*=========================================================================

  Program:   ORFEO Toolbox
  Language:  C++
  Date:      $Date$
  Version:   $Revision$l


  Copyright (c) Centre National d'Etudes Spatiales. All rights reserved.
  See OTBCopyright.txt for details.


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "otbCompareImagesController.h"
#include "otbMsgReporter.h"

namespace otb
{

CompareImagesController
::CompareImagesController()
{
  // Build the visu controller
  m_LeftVisuController         = VisuControllerType::New();
  m_RightVisuController        = VisuControllerType::New();
  
  // Build the action handlers
  m_LeftResizingHandler           = ResizingHandlerType::New();
  m_LeftChangeRegionHandler       = ChangeRegionHandlerType::New();
  
  m_RightResizingHandler           = ResizingHandlerType::New();
  m_RightChangeRegionHandler       = MyChangeExtractRegionHandlerType::New();

  // Add the action handlers to the widgets controller
  m_LeftVisuController->AddActionHandler(m_LeftResizingHandler);
  m_LeftVisuController->AddActionHandler(m_LeftChangeRegionHandler);
  m_LeftVisuController->AddActionHandler(m_RightChangeRegionHandler);
  m_RightVisuController->AddActionHandler(m_RightResizingHandler);
  
  // Add the pixel description action handler
  m_LeftPixelActionHandler = PixelDescriptionActionHandlerType::New();
  m_RightPixelActionHandler = PixelDescriptionActionHandlerType::New();
  
  m_LeftVisuController->AddActionHandler(m_LeftPixelActionHandler);
  m_LeftVisuController->AddActionHandler(m_RightPixelActionHandler);
  
  m_RightVisuController->AddActionHandler(m_RightPixelActionHandler);
  m_RightVisuController->AddActionHandler(m_LeftPixelActionHandler);

  
/** NewVisu */
// Build a "visu"controller
}

CompareImagesController
::~CompareImagesController()
{

}


CompareImagesControllerInterface::ModelType*
CompareImagesController::
GetModel()
{
  return m_Model;
}

void 
CompareImagesController
::SetModel(ModelType* model)
{
  m_Model = model;
  // Register the LeftRenderingModel to the action handlers
  m_LeftResizingHandler->SetModel(m_Model->GetLeftVisuModel());
  m_LeftChangeRegionHandler->SetModel(m_Model->GetLeftVisuModel());
  
  // Register the RightRenderingModel to the action handlers
  m_RightResizingHandler->SetModel(m_Model->GetRightVisuModel());
  m_RightChangeRegionHandler->SetModel(m_Model->GetRightVisuModel());

  m_LeftPixelActionHandler->SetModel(m_Model->GetLeftPixelDescriptionModel());
  m_RightPixelActionHandler->SetModel(m_Model->GetRightPixelDescriptionModel());
  

}

void
CompareImagesController
::SetView(CompareImagesView * view)
{
  m_View = view;
  
  // Register controllers
  m_LeftResizingHandler->SetView(m_View->GetLeftVisuView());
  m_LeftChangeRegionHandler->SetView(m_View->GetLeftVisuView());

  m_RightResizingHandler->SetView(m_View->GetRightVisuView());
  m_RightChangeRegionHandler->SetView(m_View->GetLeftVisuView());
  
  m_LeftPixelActionHandler->SetView(m_View->GetLeftVisuView());
  m_RightPixelActionHandler->SetView(m_View->GetRightVisuView());

}

void
CompareImagesController
::SaveQuit()
{
  m_Model->SaveQuit();
}


void
CompareImagesController
::ProcessCompareImagesFilter()
{
  if(m_Model->GetOutputChanged()==false)
    {
    
    m_Model->ComputeCompareImages();
    }
}


} // end namespace otb
