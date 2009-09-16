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
#ifndef __otbMonteverdiModel_cxx
#define __otbMonteverdiModel_cxx

#include "otbMonteverdiModel.h"

namespace otb
{
/**
 * Constructor
 */
MonteverdiModel::MonteverdiModel() : m_ModuleConstructorMap()
{}

MonteverdiModel::~MonteverdiModel()
{}

/** Create a module according to its name. If the name is not a
  registered module, throws an exception */
otb::Module::Pointer MonteverdiModel::CreateModuleByName(const std::string & name) const
{
  ModuleConstructorMapType::const_iterator mcIt = m_ModuleConstructorMap.find(name);

  if(mcIt!=m_ModuleConstructorMap.end())
    {
    return mcIt->second();
    }
  else
    {
    itkExceptionMacro(<<"No module named "<<name<<" has been registered.");
    }
}

}// End namespace

#endif
