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
#ifndef __otbVectorDataTransformController_cxx
#define __otbVectorDataTransformController_cxx

#include "otbVectorDataTransformController.h"
#include "otbMsgReporter.h"
#include <FL/fl_ask.H>

namespace otb
{

VectorDataTransformController::VectorDataTransformController() : m_View(), m_WidgetsController(),
                                                                 m_ResizingHandler(), m_ChangeScaleHandler(),
                                                                 m_ChangeRegionHandler(), m_ChangeScaledRegionHandler(),
                                                                 m_PixelDescriptionHandler()
{
  // Build the widgets controller
  m_WidgetsController         = WidgetsControllerType::New();
  
  // Build the action handlers
  m_ResizingHandler           = ResizingHandlerType::New();
  m_ChangeScaleHandler        = ChangeScaleHandlerType::New();
  m_ChangeRegionHandler       = ChangeRegionHandlerType::New();
  m_ChangeScaledRegionHandler = ChangeScaledRegionHandlerType::New();
  m_PixelDescriptionHandler   = PixelDescriptionActionHandlerType::New();

  // Add the action handlers to the widgets controller
  m_WidgetsController->AddActionHandler(m_ResizingHandler);
  m_WidgetsController->AddActionHandler(m_ChangeScaleHandler);
  m_WidgetsController->AddActionHandler(m_ChangeRegionHandler);
  m_WidgetsController->AddActionHandler(m_ChangeScaledRegionHandler);
  m_WidgetsController->AddActionHandler(m_PixelDescriptionHandler);
}

VectorDataTransformController::~VectorDataTransformController()
{}


void VectorDataTransformController::Exit()
{
  try
    {
    m_Model->Exit();
    }
  catch(itk::ExceptionObject & err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    }
}

void VectorDataTransformController::SetView(VectorDataTransformView * view)
{
  m_View = view;
  m_ResizingHandler->SetView(m_View->GetImageView());
  m_ChangeScaleHandler->SetView(m_View->GetImageView());
  m_ChangeRegionHandler->SetView(m_View->GetImageView());
  m_ChangeScaledRegionHandler->SetView(m_View->GetImageView());
  m_PixelDescriptionHandler->SetView(m_View->GetImageView());
}


void VectorDataTransformController::ApplyTransformToVectorData(double scale, double angle,
                                                               double translationX,
                                                               double translationY,
                                                               double centerX, double centerY)
{
  try
    {
    m_Model->ApplyTransformToVectorData(scale, angle, translationX, translationY, centerX, centerY);
    }
  catch(itk::ExceptionObject & err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    }
}


} // end namespace otb

#endif
