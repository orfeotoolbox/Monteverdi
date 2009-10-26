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

  //m_Threader = itk::MultiThreader::New();

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
      try
       {
         this->Start();
         m_Model->Start();
             }
      catch (itk::ExceptionObject & err)
       {
         std::cout<<err.GetDescription()<<std::endl;
       }
      
      /*
      //this->FreezeWindow();
      std::cout<<"Run Loop"<<std::endl;
      m_Threader->SetNumberOfThreads(2);
      m_Threader->SpawnThread(ThreadFunction, this);
      std::cout<<"Run Loop END"<<std::endl;
      //this->UnFreezeWindow();
      */
    }
}

void
BasicApplicationController
::Run( void * v )
{
  float progress = 0;
  float progressOld = -1;
  this->FreezeWindow();
  while( progress != 1)
    {
      sleep(1);
      progress = m_Model->GetWriter()->GetProgress();
      float diffProg = progress - progressOld;
  
      if(diffProg > 0.01)
       {
         Fl::lock();
         m_View->UpdateProgressBar( progress );
         Fl::unlock();
         progressOld = progress;
       }

    }
  Fl::lock();
  m_View->UpdateProgressBar( 1 );
  m_View->UpdateProgressBar( 0 );
  Fl::unlock();
  this->UnFreezeWindow();
}


void
BasicApplicationController
::StopLoop()
{
  if(m_Model->IsUpdating())
    {
      try
       {
         std::cout<<"Doesn't work well!!!Stop asked"<<std::endl;
         //m_Model->Stop();
         //std::cout<<"Stop done"<<std::endl;
             }
      catch (itk::ExceptionObject & err)
       {
         std::cout<<err.GetDescription()<<std::endl;
       }
    }
}

ITK_THREAD_RETURN_TYPE
BasicApplicationController
::ThreadFunction( void *arg )
{
  
  try
  {
    //((BasicApplicationController*) arg)->m_Model->RunLoop();
    std::cout<<"ThreadFunction before"<<std::endl;
    //ModelType::GetInstance()->RunLoop();
    std::cout<<"ThreadFunction after"<<std::endl;
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
  Fl::lock();
  m_View->gButton->deactivate();
  m_View->gMenu->deactivate();
  Fl::unlock();
}

void
BasicApplicationController
::UnFreezeWindow()
{
  m_WidgetsController->ActivateActionHandlers();
  Fl::lock();
  m_View->gButton->activate();
  m_View->gMenu->activate();
  Fl::unlock();
}


} // end namespace otb
