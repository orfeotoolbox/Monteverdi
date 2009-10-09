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

#include "otbMonteverdiController.h"
#include "otbMsgReporter.h"

namespace otb
{

/** Create a new instance of module using a module key */
void
MonteverdiController
::CreateModuleByKey(const char * modulekey)
{
  try
  {
    m_Model->CreateModuleByKey(modulekey);
  }
  catch (itk::ExceptionObject & err)
  {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
  }
}

/** Start module by instance id */
void
MonteverdiController
::StartModuleByInstanceId(const std::string& id)
{
  try
  {
  m_Model->StartModuleByInstanceId(id);
  }
  catch (itk::ExceptionObject & err)
  {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
  }
}

/** Create a connection between two modules using :
  *   -ModuleId source
  *   -OutputDataKey of the module source
  *   -ModuleId destination
  *   -InputDataKey of the module destination
  */
void
MonteverdiController
::AddModuleConnection(const std::string& src, const std::string& outKey, const std::string & dest, const std::string & inKey)
{
  try
    {
    m_Model->AddModuleConnection(src,outKey,dest,inKey);
    }
  catch (itk::ExceptionObject & err)
    {
      MsgReporter::GetInstance()->SendError(err.GetDescription());
    }
}

int
MonteverdiController
::ChangeInstanceId( const std::string & oldInstanceId,  const std::string & newInstanceId )
{
  try
    {
      m_Model->ChangeInstanceId( oldInstanceId, newInstanceId );
    }
  catch (itk::ExceptionObject & err)
    {
      MsgReporter::GetInstance()->SendError(err.GetDescription());
      return 1;
    }
  return 0;
}


} // end namespace otb

