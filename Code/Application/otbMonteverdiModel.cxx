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
MonteverdiModel::MonteverdiModel() : m_ModuleDescriptorMap(), m_ModuleMap()
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
    // Create a new module instance
    Module::Pointer module = mcIt->second.m_Constructor();

    // Build a unique key
    itk::OStringStream oss;
    oss<<key<<m_InstancesCountMap[key]<<std::endl;

    // Register module instance
    m_ModuleMap[oss.str()] = module;
    std::cout<<"New module with id "<<oss.str()<<" created: "<<m_ModuleMap[oss.str()]<<std::endl;

    // Register the main model to receive events from the new module
    module->RegisterListener(this);

    // Temporary
    this->StartModuleByKey(oss.str());

    // Update instances count
    m_InstancesCountMap[key]++;
    }
  else
    {
    itkExceptionMacro(<<"No module with key "<<key<<" has been registered.");
    }
}


void MonteverdiModel::StartModuleByKey(const std::string & key)
{
 ModuleMapType::iterator mIt = m_ModuleMap.find(key);

 if(mIt != m_ModuleMap.end())
   {
   mIt->second->Start();
   }
 else
   {
   itkExceptionMacro(<<"No module instance with key "<<key);
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
