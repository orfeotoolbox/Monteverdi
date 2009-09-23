
#include "itkExceptionObject.h"
#include "otbMagicMain.h"
#include "otbAsynchronousModel.h"


/*
int main(int argc, char* argv[])
{
  
  typedef otb::Process::AsynchronousModel ProcessorType;
  ProcessorType * model = new ProcessorType();

  model->Start();
 
  while(1)
    {
    }
  delete model;

  return EXIT_SUCCESS;
}
*/

int main(int argc, char* argv[])
{
  typedef otb::Process::AsynchronousModel ProcessorType;
  typedef otb::Process::MagicMain         MagicMainType;
  ProcessorType * model = new ProcessorType();
  MagicMainType * myMain = new MagicMainType();

  model->Register(myMain);
  model->LaunchProcess();
  model->Start();
 
  while(1)
    {
    }
  delete model;
  delete myMain;
  
  
  return EXIT_SUCCESS;
}



