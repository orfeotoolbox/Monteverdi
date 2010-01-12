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
#ifndef __otbAsynchronousProcessBase_cxx
#define __otbAsynchronousProcessBase_cxx

#include "otbAsynchronousProcessBase.h"

namespace otb
{
  //namespace Process
  //{


/** Constructor */
AsynchronousProcessBase
::AsynchronousProcessBase() : m_ThreadId1(0), m_ThreadId2(0), m_Thread1HasFinished(true), m_Thread2HasFinished(true)
{
  m_Threader = ThreaderType::New();
}


/** Destructor */

void
AsynchronousProcessBase
::StartProcess1()
{
  m_ThreadId1 = m_Threader->SpawnThread(RunProcess1_static, this);
}

void
AsynchronousProcessBase
::StopProcess1()
{
  m_Threader->TerminateThread( m_ThreadId1 );
}

void
AsynchronousProcessBase
::StartProcess2()
{
  m_ThreadId2 = m_Threader->SpawnThread(RunProcess2_static, this);
}

void
AsynchronousProcessBase
::StopProcess2()
{
  m_Threader->TerminateThread( m_ThreadId2 );
}

//} // namespace Process
} //namespace otb

#endif
