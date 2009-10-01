#include "otbMeanShiftApplicationController.h"
#include "otbMsgReporter.h"
#include "otbFltkFilterWatcher.h"
#include <FL/fl_ask.H>



namespace otb
{

MeanShiftApplicationController
::MeanShiftApplicationController(): m_View(), m_WidgetsController(),
m_ResizingHandler(), m_ChangeRegionHandler()
{
  // Build the widgets controller
  m_WidgetsController         = WidgetsControllerType::New();

  // Build the action handlers
  m_ResizingHandler           = ResizingHandlerType::New();
  m_ChangeRegionHandler       = ChangeRegionHandlerType::New();

  // Register the model to the action handlers
  m_ResizingHandler->SetModel(m_Model->GetVisualizationModel());
  m_ChangeRegionHandler->SetModel(m_Model->GetVisualizationModel());


  // Add the action handlers to the widgets controller
  m_WidgetsController->AddActionHandler(m_ResizingHandler);
  m_WidgetsController->AddActionHandler(m_ChangeRegionHandler);

  m_Threader = itk::MultiThreader::New();

  m_ImageReady = false;

}

MeanShiftApplicationController
::~MeanShiftApplicationController()
{
  MsgReporter::GetInstance()->Hide();
}

void MeanShiftApplicationController::SetView(MeanShiftApplicationView * view)
{
  m_View = view;
  m_ResizingHandler->SetView(m_View->GetImageView());
  m_ChangeRegionHandler->SetView(m_View->GetImageView());
}

void
MeanShiftApplicationController
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
MeanShiftApplicationController
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
MeanShiftApplicationController
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


void MeanShiftApplicationController::SetSpatialRadius(unsigned int sr)
{
	m_Model->SetSpatialRadius(sr);
}

void MeanShiftApplicationController::SetSpectralRadius(unsigned int sr)
{
	m_Model->SetSpectralRadius(sr);
}

void MeanShiftApplicationController::SetMinRegionSize(unsigned int mr)
{
	m_Model->SetMinRegionSize(mr);
}


bool
MeanShiftApplicationController
::GenerateFiltered()
{
  return m_Model->GenerateFiltered();
}

bool
MeanShiftApplicationController
::GenerateClustered()
{
  return m_Model->GenerateClustered();
}

bool
MeanShiftApplicationController
::GenerateLabeled()
{
  return m_Model->GenerateLabeled();
}


void
MeanShiftApplicationController
::GenerateFiltered(bool t)
{
  m_Model->GenerateFiltered(t);
}

void
MeanShiftApplicationController
::GenerateClustered(bool t)
{
  m_Model->GenerateClustered(t);
}

void
MeanShiftApplicationController
::GenerateLabeled(bool t)
{
  m_Model->GenerateLabeled(t);
}

void
MeanShiftApplicationController
::RunSegmentation()
{
  m_Model->RunSegmentation();
}

void
MeanShiftApplicationController
::SwitchClusters(bool sc)
{
  m_Model->SwitchClusters(sc);
}

void
MeanShiftApplicationController
::SwitchBoundaries(bool sb)
{
  m_Model->SwitchBoundaries(sb);
}


ITK_THREAD_RETURN_TYPE
MeanShiftApplicationController
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
