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
::VectorizationController() : m_View(),
  m_Model(),
  m_WidgetController(),
  m_ResizingHandler(),
  m_ChangeRegionHandler(),
  m_ChangeScaledRegionHandler(),
  m_ChangeScaleHandler(),
  m_VectorDataActionHandler()
{
  // Build the widgets controller
  m_WidgetController          = WidgetControllerType::New();

  // Build the action handlers
  m_ResizingHandler           = ResizingHandlerType::New();
  m_ChangeRegionHandler       = ChangeRegionHandlerType::New();
  m_ChangeScaledRegionHandler = ChangeScaledRegionHandlerType::New();
  m_ChangeScaleHandler        = ChangeScaleHandlerType::New();
  m_VectorDataActionHandler   = VectorDataActionHandlerType::New();

  // Add the action handlers to the widgets controller
  m_WidgetController->AddActionHandler(m_ResizingHandler);
  m_WidgetController->AddActionHandler(m_ChangeRegionHandler);
  m_WidgetController->AddActionHandler(m_ChangeScaledRegionHandler);
  m_WidgetController->AddActionHandler(m_ChangeScaleHandler);
  m_WidgetController->AddActionHandler(m_VectorDataActionHandler);

  // Specific buttons mapping
  m_ChangeRegionHandler->SetMouseButton(2);
  m_ChangeScaledRegionHandler->SetMouseButton(2);
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
  m_VectorDataActionHandler->SetModel(m_Model->GetVectorDataModel());
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
  m_VectorDataActionHandler->SetView(m_View->GetImageView());
}


void VectorizationController::ChangeNavigationMode()
{
  if(m_ChangeRegionHandler->GetMouseButton() == 1)
    {
      m_ChangeRegionHandler->SetMouseButton(2);
      m_ChangeScaledRegionHandler->SetMouseButton(2);
      m_VectorDataActionHandler->SetAddMouseButton(1);
    }
  else
    {
      m_ChangeRegionHandler->SetMouseButton(1);
      m_ChangeScaledRegionHandler->SetMouseButton(1);
      m_VectorDataActionHandler->SetAddMouseButton(2);
    }

}

void VectorizationController::RemoveDataNode(DataNodeType * node)
{
  try
    {
    m_Model->RemoveDataNode(node);
    }
  catch (itk::ExceptionObject& err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    }
}
void VectorizationController::SetDataNodeFieldAsInt(DataNodeType * node, const std::string& name, int value)
{
  try
    {
    m_Model->SetDataNodeFieldAsInt(node, name, value);
    }
  catch (itk::ExceptionObject& err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    }
}
void VectorizationController::SetDataNodeFieldAsFloat(DataNodeType * node, const std::string& name, float value)
{
  try
    {
    m_Model->SetDataNodeFieldAsFloat(node, name, value);
    }
  catch (itk::ExceptionObject& err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    }
}
void VectorizationController::SetDataNodeFieldAsString(DataNodeType* node,
                                                       const std::string& name,
                                                       const std::string& value)
{
  try
    {
    m_Model->SetDataNodeFieldAsString(node, name, value);
    }
  catch (itk::ExceptionObject& err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    }
}
void VectorizationController::RemoveFieldFromDataNode(DataNodeType * node, const std::string& name)
{
  try
    {
    m_Model->RemoveFieldFromDataNode(node, name);
    }
  catch (itk::ExceptionObject& err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    }
}

void VectorizationController::RemovePointFromDataNode(DataNodeType * node,
                                                      const unsigned long& index,
                                                      bool interiorRing,
                                                      const unsigned int& interiorRingIndex)
{
  try
    {
    m_Model->RemovePointFromDataNode(node, index, interiorRing, interiorRingIndex);
    }
  catch (itk::ExceptionObject& err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    }
}
void VectorizationController::UpdatePointFromDataNode(DataNodeType * node,
                                                      const unsigned long& index,
                                                      const PointType& point,
                                                      bool interiorRing,
                                                      const unsigned int& interiorRingIndex)
{
  try
    {
    m_Model->UpdatePointFromDataNode(node, index, point, interiorRing, interiorRingIndex);
    }
  catch (itk::ExceptionObject& err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    }
}


void VectorizationController::AddVectorData(VectorDataPointer vData)
{
 try
    {
      m_Model->AddVectorData(vData);
    }
 catch (itk::ExceptionObject& err)
   {
     MsgReporter::GetInstance()->SendError(err.GetDescription());
   }
}

void VectorizationController::OK()
{
 try
    {
      m_Model->OK();
    }
 catch (itk::ExceptionObject& err)
   {
     MsgReporter::GetInstance()->SendError(err.GetDescription());
   }
}


} // end namespace otb
