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

MonteverdiController
::MonteverdiController()
{

}

MonteverdiController
::~MonteverdiController()
{}


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
  //  MsgReporter::GetInstance()->SendError(err.GetDescription());
  }
}

void
MonteverdiController
::AddModuleConnection(const std::string& src, const std::string& outKey, const std::string & dest, const std::string & inKey)
{
  m_Model->AddModuleConnection(src,outKey,dest,inKey);
}

} // end namespace otb

