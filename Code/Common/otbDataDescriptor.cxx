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

#ifndef __otbDataDescriptor_cxx
#define __otbDataDescriptor_cxx

#include "otbDataDescriptor.h"
#include "otbMsgReporter.h"

namespace otb
{
/** Constructors */
DataDescriptor::DataDescriptor(): m_DataType("Unknown"), m_DataKey("Unknown"), m_DataDescription("Unknown"), m_Data()
{}

DataDescriptor::DataDescriptor(const std::string& type, const std::string& key, const std::string& description)
{
  // Call to the set method
  Set(type, key, description);
}

/** Destructor */
DataDescriptor::~DataDescriptor()
{}

/** Set the data object mandatory types */
void DataDescriptor::Set(const std::string& type, const std::string& key, const std::string& description)
{
  m_DataType = type;
  m_DataKey  = key;
  m_DataDescription = description;
}

/** Get the DataType */
const std::string& DataDescriptor::GetDataType() const
{
  return m_DataType;
}

/** Get the DataKey */
const std::string& DataDescriptor::GetDataKey() const
{
  return m_DataKey;
}

/** Set the DataKey */
void DataDescriptor::SetDataKey(const std::string& key)
{
  m_DataKey = key;
}

/** Get the DataDescription */
const std::string& DataDescriptor::GetDataDescription() const
{
  return m_DataDescription;
}

/** Check if the given type is compatible with the input (virtual
 * because behaviour depends on subclasses) */
bool DataDescriptor::IsTypeCompatible(const std::string& type) const
{
  return (type == m_DataType);
}

/** Get number of data */
unsigned int DataDescriptor::GetNumberOfData() const
{
  return m_Data.size();
}

/** Add data (virtual because behaviour depends
 *  on subclasses) */
void DataDescriptor::AddData(const DataObjectWrapper& wrapper)
{
  if (this->IsTypeCompatible(wrapper.GetDataType()))
    {
    m_Data.push_back(wrapper);
    }
  else
    {
    itkGenericExceptionMacro(
      << "Incompatible type. Expected " << this->GetDataType() << ", received " << wrapper.GetDataType());
    }

}

/** Get nth data */
const DataObjectWrapper& DataDescriptor::GetNthData(const unsigned int& idx) const
{
  try
    {
    return m_Data.at(idx);
    }
  catch (std::out_of_range& err)
    {
    itkGenericExceptionMacro(<< "Index " << idx << " out of range for descriptor " << m_DataKey);
       MsgReporter::GetInstance()->SendError(std::string("Out of Range : ")+ err.what());
    }
}

/** Clear data */
void DataDescriptor::ClearData()
{
  m_Data.clear();
}

/** Overloading the << operator */
std::ostream& operator <<(std::ostream& ostr, const DataDescriptor& descriptor)
{
  ostr << "DataDescriptor: type = " << descriptor.GetDataType()
  << ", key = " << descriptor.GetDataKey()
  << ", description = " << descriptor.GetDataDescription()
  << ", Number of data: " << descriptor.GetNumberOfData();
  return ostr;
}

} // End namespace otb

#endif
