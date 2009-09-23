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


#include "otbAsynchronousProcessBase.h"

namespace otb
{
namespace Process
{


/** Constructor */
AsynchronousProcessBase
::AsynchronousProcessBase() : m_ProcessStatus(0), m_ThreadId(0)
{
  m_Threader = ThreaderType::New();
}


/** Destructor */

void
AsynchronousProcessBase
::Start()
{
  m_ProcessStatus = 0;
  m_ThreadId = m_Threader->SpawnThread(Run_static, this);
  /*
  int res = pthread_create( &m_ThreadId, 0, (void*(*)(void*))Run_static, (void *)this );

  if(res!=0)
    {
      std::cout<<"Impossible to create new thread, returned value is: "<<res<<std::endl;
      return;
    }
  */
}

void
AsynchronousProcessBase
::Stop()
{
  std::cout<<"Stop called "<<std::endl;
  m_Threader->TerminateThread( m_ThreadId );
  /*
  int res = pthread_cancel( m_ThreadId );
  if(res!=0)
    {
      std::cout<<"Impossible to cancel thread, returned value is: "<<res<<std::endl;
      return;
    }
  */
}

void
AsynchronousProcessBase
::Wait()
{
  std::cout<<"Wait called but not implemented yet"<<std::endl;
  /*  
  int res = pthread_join( m_ThreadId, NULL );
  if(res!=0)
    {
      std::cout<<"Impossible to join thread, returned value is: "<<res<<std::endl;
      return;
    }
  */
  //m_ThreadId->WaitForSingleMethodThread( m_ThreadId );
}


int
AsynchronousProcessBase
::GetProcessStatus()
{
  return m_ProcessStatus;
}

void
AsynchronousProcessBase
::SetProcessStatus( int val )
{
  m_ProcessStatus = val;
}


} // namespace Process
} //namespace otb
