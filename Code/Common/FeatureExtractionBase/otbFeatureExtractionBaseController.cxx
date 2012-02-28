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

#include "otbFeatureExtractionBaseController.h"
#include "otbMsgReporter.h"

namespace otb
{

FeatureExtractionBaseController
::FeatureExtractionBaseController()
{
  /** NewVisu */
  // Build a "visu"controller
  m_VisuController                   = VisuControllerType::New();
  m_ResultVisuController             = VisuControllerType::New();
  m_MouseClickedController           = MouseClickedController::New();
  m_LeftMouseClickedHandler          = MouseClickedHandlertype::New();
  m_ResizingHandler                  = ResizingHandlerType::New();
  m_ChangeExtractRegionHandler       = ChangeExtractRegionHandlerType::New();
  m_ResultResizingHandler            = ResizingHandlerType::New();
  m_ResultChangeExtractRegionHandler = MyChangeExtractRegionHandlerType::New();

  m_ResultChangeExtractRegionHandler->SetController(this);

  // Link pixel clicked model (controller in relity...)
  m_MouseClickedController->SetMouseButton(1);
  m_MouseClickedController->SetImageViewId(0);
  m_MouseClickedController->SetController(this);
  m_LeftMouseClickedHandler->SetModel(m_MouseClickedController);
  m_LeftMouseClickedHandler->SetActiveOnZoomWidget(false);
  m_LeftMouseClickedHandler->SetActiveOnScrollWidget(false);

  m_VisuController->AddActionHandler(m_ResizingHandler);
  m_VisuController->AddActionHandler(m_ChangeExtractRegionHandler);
  m_VisuController->AddActionHandler(m_ResultChangeExtractRegionHandler);
  m_ResultVisuController->AddActionHandler(m_ResultResizingHandler);
  m_VisuController->AddActionHandler(m_LeftMouseClickedHandler);
}

FeatureExtractionBaseController
::~FeatureExtractionBaseController()
{}

FeatureExtractionBaseControllerInterface::ModelType*
FeatureExtractionBaseController::
GetModel()
{
  return m_Model;
}

void
FeatureExtractionBaseController
::SetModel(ModelType::Pointer model)
{
  m_Model = model;
// Register the model to the action handlers
  m_ResizingHandler->SetModel(m_Model->GetVisuModel());
  m_ChangeExtractRegionHandler->SetModel(m_Model->GetVisuModel());
  m_ResultResizingHandler->SetModel(m_Model->GetResultVisuModel());
  m_ResultChangeExtractRegionHandler->SetModel(m_Model->GetResultVisuModel());
}

void
FeatureExtractionBaseController
::SetView(ViewPointerType pView)
{
  m_View = pView;

  m_ResizingHandler->SetView(m_View->GetVisuView());
  m_ChangeExtractRegionHandler->SetView(m_View->GetVisuView());
  m_ResultResizingHandler->SetView(m_View->GetResultVisuView());
  m_ResultChangeExtractRegionHandler->SetView(m_View->GetVisuView());
  m_LeftMouseClickedHandler->SetView(m_View->GetVisuView());
}

void
FeatureExtractionBaseController
::LeftMouseButtonClicked(ContinuousIndexType index)
{
  IndexType id;
  id[0] = static_cast<long int>(index[0]);
  id[1] = static_cast<long int>(index[1]);
  m_View->UpdateSelectedPixel(id);
}

void
FeatureExtractionBaseController
::ShowBarStatus()
{
  m_View->pBar->value(1);
  m_View->pBar->show();
  Fl::check();
}

void
FeatureExtractionBaseController
::HideBarStatus()
{
  m_View->pBar->hide();
  Fl::check();
}

void
FeatureExtractionBaseController
::SetOutputFileName(const char * name)
{
  try
    {
    m_Model->SetOutputFileName(name);
    }
  catch (itk::ExceptionObject& err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    }
}

void
FeatureExtractionBaseController
::SaveOutput()
{
  try
    {
    m_Model->GenerateOutputImage();
    }
  catch (itk::ExceptionObject& err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    }
}

void
FeatureExtractionBaseController
::ClearFeatures()
{
  try
    {

    m_Model->ClearFilterList();
    m_Model->ClearFilterTypeList();
    m_Model->ClearOutputFilterInformation();
    m_Model->ClearSelectedFilters();
    m_Model->ClearOutputListOrder();
    }
  catch (itk::ExceptionObject& err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    }

}

void
FeatureExtractionBaseController
::ClearSelectedChannels()
{
  try
    {
    this->ClearFeatures();
    m_Model->ClearOutputChannelsInformation();
    m_Model->ClearInputImageList();
    }
  catch (itk::ExceptionObject& err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    }
}

void
FeatureExtractionBaseController
::AddInputChannels(std::vector<unsigned int> chList)
{
  try
    {
    // channel index starts at 1
    m_Model->ClearInputImageList();
    m_Model->AddChannels(chList);
    }
  catch (itk::ExceptionObject& err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    }
}

void
FeatureExtractionBaseController
::ChangeFilterStatus(int id)
{
  try
    {
    m_Model->GetSelectedFilters()[id] = !(m_Model->GetSelectedFilters()[id]);
    }
  catch (itk::ExceptionObject& err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    }
}

void
FeatureExtractionBaseController
::InitInput()
{
  try
    {
    m_Model->InitInput();
    }
  catch (itk::ExceptionObject& err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    }
}

void
FeatureExtractionBaseController
::AddToOutputListOrder(int id)
{
  try
    {
    m_Model->AddToOutputListOrder(id);
    }
  catch (itk::ExceptionObject& err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    }
}

void
FeatureExtractionBaseController
::RemoveFromOutputListOrder(int id)
{
  try
    {
    m_Model->RemoveFromOutputListOrder(id);
    }
  catch (itk::ExceptionObject& err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    }
}

void
FeatureExtractionBaseController
::ExchangeOutputListOrder(int direction)
{
  int id = m_View->guiOutputFeatureList->value() - 1;
  // if switch with  next element
  if (direction > 0)
    {
    if (id != 0) m_Model->ExchangeOutputListOrder(id, id - 1);
    else m_Model->ExchangeOutputListOrder(id, m_View->guiOutputFeatureList->size() - 1);
    }
  // if switch with  previous element
  else
    {
    if (id != m_View->guiOutputFeatureList->size() - 1) m_Model->ExchangeOutputListOrder (id + 1, id);
    else m_Model->ExchangeOutputListOrder (id, 0);
    }
}

void FeatureExtractionBaseController::ViewedRegionChanged()
{
  try
    {
    m_View->UpdateFeaturePreview();
    }
  catch (itk::ExceptionObject& err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    }
}

void
FeatureExtractionBaseController
::CreateFeature(FeatureType featureType)
{
  this->CreateSpecificFeature(featureType);
}

void
FeatureExtractionBaseController::UpdateFeaturePreview(unsigned int id)
{
  try
    {
    this->ShowBarStatus();
    m_Model->GetSingleOutput(id);
    }
  catch (itk::ExceptionObject& err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    }
  this->HideBarStatus();
}

void FeatureExtractionBaseController::Quit()
{
  try
    {
    m_Model->Quit();
    }
  catch (itk::ExceptionObject& err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    }
}

void FeatureExtractionBaseController::Cancel()
{
  try
    {
    m_Model->Cancel();
    }
  catch (itk::ExceptionObject& err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    }
}

} // end namespace otb
