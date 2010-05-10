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
#include "itkObject.h"
#include "itkExceptionObject.h"
#include "otbThreads.h"

namespace otb
{
  //namespace Process
  //{

class ITK_EXPORT AsynchronousProcessBase
  : public itk::Object
{
 public:
  typedef AsynchronousProcessBase       Self;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;
  typedef itk::Object                   Superclass;
  
  /** New macro */
  itkNewMacro(Self);

  /** Type macro */
  itkTypeMacro(AsynchronousProcessBase,itk::Object);

  typedef itk::MultiThreader ThreaderType;

  void StartProcess1();
  void StopProcess1();
  void StartProcess2();
  void StopProcess2();
 
 void SetThreader(ThreaderType::Pointer threader)
  {
    m_Threader = threader;
  }
  ThreaderType::Pointer GetThreader()
  {
    return m_Threader;
  }

  itkGetConstMacro(Thread1HasFinished, bool);
  itkSetMacro(Thread1HasFinished, bool);
  itkGetConstMacro(Thread2HasFinished, bool);
  itkSetMacro(Thread2HasFinished, bool);

 protected:

  AsynchronousProcessBase();
  virtual ~AsynchronousProcessBase(){};
    /** PrintSelf method */
  virtual void PrintSelf(std::ostream& os, itk::Indent indent) const{};

  static ITK_THREAD_RETURN_TYPE RunProcess1_static(void * t)
    {
      struct itk::MultiThreader::ThreadInfoStruct * pInfo = (itk::MultiThreader::ThreadInfoStruct *)(t);
      AsynchronousProcessBase::Pointer lThis = (AsynchronousProcessBase*)(pInfo->UserData);
      lThis->RunProcess1(t);
      return 0;
    }
  
  virtual void RunProcess1(void * t)
    {
      itkExceptionMacro(<<"The RunProcess1 method has to be overloaded.");
    }
 
  static ITK_THREAD_RETURN_TYPE RunProcess2_static(void * t)
    {
      struct itk::MultiThreader::ThreadInfoStruct * pInfo = (itk::MultiThreader::ThreadInfoStruct *)(t);
      AsynchronousProcessBase::Pointer lThis = (AsynchronousProcessBase*)(pInfo->UserData);
      lThis->RunProcess2(t);
      return 0;
    }
  
  virtual void RunProcess2(void * t)
    {
      itkExceptionMacro(<<"The RunProcess2 method has to be overloaded.");
    }

  // Internal call to openthread::Thread::microSleep
  // by the intermediate of the otb class Threads
  static int Sleep(unsigned int microsec)
  {
    return Threads::Sleep(microsec);
  }

 private:
  AsynchronousProcessBase(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  ThreaderType::Pointer m_Threader;
  int m_ThreadId1;
  int m_ThreadId2;
  bool m_Thread1HasFinished;
  bool m_Thread2HasFinished;
};

  //} // namespace Process
} // namespace otb

#endif
