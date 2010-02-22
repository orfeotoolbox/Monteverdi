#include "otbMeanShiftModuleController.h"
#include "otbMsgReporter.h"
#include "otbFltkFilterWatcher.h"
#include <FL/fl_ask.H>


namespace otb
{

MeanShiftModuleController
::MeanShiftModuleController(): m_View(), m_Model(), m_WidgetsController(),
m_ResizingHandler(), m_ChangeRegionHandler()
{
  // Build the widgets controller
  m_WidgetsController         = WidgetsControllerType::New();

  // Build the action handlers
  m_ResizingHandler           = ResizingHandlerType::New();
  m_ChangeRegionHandler       = ChangeRegionHandlerType::New();
  m_ChangeScaledRegionHandler = ChangeScaledRegionHandlerType::New();
  m_ChangeScaleHandler        = ChangeScaleHandlerType::New();

  // Add the action handlers to the widgets controller
  m_WidgetsController->AddActionHandler(m_ResizingHandler);
  m_WidgetsController->AddActionHandler(m_ChangeRegionHandler);
  m_WidgetsController->AddActionHandler(m_ChangeScaledRegionHandler);
  m_WidgetsController->AddActionHandler(m_ChangeScaleHandler);

  m_Threader = itk::MultiThreader::New();

  m_ImageReady = false;

}

MeanShiftModuleController
::~MeanShiftModuleController()
{
  MsgReporter::GetInstance()->Hide();
}

void MeanShiftModuleController::SetModel(ModelType* model){
  m_Model = model;
  // Register the model to the action handlers
  m_ResizingHandler->SetModel(m_Model->GetVisualizationModel());
  m_ChangeRegionHandler->SetModel(m_Model->GetVisualizationModel());
  m_ChangeScaledRegionHandler->SetModel(m_Model->GetVisualizationModel());
  m_ChangeScaleHandler->SetModel(m_Model->GetVisualizationModel());

}

void MeanShiftModuleController::SetView(MeanShiftModuleView * view)
{
  m_View = view;
  m_ResizingHandler->SetView(m_View->GetImageView());
  m_ChangeRegionHandler->SetView(m_View->GetImageView());
  m_ChangeScaledRegionHandler->SetView(m_View->GetImageView());
  m_ChangeScaleHandler->SetView(m_View->GetImageView());
}


void
MeanShiftModuleController
::SetSpatialRadius(unsigned int sr)
{
  m_Model->SetSpatialRadius(sr);
}

void
MeanShiftModuleController
::SetSpectralRadius(unsigned int sr)
{
  m_Model->SetSpectralRadius(sr);
}

void
MeanShiftModuleController
::SetMinRegionSize(unsigned int mr)
{
  m_Model->SetMinRegionSize(mr);
}

void
MeanShiftModuleController
::UpdateViewerDisplay()
{
  if(!m_Model->GetIsImageReady())
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


void
MeanShiftModuleController
::RunSegmentation()
{
  m_Model->RunSegmentation();
  
  if( m_View->mBoundButton->value() != 0 )
    {
      m_Model->SwitchBoundaries(true);
    }
  
}

void
MeanShiftModuleController
::SetOpacity(double op)
{
  m_Model->SetOpacity(op);
}


void
MeanShiftModuleController
::SwitchClusters(bool sc)
{
  m_Model->SwitchClusters(sc);
}

void
MeanShiftModuleController
::SwitchBoundaries(bool sb)
{
  m_Model->SwitchBoundaries(sb);
}


void
MeanShiftModuleController
::Quit()
{
  m_Model->Quit();
}


} // end namespace otb
