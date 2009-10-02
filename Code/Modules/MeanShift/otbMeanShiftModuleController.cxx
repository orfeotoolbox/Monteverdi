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



  // Add the action handlers to the widgets controller
  m_WidgetsController->AddActionHandler(m_ResizingHandler);
  m_WidgetsController->AddActionHandler(m_ChangeRegionHandler);

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

}

void MeanShiftModuleController::SetView(MeanShiftModuleView * view)
{
  m_View = view;
  m_ResizingHandler->SetView(m_View->GetImageView());
  m_ChangeRegionHandler->SetView(m_View->GetImageView());
}

void
MeanShiftModuleController
::OpenImage( const char * filename )
{
  try
    {
    m_ImageReady = false;
    m_Model->OpenImage(filename);
    m_ImageReady = true;
    }
  catch(itk::ExceptionObject & err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    }
}

void
MeanShiftModuleController
::SaveLabelImage( const char * filename )
{
  try
    {
    m_Model->SaveLabelImage(filename);
    }
  catch(itk::ExceptionObject & err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    }
}

void
MeanShiftModuleController
::SaveClusterImage( const char * filename )
{
  try
    {
    m_Model->SaveClusterImage(filename);
    }
  catch(itk::ExceptionObject & err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    }
}


void MeanShiftModuleController::SetSpatialRadius(unsigned int sr)
{
	m_Model->SetSpatialRadius(sr);
}

void MeanShiftModuleController::SetSpectralRadius(unsigned int sr)
{
	m_Model->SetSpectralRadius(sr);
}

void MeanShiftModuleController::SetMinRegionSize(unsigned int mr)
{
	m_Model->SetMinRegionSize(mr);
}


bool
MeanShiftModuleController
::GenerateFiltered()
{
  return m_Model->GetGenerateFiltered();
}

bool
MeanShiftModuleController
::GenerateClustered()
{
  return m_Model->GetGenerateClustered();
}

bool
MeanShiftModuleController
::GenerateLabeled()
{
  return m_Model->GetGenerateLabeled();
}


void
MeanShiftModuleController
::GenerateFiltered(bool t)
{
  m_Model->SetGenerateFiltered(t);
}

void
MeanShiftModuleController
::GenerateClustered(bool t)
{
  m_Model->SetGenerateClustered(t);
}

void
MeanShiftModuleController
::GenerateLabeled(bool t)
{
  m_Model->SetGenerateLabeled(t);
}

void
MeanShiftModuleController
::RunSegmentation()
{
  m_Model->RunSegmentation();
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


ITK_THREAD_RETURN_TYPE
MeanShiftModuleController
::ThreadFunction( void *arg )
{
  try
  {
    ModelType::GetInstance()->RunSegmentation();
  }
  catch (itk::ExceptionObject & err)
  {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
  }
}

} // end namespace otb
