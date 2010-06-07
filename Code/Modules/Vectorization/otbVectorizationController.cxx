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
#include "otbVectorizationController.h"
#include "otbMsgReporter.h"

namespace otb
{

VectorizationController
::VectorizationController(): m_View(),
                             m_Model(),
                             m_WidgetController(),
                             m_ResizingHandler(),
                             m_ChangeRegionHandler(),
                             m_ChangeScaledRegionHandler(),
                             m_ChangeScaleHandler()
{
  // Build the widgets controller
  m_WidgetController          = WidgetControllerType::New();

  // Build the action handlers
  m_ResizingHandler           = ResizingHandlerType::New();
  m_ChangeRegionHandler       = ChangeRegionHandlerType::New();
  m_ChangeScaledRegionHandler = ChangeScaledRegionHandlerType::New();
  m_ChangeScaleHandler        = ChangeScaleHandlerType::New();

  // Add the action handlers to the widgets controller
  m_WidgetController->AddActionHandler(m_ResizingHandler);
  m_WidgetController->AddActionHandler(m_ChangeRegionHandler);
  m_WidgetController->AddActionHandler(m_ChangeScaledRegionHandler);
  m_WidgetController->AddActionHandler(m_ChangeScaleHandler);
}


VectorizationController
::~VectorizationController()
{
  MsgReporter::GetInstance()->Hide();
}

void
VectorizationController
::SetModel(ModelType* model)
{
  m_Model = model;
  // Register the model to the action handlers
  m_ResizingHandler->SetModel(m_Model->GetVisualizationModel());
  m_ChangeRegionHandler->SetModel(m_Model->GetVisualizationModel());
  m_ChangeScaledRegionHandler->SetModel(m_Model->GetVisualizationModel());
  m_ChangeScaleHandler->SetModel(m_Model->GetVisualizationModel());
}

void
VectorizationController
::SetView(VectorizationView * view)
{
  m_View = view;
  m_ResizingHandler->SetView(m_View->GetImageView());
  m_ChangeRegionHandler->SetView(m_View->GetImageView());
  m_ChangeScaledRegionHandler->SetView(m_View->GetImageView());
  m_ChangeScaleHandler->SetView(m_View->GetImageView());
}
} // end namespace otb
