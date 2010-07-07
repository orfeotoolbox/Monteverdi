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

#ifndef __otbDataObjectWrapper_cxx
#define __otbDataObjectWrapper_cxx

#include "otbDataObjectWrapper.h"

namespace otb
{

/** Constructors */
DataObjectWrapper::DataObjectWrapper() : m_DataType("Unknown"), m_DataObject(), m_SourceInstanceId("Unknwon"),
  m_SourceOutputKey("Unknown"), m_Description("Unknown")
{}

/** Destructor */
DataObjectWrapper::~DataObjectWrapper()
{}

/** Get the DataObject */
itk::DataObject * DataObjectWrapper::GetDataObject() const
{
  return m_DataObject;
}

/** Get the DataType */
const std::string& DataObjectWrapper::GetDataType() const
{
  return m_DataType;
}

/** Set the source instance id */
void DataObjectWrapper::SetSourceInstanceId(const std::string& id)
{
  m_SourceInstanceId = id;
}

/** Get the source instance id */
const std::string& DataObjectWrapper::GetSourceInstanceId() const
{
  return m_SourceInstanceId;
}

/** Set the source output Key */
void DataObjectWrapper::SetSourceOutputKey(const std::string& key)
{
  m_SourceOutputKey = key;
}

/** Get the source output Key */
const std::string& DataObjectWrapper::GetSourceOutputKey() const
{
  return m_SourceOutputKey;
}

/** Set the description */
void DataObjectWrapper::SetDescription(const std::string& desc)
{
  m_Description = desc;
}

/** Get the description */
const std::string& DataObjectWrapper::GetDescription() const
{
  return m_Description;
}

/** Overloading the << operator */
std::ostream& operator <<(std::ostream& ostr, const DataObjectWrapper& wrapper)
{
  ostr << "DataObjectWrapper: type= " << wrapper.GetDataType()
  << ", object= " << wrapper.GetDataObject()
  << ", source id= " << wrapper.GetSourceInstanceId()
  << ", output key= " << wrapper.GetSourceOutputKey()
  << ", description= " << wrapper.GetDescription();

  return ostr;
}

} // End namespace otb

#endif
