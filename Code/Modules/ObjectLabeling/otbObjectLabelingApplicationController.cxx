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
#ifndef __otbObjectLabelingApplicationController_cxx
#define __otbObjectLabelingApplicationController_cxx

#include "otbObjectLabelingApplicationController.h"
#include "otbMsgReporter.h"
#include <FL/fl_ask.H>

namespace otb
{

ObjectLabelingApplicationController::ObjectLabelingApplicationController() : m_View(), m_WidgetsController(), 
									     m_ResizingHandler(),m_ChangeScaleHandler(), m_ChangeRegionHandler(), 
									     m_ChangeScaledRegionHandler(), m_PixelDescriptionHandler(), m_MouseClickHandler()
{
  // Build the widgets controller
  m_WidgetsController         = WidgetsControllerType::New();
  
  // Build the action handlers
  m_ResizingHandler           = ResizingHandlerType::New();
  m_ChangeScaleHandler        = ChangeScaleHandlerType::New();
  m_ChangeRegionHandler       = ChangeRegionHandlerType::New();
  m_ChangeScaledRegionHandler = ChangeScaledRegionHandlerType::New();
  m_PixelDescriptionHandler   = PixelDescriptionActionHandlerType::New();
  m_MouseClickHandler         = MouseClickActionHandlerType::New();

  // Register the model to the action handlers
  m_ResizingHandler->SetModel(m_Model->GetVisualizationModel());
  m_ChangeScaleHandler->SetModel(m_Model->GetVisualizationModel());
  m_ChangeRegionHandler->SetModel(m_Model->GetVisualizationModel());
  m_ChangeScaledRegionHandler->SetModel(m_Model->GetVisualizationModel());
  m_PixelDescriptionHandler->SetModel(m_Model->GetPixelDescriptionModel());
  m_MouseClickHandler->SetModel(m_Model);

  // Set up mouse click handler
  m_MouseClickHandler->SetMouseButton(3);
  m_MouseClickHandler->ActiveOnScrollWidgetOff();

  // Add the action handlers to the widgets controller
  m_WidgetsController->AddActionHandler(m_ResizingHandler);
  m_WidgetsController->AddActionHandler(m_ChangeScaleHandler);
  m_WidgetsController->AddActionHandler(m_ChangeRegionHandler);
  m_WidgetsController->AddActionHandler(m_ChangeScaledRegionHandler);
  m_WidgetsController->AddActionHandler(m_PixelDescriptionHandler);
  m_WidgetsController->AddActionHandler(m_MouseClickHandler);
}

ObjectLabelingApplicationController::~ObjectLabelingApplicationController()
{}

void ObjectLabelingApplicationController::SetView(ObjectLabelingApplicationView * view)
{
  m_View = view;
  m_ResizingHandler->SetView(m_View->GetImageView());
  m_ChangeScaleHandler->SetView(m_View->GetImageView());
  m_ChangeRegionHandler->SetView(m_View->GetImageView());
  m_ChangeScaledRegionHandler->SetView(m_View->GetImageView());
  m_PixelDescriptionHandler->SetView(m_View->GetImageView());
  m_MouseClickHandler->SetView(m_View->GetImageView());
}

void ObjectLabelingApplicationController::ClassSelected(unsigned int classIndex)
{
  try
    {
    m_Model->SelectClass(classIndex);
    }
  catch(itk::ExceptionObject & err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    }
}

void ObjectLabelingApplicationController::ClearSelectedClass()
{ 
  try
    {
    m_Model->ClearSelectedClass();
    }
  catch(itk::ExceptionObject & err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    }
}

void ObjectLabelingApplicationController::AddClass()
{  
  try
    {
    m_Model->AddClass();
    }
  catch(itk::ExceptionObject & err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    }
}
void ObjectLabelingApplicationController::RemoveClass(unsigned int classIndex)
{
  try
    {
    m_Model->RemoveClass(classIndex);
    }
  catch(itk::ExceptionObject & err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    }
}
void ObjectLabelingApplicationController::ClearClasses()
{  
  try
    {
    m_Model->ClearClasses();
    }
  catch(itk::ExceptionObject & err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    }
}
void ObjectLabelingApplicationController::ChangeClassColor(unsigned int classIndex,const ColorType & color)
{
  try
    {
    m_Model->SetClassColor(color,classIndex);
    }
  catch(itk::ExceptionObject & err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    }
}
void ObjectLabelingApplicationController::ChangeClassName(unsigned int classIndex,const char * name)
{
  try
    {
    m_Model->SetClassName(name,classIndex);
    }
  catch(itk::ExceptionObject & err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    }
}
void ObjectLabelingApplicationController::ChangeClassLabel(unsigned int classIndex,const LabelType & label)
{
  try
    {
    m_Model->SetClassLabel(label,classIndex);
    }
  catch(itk::ExceptionObject & err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    }
}
void ObjectLabelingApplicationController::RemoveObject(unsigned int objectIndex)
{
  if(m_Model->HasSelectedClass())
    {
    try
      {
      m_Model->RemoveSampleFromClass(m_Model->GetClass(m_Model->GetSelectedClass()).m_Samples[objectIndex],m_Model->GetSelectedClass());
      }
    catch(itk::ExceptionObject & err)
      {
      MsgReporter::GetInstance()->SendError(err.GetDescription());
      }
    }
}

void ObjectLabelingApplicationController::SelectObject(unsigned int objectIndex)
{
  if(m_Model->HasSelectedClass())
    {
    try
      {
      m_Model->FocusOnSample(m_Model->GetClass(m_Model->GetSelectedClass()).m_Samples[objectIndex]);
      m_Model->SelectSample(m_Model->GetClass(m_Model->GetSelectedClass()).m_Samples[objectIndex]);
      

      }
    catch(itk::ExceptionObject & err)
      {
      MsgReporter::GetInstance()->SendError(err.GetDescription());
      }
    }				    
}

void ObjectLabelingApplicationController::ClearObjects()
{
  if(m_Model->HasSelectedClass())
    {
    try
      {
      m_Model->ClearSamplesFromClass(m_Model->GetSelectedClass());
      }
    catch(itk::ExceptionObject & err)
      {
      MsgReporter::GetInstance()->SendError(err.GetDescription());
      }
    }

}

void ObjectLabelingApplicationController::SaveSamplesToXMLFile(const char * fname)
{
  try
    {
    m_Model->SaveSamplesToXMLFile(fname);
    }
  catch(itk::ExceptionObject & err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    }
}

void ObjectLabelingApplicationController::LoadSamplesFromXMLFile(const char * fname)
{
  try
    {
    m_Model->LoadSamplesFromXMLFile(fname);
    }
  catch(itk::ExceptionObject & err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    }
}

void ObjectLabelingApplicationController::SaveClassificationParametersToXMLFile(const char * fname)
{
  try
    {
    m_Model->SaveClassificationParametersToXMLFile(fname);
    }
  catch(itk::ExceptionObject & err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    }
}


void ObjectLabelingApplicationController::SaveColorsToAsciiFile(const char * fname)
{
  try
    {
    m_Model->SaveColorsToAsciiFile(fname);
    }
  catch(itk::ExceptionObject & err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    }
}

void ObjectLabelingApplicationController::SaveClassification(const char * fname)
{
  try
    {
    m_Model->SaveClassification(fname);
    }
  catch(itk::ExceptionObject & err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    }
}

void ObjectLabelingApplicationController::SaveClassificationGraph(const char * fname)
{
  try
    {
    m_Model->SaveClassificationGraph(fname);
    }
  catch(itk::ExceptionObject & err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    }
}

void ObjectLabelingApplicationController::ExportClassificationToGIS(const GISExportInfo& exportInfo)
{
  try
    {
    m_Model->ExportClassificationToGIS(exportInfo);
    }
  catch(itk::ExceptionObject & err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    }
  catch(std::exception & e)
    {
    MsgReporter::GetInstance()->SendError(e.what());
    }
}
// void ObjectLabelingApplicationController::LoadImages(const char * image, const char * label)
// {
// //   try
// //     {
// //     m_Model->OpenImage(image,label);
// //     m_View->UpdateViewerSetup();
// //     }
// //   catch(itk::ExceptionObject & err)
// //     {
// //     MsgReporter::GetInstance()->SendError(err.GetDescription());
// //     }
// }

void ObjectLabelingApplicationController::SampleMargin()
{
  try
    {
    m_Model->SampleMargin();
    }
  catch(itk::ExceptionObject & err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    }
}


void ObjectLabelingApplicationController::ChangeKernelType(int kernel)
{
  try
    {
    m_Model->GetSVMEstimator()->SetKernelType(kernel);
    }
  catch(itk::ExceptionObject & err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    }
}

void ObjectLabelingApplicationController::ChangeNumberOfCrossValidationFolders(unsigned int nb)
{ 
  try
    {
    m_Model->GetSVMEstimator()->SetNumberOfCrossValidationFolders(nb);
    }
  catch(itk::ExceptionObject & err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    }
}

void ObjectLabelingApplicationController::ChangeParametersOptimisation(bool value)
{
  try
    {
    m_Model->GetSVMEstimator()->SetParametersOptimization(value);
    }
  catch(itk::ExceptionObject & err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    }
}

void ObjectLabelingApplicationController::ChangeNumberOfCoarseSteps(unsigned int nb)
{
  try
    {
    m_Model->GetSVMEstimator()->SetCoarseOptimizationNumberOfSteps(nb);
    }
  catch(itk::ExceptionObject & err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    }
}

void ObjectLabelingApplicationController::ChangeNumberOfFineSteps(unsigned int nb)
{
  try
    {
    m_Model->GetSVMEstimator()->SetFineOptimizationNumberOfSteps(nb);
    }
  catch(itk::ExceptionObject & err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    }
}

void ObjectLabelingApplicationController::ChangeNumberOfMarginSamples(unsigned int nb)
{
  try
    {
    m_Model->GetMarginSampler()->SetNumberOfCandidates(nb);
    }
  catch(itk::ExceptionObject & err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    }
}

void ObjectLabelingApplicationController::ChangeMarginColor(const ColorType & color){}

void ObjectLabelingApplicationController::FocusOnMarginSample(unsigned int sample)
{
  if(sample>0 && sample <= m_Model->GetMarginSamples().size())
    {
    try
      {
      m_Model->FocusOnSample(m_Model->GetMarginSamples()[sample-1]);
      m_Model->SelectSample(m_Model->GetMarginSamples()[sample-1]);
      }
    catch(itk::ExceptionObject & err)
      {
      MsgReporter::GetInstance()->SendError(err.GetDescription());
      }
    }
}

void ObjectLabelingApplicationController::ClearMarginSamples()
{
  try
    {
    m_Model->ClearMarginSamples();
    }
  catch(itk::ExceptionObject & err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    }
}

void ObjectLabelingApplicationController::Classify()
{
  m_Model->Classify();
}
void ObjectLabelingApplicationController::ClearClassification()
{
  m_Model->ClearClassification();
}

void ObjectLabelingApplicationController::ChangeFeatureState(const std::string & fname,bool state)
{
  try
    {
    m_Model->ChangeFeatureState(fname,state);
    }
  catch(itk::ExceptionObject & err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    }
}

void ObjectLabelingApplicationController::SetUseContext(bool context)
{
  try
    {
    m_Model->SetUseContext(context);
    }
  catch(itk::ExceptionObject & err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    }
}


void ObjectLabelingApplicationController::ChangeClassificationOpacity(double value)
{
  try
    {
    m_Model->ChangeClassificationOpacity(value);
    }
  catch(itk::ExceptionObject & err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    }
}

void ObjectLabelingApplicationController::UpdateViewerDisplay()
{
  if(!m_Model->GetVectorImage())
    {
      return;
    }
  
  std::vector<unsigned int> channels;
  if (m_View->rViewerSetupColorMode->value())
    {
      channels.push_back(atoi(m_View->iRChannelChoice->value())-1);
      channels.push_back(atoi(m_View->iGChannelChoice->value())-1);
      channels.push_back(atoi(m_View->iBChannelChoice->value())-1);							    
    }
  else if (m_View->rViewerSetupGrayscaleMode->value())
    {
      channels.push_back(atoi(m_View->iGrayscaleChannelChoice->value())-1);
    }

  m_Model->UpdateViewerDisplay(channels);
}


} // end namespace otb

#endif
