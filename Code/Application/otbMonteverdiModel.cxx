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
MonteverdiModel::MonteverdiModel() : m_ModuleDescriptorMap(), m_ModuleMap(), m_InstancesCountMap()
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
    oss<<key<<m_InstancesCountMap[key];

    // Register module instance
    module->SetInstanceId(oss.str());
    m_ModuleMap[oss.str()] = module;
    std::cout<<"New module with id "<<oss.str()<<" created: "<<m_ModuleMap[oss.str()]<<std::endl;

    // Register the main model to receive events from the new module
    module->RegisterListener(this);

    // Notify the view that a new module needs inputs
    this->NotifyAll(MonteverdiEvent("InstanceCreated",oss.str()));

    // Update instances count
    m_InstancesCountMap[key]++;
    }
  else
    {
    itkExceptionMacro(<<"No module with key "<<key<<" has been registered.");
    }
}


void MonteverdiModel::StartModuleByInstanceId(const std::string & id)
{
  this->GetModuleByInstanceId(id)->Start();
}

/** Get available modules map */
const MonteverdiModel::ModuleDescriptorMapType & MonteverdiModel::GetRegisteredModuleDescriptors() const
{
  return m_ModuleDescriptorMap;
}


/** Get available module instances */
const std::vector<std::string> MonteverdiModel::GetAvailableModuleInstanceIds() const
{
  std::vector<std::string> availableModulesInstances;
  ModuleMapType::const_iterator mcIt;
  for(mcIt=m_ModuleMap.begin(); mcIt!=m_ModuleMap.end(); mcIt++)
    {
    availableModulesInstances.push_back(mcIt->first);
    }
  return availableModulesInstances;
}

/** Get outputs for a given module instance */
const MonteverdiModel::OutputDataDescriptorMapType & MonteverdiModel::GetModuleOutputsByInstanceId(const std::string & id) const
{
    return this->GetModuleByInstanceId(id)->GetOutputsMap();
}

/** Get inputs for a given module instance */
const MonteverdiModel::InputDataDescriptorMapType & MonteverdiModel::GetModuleInputsByInstanceId(const std::string & id) const
{
  return this->GetModuleByInstanceId(id)->GetInputsMap();
}

/** Get the pointer to the module by an instanceId */
Module * MonteverdiModel::GetModuleByInstanceId(const std::string & instanceId) const
{
  ModuleMapType::const_iterator mcIt = m_ModuleMap.find(instanceId);
  if(mcIt!=m_ModuleMap.end())
    {
    return mcIt->second;
    }
  else
    {
    itkExceptionMacro(<<"No module instance with id "<<instanceId<<" has been registered.");
    }
}

/** Add a new connection between modules */
void MonteverdiModel::AddModuleConnection(const std::string& sourceModuleId,const std::string& outputKey, const std::string& targetModuleId, const std::string& inputKey)
{
  // Find the source module
  Module::Pointer source = this->GetModuleByInstanceId(sourceModuleId);

  // Find the target module
  Module::Pointer target = this->GetModuleByInstanceId(targetModuleId);

  // Retrieve the output data wrapper from the source module
  DataObjectWrapper outputWrapper = source->GetOutputByKey(outputKey);

  // Add the given data wrapper as an input to target module
  target->AddInputByKey(inputKey,outputWrapper);
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
