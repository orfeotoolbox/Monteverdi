
#include "itkExceptionObject.h"
#include "otbAsynchronousModel.h"

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
