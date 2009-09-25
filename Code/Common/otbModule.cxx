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
Module::Module() :  m_InstanceId("Unknown"), m_InputsMap(), m_OutputsMap()
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
  // Search for the key in the input map
  InputDataDescriptorMapType::const_iterator it = m_InputsMap.find(key);

  // If the key can not be found, throw an exception
  if(it == m_InputsMap.end())
    {
    itkExceptionMacro(<<"Module has no input with key "<<key);
    }

  // Else, check for type matching and eventually throw an exception
  if(it->second.GetDataType() != data.GetDataType())
    {
    itkExceptionMacro(<<"Type mismatch for input with key "<<key<<": expected "<<it->second.GetDataType()<<", received "<<data.GetDataType());
    }

  // Then if everything is ok, call the assign method
  this->AssignInputByKey(key,data);

  // Toggle the used flag
  m_InputsMap[key].SetUsed(true);
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
  DataObjectWrapper wrapper = this->RetrieveOutputByKey(key);

  // Check types correctness
  // Else, check for type matching and eventually throw an exception
  if(it->second.GetDataType() != wrapper.GetDataType())
    {
    itkExceptionMacro(<<"Type mismatch for output with key "<<key<<": expected "<<it->second.GetDataType()<<", retrieved "<<wrapper.GetDataType());
    }

  return wrapper;
}

/** Assign input by key. Subclasses should override this method.
 *  When this method is called, key checking and data type matching
 *  is already done. */
void Module::AssignInputByKey(const std::string & key, const DataObjectWrapper & data)
{
  itkExceptionMacro(<<"Subclasses should override this method");
}

/** Retrieve output by key. Subclasses should override this method.
 *  When this method is called, key checking and data type matching
 *  is already done. */
const DataObjectWrapper Module::RetrieveOutputByKey(const std::string & key) const
{
  itkExceptionMacro(<<"Subclasses should override this method");
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

/** Check that every mandatory input has been filled and call the
 * protected virtual run method */
void Module::Start()
{
  // Check input parameters
  for(InputDataDescriptorMapType::const_iterator it = m_InputsMap.begin(); it!=m_InputsMap.end();++it)
    {
    if(!it->second.IsOptional() && !it->second.IsUsed())
      {
      itkExceptionMacro(<<"Mandatory input "<<it->second.GetDataKey()<<" is not used.");
      }
    }
    // Once parameters have been checked, the run method can be triggered.
    this->Run();
}

/** The custom run command */
void Module::Run()
{
  itkExceptionMacro(<<"Subclasses should override this method");
}

} // End namespace otb

#endif
