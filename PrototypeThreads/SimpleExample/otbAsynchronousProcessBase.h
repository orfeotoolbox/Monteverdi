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
#ifndef __otbAsynchronousProcessBase_H
#define __otbAsynchronousProcessBase_H


#include <iostream>
#include "itkMultiThreader.h"


namespace otb
{
namespace Process
{

class AsynchronousProcessBase
{
 public:
  
  typedef itk::MultiThreader ThreaderType;

  void Start();
  void Stop();
  void Wait();
  int GetProcessStatus();
  void SetProcessStatus(int val);


  AsynchronousProcessBase();
  virtual ~AsynchronousProcessBase(){};
  
 void SetThreader(ThreaderType::Pointer threader)
  {
    m_Threader = threader;
  }
  ThreaderType::Pointer GetThreader()
  {
    return m_Threader;
  }

 protected: 

  //virtual ITK_THREAD_RETURN_TYPE Run(void * t) =0;
  
  static ITK_THREAD_RETURN_TYPE Run_static(void * t)
    {
      std::cout<<"Run_static"<<std::endl;
      AsynchronousProcessBase* lThis = (AsynchronousProcessBase*)(t);
      std::cout<<lThis<<std::endl;
      lThis->Run(t);
      std::cout<<"Run_static OUT"<<std::endl;
      return 0;
    }
  
  virtual void Run(void * t)
    {
      throw("Overload la");
    }
 

 private:
  int m_ProcessStatus;
  ThreaderType::Pointer m_Threader;
  //ThreaderType m_Threader;
  int m_ThreadId;
};

} // namespace Process
} // namespace otb

#endif
