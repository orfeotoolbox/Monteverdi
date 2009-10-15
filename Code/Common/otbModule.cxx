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
#ifndef __otbModule_cxx
#define __otbModule_cxx

#include "otbModule.h"

namespace otb
{
/** Constructor */
Module::Module() :  m_InstanceId("Unknown"), m_InputsMap(), m_OutputsMap(), m_NeedsPipelineLocking(false), m_Busy(false)
{}

/** Destructor */
Module::~Module()
{}

/** PrintSelf method */
void Module::PrintSelf(std::ostream& os, itk::Indent indent) const
{
  // Call superclass implementation
  Superclass::PrintSelf(os,indent);

  // Print inputs
  os<<indent<<"Number of inputs: "<<m_InputsMap.size()<<std::endl;
  for(InputDataDescriptorMapType::const_iterator inIt = m_InputsMap.begin(); inIt != m_InputsMap.end(); ++inIt)
    {
    os<<indent<<indent<<inIt->second<<std::endl;
    }

  // Print outputs
  os<<indent<<"Number of outputs: "<<m_OutputsMap.size()<<std::endl;
  for(OutputDataDescriptorMapType::const_iterator outIt = m_OutputsMap.begin(); outIt != m_OutputsMap.end(); ++outIt)
    {
    os<<indent<<indent<<outIt->second<<std::endl;
    }
}

/** Add an input data by its key */
void Module::AddInputByKey(const std::string & key, const DataObjectWrapper & data)
{
  std::cout<<"AddInputByKey:key : " << key<<" data : "<<data<<std::endl;

  // Search for the key in the input map
  InputDataDescriptorMapType::iterator it = m_InputsMap.find(key);

  // If the key can not be found, throw an exception
  if(it == m_InputsMap.end())
    {
    itkExceptionMacro(<<"Module has no input with key "<<key);
    }

  // Else, check for type matching and eventually throw an exception
  if(!it->second.IsTypeCompatible(data.GetDataType()))
    {
    itkExceptionMacro(<<"Type mismatch for input with key "<<key<<": expected "<<it->second.GetDataType()<<", received "<<data.GetDataType());
    }

  /** Add the data to the input descriptor */
  it->second.AddData(data);

}

/** Get an output by its key */
const DataObjectWrapper Module::GetOutputByKey(const std::string & key, unsigned int idx) const
{
  // Search for the key in the output map
  OutputDataDescriptorMapType::const_iterator it = m_OutputsMap.find(key);

  // If the key can not be found, throw an exception
  if(it == m_OutputsMap.end())
    {
    itkExceptionMacro(<<"Module has no output with key "<<key);
    }

  // Then if everything is ok, call the assign method
  DataObjectWrapper wrapper = it->second.GetNthData(idx);

  if(!it->second.IsTypeCompatible(wrapper.GetDataType()))
  {
  itkExceptionMacro(<<"Type mismatch for output with key "<<key<<": expected "<<it->second.GetDataType()<<", received "<<wrapper.GetDataType());
  }
  return wrapper;
}

/** Get the Data object descriptor corresponding to the given key */
const InputDataDescriptor & Module::GetInputDataDescriptorByKey(const std::string & key) const
{
  // Search for the key in the output map
  InputDataDescriptorMapType::const_iterator it = m_InputsMap.find(key);

  // If the key can not be found, throw an exception
  if(it == m_InputsMap.end())
    {
    itkExceptionMacro(<<"Module has no input with key "<<key);
    }
  return it->second;
}

/** Get the input data descriptors map */
const Module::InputDataDescriptorMapType & Module::GetInputsMap() const
{
  return m_InputsMap;
}

/** Get the output data descriptors map */
const Module::OutputDataDescriptorMapType & Module::GetOutputsMap() const
{
  return m_OutputsMap;
}
/** Get number of input data by key */
unsigned int Module::GetNumberOfInputDataByKey(const std::string & key) const
{
  // Search for the key in the input map
  InputDataDescriptorMapType::const_iterator it = m_InputsMap.find(key);

  // If the key can not be found, throw an exception
  if(it == m_InputsMap.end())
    {
    itkExceptionMacro(<<"Module has no input with key "<<key);
    }
  return it->second.GetNumberOfData();
}

/** Load cached data */
void Module::LoadCachedData(const DataObjectWrapper & data, const std::string & key, unsigned int idx)
{
  // Search for the key in the output map
  OutputDataDescriptorMapType::iterator it = m_OutputsMap.find(key);
  
  // If the key can not be found, throw an exception
  if(it == m_OutputsMap.end())
    {
    itkExceptionMacro(<<"Module has no output with key "<<key);
    }

  // Change the 
  it->second.CacheNthData(idx,data);
}

/** Check that every mandatory input has been filled and call the
 * protected virtual run method */
void Module::Start()
{
  // Check input parameters
  for(InputDataDescriptorMapType::const_iterator it = m_InputsMap.begin(); it!=m_InputsMap.end();++it)
    {
    if(!it->second.IsConsistent())
      {
      itkExceptionMacro(<<"Input "<<it->second.GetDataKey()<<" is inconsistent.");
      }
    }
    // Once parameters have been checked, the run method can be triggered.
    this->Run();
}

/** The custom run command */
void Module::Run()
{
  // Start the threaded run
  this->StartProcess2();

  // Start the watcher
  this->StartProcess1();
}

/** Notify outputs change */
void Module::NotifyOutputsChange()
{
  this->NotifyAll(MonteverdiEvent("OutputsUpdated",m_InstanceId));
}

/** Clear output descriptors */
void Module::ClearOutputDescriptors()
{
  m_OutputsMap.clear();
  m_ProcessObjects.clear();
}

/** Erase an output with its key */
void Module::EraseOutputByKey(const std::string & key)
{
  m_OutputsMap.erase(key);
}

/** The custom run command */
void Module::ThreadedRun()
{
  itkExceptionMacro(<<"Subclasses should override this method");
}

/** The custom run command */
void Module::ThreadedWatch()
{
  itkExceptionMacro(<<"Subclasses should override this method");
}

/** The custom run command */
void Module::RunProcess1(void *)
{
  this->ThreadedWatch();
}

/** The custom run command */
void Module::RunProcess2(void *)
{
  this->ThreadedRun();
}

/** Does this module need to lock pipeline ? */
bool Module::NeedsPipelineLocking() const
{
  return m_NeedsPipelineLocking;
}

/** Is this module working ? */
bool Module::IsBusy() const
{
  return m_Busy;
}

/** Does this module have a lock on pipeline ? */
bool Module::IsLockingPipeline() const
{
  return m_NeedsPipelineLocking && m_Busy;
}

} // End namespace otb

#endif
