#include "otbBasicApplicationController.h"
#include "otbMsgReporter.h"
#include "otbFltkFilterWatcher.h"
#include <FL/fl_ask.H>



namespace otb
{

BasicApplicationController
::BasicApplicationController(): m_View(), m_WidgetsController(),
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

BasicApplicationController
::~BasicApplicationController()
{
  MsgReporter::GetInstance()->Hide();
}

void BasicApplicationController::SetView(BasicApplicationView * view)
{
  m_View = view;
  m_ResizingHandler->SetView(m_View->GetImageView());
  m_ChangeRegionHandler->SetView(m_View->GetImageView());
}
 
void
BasicApplicationController
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
bool what = true;
void
BasicApplicationController
::RunLoop()
{
  if(m_ImageReady && !m_Model->IsUpdating())
    {
      //this->FreezeWindow();
      m_Threader->SetNumberOfThreads(1);
      m_Threader->SpawnThread(ThreadFunction, this);
      //this->UnFreezeWindow();
    }
}

ITK_THREAD_RETURN_TYPE
BasicApplicationController
::ThreadFunction( void *arg )
{
  try
  {
    //((BasicApplicationController*) arg)->m_Model->RunLoop();
    ModelType::GetInstance()->RunLoop();
  }
  catch (itk::ExceptionObject & err)
  {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
  }
}


void
BasicApplicationController
::FreezeWindow()
{
  m_WidgetsController->DeactivateActionHandlers();
  m_View->gButton->deactivate();
  m_View->gMenu->deactivate();
}

void
BasicApplicationController
::UnFreezeWindow()
{
  m_WidgetsController->ActivateActionHandlers();
 m_View->gButton->activate();
  m_View->gMenu->activate();
}


} // end namespace otb
