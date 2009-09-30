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
#ifndef __otbModule_txx
#define __otbModule_txx

#include "otbModule.h"

namespace otb
{

/** Add a new input descriptor */
template <typename T> void Module::AddInputDescriptor(const std::string & key, const std::string & description, bool optional, bool multiple)
{
  // Check if the key already exists
  if(m_InputsMap.count(key) > 0)
    {
    itkExceptionMacro(<<"An input with key "<<key<<" already exists !");
    }

  // Else build a new descriptor
  InputDataDescriptor desc(TypeManager::GetInstance()->GetTypeName<T>(),key,description);
  desc.SetOptional(optional);
  desc.SetMultiple(multiple);

  // Insert it into the map
  m_InputsMap[key]=desc;
}

/** Add additional supported types for a given input descriptors */
template <typename T> void Module::AddTypeToInputDescriptor(const std::string & key)
{
  // Check if the key already exists
  if(m_InputsMap.count(key) <= 0)
    {
    itkExceptionMacro(<<"No input with key "<<key);
    }
  m_InputsMap[key].AddSupportedType(TypeManager::GetInstance()->GetTypeName<T>());
}

/** Add a new output descriptor */
template <typename T> void Module::AddOutputDescriptor(T* data, const std::string & key, const std::string & description)
{
  // Check if the key already exists
  if(m_OutputsMap.count(key) > 0)
    {
    itkExceptionMacro(<<"An Output with key "<<key<<" already exists !");
    }

  // Create a DataObjectWrapper
  DataObjectWrapper wrapper = DataObjectWrapper::Create(data);
  wrapper.SetSourceInstanceId(m_InstanceId);
  wrapper.SetSourceOutputKey(key);
  wrapper.SetDescription(description);

  // Else build a new descriptor
  OutputDataDescriptor desc(wrapper,key,description);
    
  // Insert it into the map
  m_OutputsMap[key]=desc;
}

/** Add a new output descritpor (SmartPointer version) */
template <typename T> void Module::AddOutputDescriptor(itk::SmartPointer<T> data, const std::string & key, const std::string & description)
{
  this->AddOutputDescriptor(data.GetPointer(),key,description);
}

/** Add data to the output descriptor */
template <typename T> void Module::AddDataToOutputDescriptor(T * data, const std::string & key)
{
 
  // Search for the key in the output map
  OutputDataDescriptorMapType::iterator it = m_OutputsMap.find(key);

  // If the key can not be found, throw an exception
  if(it == m_OutputsMap.end())
    {
    itkExceptionMacro(<<"Module has no output with key "<<key);
    }
  
  // Create a DataObjectWrapper
  DataObjectWrapper wrapper = DataObjectWrapper::Create(data);
  wrapper.SetSourceInstanceId(m_InstanceId);
  wrapper.SetSourceOutputKey(key);

  // Add data to the descriptor
  it->second.AddData(wrapper);
}

/** Add data to the output descriptor (SmartPointer version) */
template <typename T> void Module::AddDataToOutputDescriptor(itk::SmartPointer<T> data, const std::string & key)
{
  this->AddDataToOutputDescriptor(data.GetPointer(),key);
}

/** Retrieve the actual data from the map (returns NULL if wrong DataType */
template <typename T> T * Module::GetInputData(const std::string & key, unsigned int idx) const
{
  // Search for the key in the input map
  InputDataDescriptorMapType::const_iterator it = m_InputsMap.find(key);
  
  // If the key can not be found, throw an exception
  if(it == m_InputsMap.end())
    {
    itkExceptionMacro(<<"Module has no input with key "<<key);
    }
  
  // Check if type are compatible
  if(!it->second.IsTypeCompatible(TypeManager::GetInstance()->GetTypeName<T>()))
    {
    itkExceptionMacro(<<"Type "<<TypeManager::GetInstance()->GetTypeName<T>()<<" incompatible with available types for this input: "<<it->second.GetDataType());
    }

  // If type is compatible, try to convert
  T * resp = dynamic_cast<T*>(it->second.GetNthData(idx).GetDataObject());
 
  return resp;
}

/** Retrieve the actual data description from the map */
template <typename T> std::string Module::GetInputDataDescription(const std::string & key, unsigned int idx) const
{
  /*
  T * data = this->GetInputData<T>(key, idx);
  DataObjectWrapper wrapper = DataObjectWrapper::Create(data);
  */

  // Search for the key in the input map
  InputDataDescriptorMapType::const_iterator it = m_InputsMap.find(key);
  
  // If the key can not be found, throw an exception
  if(it == m_InputsMap.end())
    {
    itkExceptionMacro(<<"Module has no input with key "<<key);
    }
  
  // Check if type are compatible
  if(!it->second.IsTypeCompatible(TypeManager::GetInstance()->GetTypeName<T>()))
    {
    itkExceptionMacro(<<"Type "<<TypeManager::GetInstance()->GetTypeName<T>()<<" incompatible with available types for this input: "<<it->second.GetDataType());
    }

  return it->second.GetNthData(idx).GetDescription();
}



} // end namespace otb

#endif
