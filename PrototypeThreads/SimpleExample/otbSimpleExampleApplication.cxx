
#include "itkExceptionObject.h"
#include "otbAsynchronousProcess.h"

int main(int argc, char* argv[])
{
  typedef otb::Process::AsynchronousProcess ProcessorType;
  ProcessorType * proc1 = new ProcessorType("toto");
  ProcessorType * proc2 = new ProcessorType("tutu");

  itk::MultiThreader::Pointer mainThreader =  itk::MultiThreader::New();
  mainThreader->SetNumberOfThreads(3);
  
  proc1->SetThreader(mainThreader);
  proc2->SetThreader(mainThreader);

  proc1->Start();
  sleep(1);
  proc2->Start();

  bool done = true;
  bool stopped = false;
  int val1, val2 = 0;
  int val1Old, val2Old = -1;

  while(done)
    {
      val1 = proc1->GetProcessStatus();
      val2 = proc2->GetProcessStatus();
      //std::cout<<"Processor 1: "<< val1 <<"/100,  Processor 2: "<< val2 <<"/100"<<std::endl;
      if( val1Old!=val1 || val2Old!=val2 )
 	{
	  std::cout<<"************Processor 1: "<< val1 <<"/100,  Processor 2: "<< val2 <<"/100"<<std::endl;
	}

//       if( val1Old!=val1 || val2Old!=val2 )
// 	{
// 	  if(val1<100 && val2<100)
// 	    std::cout<<"Processor 1: "<< val1 <<"/100,  Processor 2: "<< val2 <<"/100"<<std::endl;
// 	  else if( val1==100 && val2<100 )
// 	    std::cout<<"Processor 1: FINISHED,  Processor 2: "<< val2 <<"/100"<<std::endl;
// 	  else if( val1<100 && val2==100)
// 	    std::cout<<"Processor 1: "<< val1 <<"/100,  Processor 2: FINISHED"<<std::endl;
	  
 	  if( val1 == 100 && val2 == 100 )
 	    {
 	      std::cout<<"Processor 1: FINISHED,  Processor 2: FINISHED"<<std::endl;
 	      done = false;
 	    }
	  //    }

    //   else if( stopped==true && val2Old!=val2 )
// 	{
// 	  if(val2<100)
// 	    std::cout<<"Processor 1: STOPPED,  Processor 2: "<< val2 <<"/100"<<std::endl;
// 	  else if( val1<100 && val2==100)
// 	    std::cout<<"Processor 1: STOPPED,  Processor 2: FINISHED"<<std::endl;
// 	}
         

      if(val1==50 && stopped == false)
	{
	  try
	    {
	      proc1->Stop();
	    }
	  catch(...)
	    {
	    }
	  stopped = true;
	}

      val1Old = val1;
      val2Old = val2; 
    }
  std::cout<<"Processes finished"<<std::endl;


  delete proc1;
  delete proc2; 

  return EXIT_SUCCESS;
}
