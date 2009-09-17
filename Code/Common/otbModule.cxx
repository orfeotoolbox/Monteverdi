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
Module::Module() : m_DataMap()
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
  os<<indent<<"Number of inputs: "<<m_DataMap.size()<<std::endl;
  for(DataDescriptorMapType::const_iterator inIt = m_DataMap.begin(); inIt != m_DataMap.end(); ++inIt)
    {
    os<<indent<<indent<<inIt->second<<std::endl;
    }

}

/** Add a new input descriptor */
void Module::AddDataDescriptor(const std::string & type, const std::string & key, const std::string & description, bool optional, bool multiple)
{
  // Check if the key already exists
  if(m_DataMap.count(key) > 0)
    {
    itkExceptionMacro(<<"An input with key "<<key<<" already exists !");
    }

  // Else build a new descriptor
  DataDescriptor desc(type,key,description);
  desc.SetOptional(optional);
  desc.SetMultiple(multiple);

  // Insert it into the map
  m_DataMap[key]=desc;
}

/** Add an input data by its key */
void Module::AddDataByKey(const std::string & key, const DataObjectWrapper & data)
{
  // Search for the key in the input map
  DataDescriptorMapType::const_iterator it = m_DataMap.find(key);

  // If the key can not be found, throw an exception
  if(it == m_DataMap.end())
    {
    itkExceptionMacro(<<"Module has no input with key "<<key);
    }

  // Else, check for type matching and eventually throw an exception
  if(it->second.GetDataType() != data.GetDataType())
    {
    itkExceptionMacro(<<"Type mismatch for input with key "<<key<<": expected "<<it->second.GetDataType()<<", received "<<data.GetDataType());
    }

  // Then if everything is ok, call the assign method
  this->AssignDataByKey(key,data);

  // Toggle the used flag
  m_DataMap[key].SetUsed(true);
}

/** Get an output by its key */
const DataObjectWrapper Module::GetDataByKey(const std::string & key, unsigned int idx) const
{
  // Search for the key in the output map
  DataDescriptorMapType::const_iterator it = m_DataMap.find(key);

  // If the key can not be found, throw an exception
  if(it == m_DataMap.end())
    {
    itkExceptionMacro(<<"Module has no output with key "<<key);
    }

  // Then if everything is ok, call the assign method
  DataObjectWrapper wrapper = this->RetrieveDataByKey(key);

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
void Module::AssignDataByKey(const std::string & key, const DataObjectWrapper & data)
{
  itkExceptionMacro(<<"Subclasses should override this method");
}

/** Retrieve output by key. Subclasses should override this method.
 *  When this method is called, key checking and data type matching
 *  is already done. */
const DataObjectWrapper Module::RetrieveDataByKey(const std::string & key) const
{
  itkExceptionMacro(<<"Subclasses should override this method");
}

/** Get the input data descriptors map */
const Module::DataDescriptorMapType & Module::GetDataMap() const
{
  return m_DataMap;
}

/** Check that every mandatory input has been filled and call the
 * protected virtual run method */
void Module::Start()
{
  // Check input parameters
  for(DataDescriptorMapType::const_iterator it = m_DataMap.begin(); it!=m_DataMap.end();++it)
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
