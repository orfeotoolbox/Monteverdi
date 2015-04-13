#include "otbMeanShiftModuleController.h"
#include "otbMsgReporter.h"
#include "otbFltkFilterWatcher.h"
#include <FL/fl_ask.H>

namespace otb
{

MeanShiftModuleController
::MeanShiftModuleController() : m_View(), m_Model(), m_WidgetsController(),
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
::SetSpectralRadius(double sr)
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
  try
    {
    if (!m_Model->GetIsImageReady())
      {
      return;
      }

    std::vector<unsigned int> channels;
    if (m_View->rViewerSetupColorMode->value())
      {
      channels.push_back(atoi(m_View->iRChannelChoice->value()) - 1);
      channels.push_back(atoi(m_View->iGChannelChoice->value()) - 1);
      channels.push_back(atoi(m_View->iBChannelChoice->value()) - 1);
      }
    else if (m_View->rViewerSetupGrayscaleMode->value())
      {
      channels.push_back(atoi(m_View->iGrayscaleChannelChoice->value()) - 1);
      }

    m_Model->UpdateViewerDisplay(channels);
    }
  catch (itk::ExceptionObject& err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    return;
    }
  catch (std::exception& e)
    {
    MsgReporter::GetInstance()->SendError(e.what());
    return;
    }
}

void
MeanShiftModuleController
::RunSegmentation()
{
  try
    {
    m_Model->RunSegmentationModel();

    //if (m_View->mBoundButton->value() != 0)
    //  {
    //  m_Model->SwitchBoundaries(true);
    //  }
    }
  catch (itk::ExceptionObject& err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    return;
    }
  catch (std::exception& e)
    {
    std::string exceptionName(e.what());
    if (exceptionName.compare("bad allocation") == 0)
      {
      MsgReporter::GetInstance()->SendError("Not enough memory to run a segmentation on the input image");
      }
    else
      {
      MsgReporter::GetInstance()->SendError(e.what());
      }
    return;
    }
}

/* void
MeanShiftModuleController
::SetOpacity(double op)
{
  try
    {
    m_Model->SetOpacity(op);
    }
  catch (itk::ExceptionObject& err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    return;
    }
  catch (std::exception& e)
    {
    MsgReporter::GetInstance()->SendError(e.what());
    return;
    }
} */

void
MeanShiftModuleController
::SwitchClusters(bool sc)
{
  try
    {
    m_Model->SwitchClusters(sc);
    }
  catch (itk::ExceptionObject& err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    return;
    }
  catch (std::exception& e)
    {
    MsgReporter::GetInstance()->SendError(e.what());
    return;
    }
}

/* void
MeanShiftModuleController
::SwitchBoundaries(bool sb)
{
  try
    {
    m_Model->SwitchBoundaries(sb);
    }
  catch (itk::ExceptionObject& err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    return;
    }
  catch (std::exception& e)
    {
    MsgReporter::GetInstance()->SendError(e.what());
    return;
    }
} */

void
MeanShiftModuleController
::Quit()
{
  try
    {
    m_Model->Quit();
    }
  catch (itk::ExceptionObject& err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    return;
    }
  catch (std::exception& e)
    {
    MsgReporter::GetInstance()->SendError(e.what());
    return;
    }
}

} // end namespace otb
