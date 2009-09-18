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

/** Initialize the singleton */
MonteverdiModel::Pointer MonteverdiModel::Instance = NULL;


/**
 * Constructor
 */
MonteverdiModel::MonteverdiModel() : m_ModuleDescriptorMap(), m_ModuleList()
{}

MonteverdiModel::~MonteverdiModel()
{}

/** Create a module according to its name. If the name is not a
  registered module, throws an exception */
void MonteverdiModel::CreateModuleByKey(const std::string & key)
{
  ModuleDescriptorMapType::const_iterator mcIt = m_ModuleDescriptorMap.find(key);

  if(mcIt!=m_ModuleDescriptorMap.end())
    {
    m_ModuleList.push_back(mcIt->second.m_Constructor());
    std::cout<<"New module created: "<<m_ModuleList.back()<<std::endl;
    }
  else
    {
    itkExceptionMacro(<<"No module with key "<<key<<" has been registered.");
    }
}

/** Get available modules map */
const MonteverdiModel::ModuleDescriptorMapType & MonteverdiModel::GetRegisteredModuleDescriptors() const
{
  return m_ModuleDescriptorMap;
}

/** Manage the singleton */
MonteverdiModel::Pointer
MonteverdiModel::GetInstance()
{
  if (!Instance)
  {
    Instance = MonteverdiModel::New();
  }
  return Instance;
}

}// End namespace

#endif
