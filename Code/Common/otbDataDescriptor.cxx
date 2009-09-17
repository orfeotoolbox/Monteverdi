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

namespace otb
{
/** Constructors */
DataDescriptor::DataDescriptor() : m_DataType("Unknown"), m_DataKey("Unknown"), m_DataDescription("Unknown"), m_Optional(false), m_Multiple(false), m_NumberOfData(1)
{}

DataDescriptor::DataDescriptor(const std::string & type, const std::string & key, const std::string & description)
{
  // Call to the set method
  Set(type,key,description);
  m_NumberOfData = 1;
  m_Optional = false;
  m_Multiple = false;
  m_Used     = false;
}

/** Destructor */
DataDescriptor::~DataDescriptor()
{}

/** Set the data object mandatory types */
void DataDescriptor::Set(const std::string & type, const std::string & key, const std::string & description)
{
  m_DataType = type;
  m_DataKey  = key;
  m_DataDescription = description;
}

/** Get the DataType */
const std::string & DataDescriptor::GetDataType() const
{
  return m_DataType;
}

/** Get the DataKey */
const std::string & DataDescriptor::GetDataKey() const
{
  return m_DataKey;
}

/** Get the DataDescription */
const std::string & DataDescriptor::GetDataDescription() const
{
  return m_DataDescription;
}

/** Is the input data optional ? */
bool DataDescriptor::IsOptional() const
{
  return m_Optional;
}

/** Is the input data multiple ? */
bool DataDescriptor::IsMultiple() const
{
  return m_Multiple;
}

/** Is the input data used ? */
bool DataDescriptor::IsUsed() const
{
  return m_Used;
}

/** Set the optional flag */
void DataDescriptor::SetOptional(bool flag)
{
  m_Optional = flag;
}

/** Set the multiple flag */
void DataDescriptor::SetMultiple(bool flag)
{
  m_Multiple = flag;
}

/** Set the used flag */
void DataDescriptor::SetUsed(bool flag)
{
  m_Used = flag;
}

/** Get the number of data */
unsigned int DataDescriptor::GetNumberOfData() const
{
  return m_NumberOfData;
}

/** Set the number of data */
void DataDescriptor::SetNumberOfData(unsigned int nb)
{
  m_NumberOfData = nb;
}

/** Overloading the << operator */
std::ostream & operator<<(std::ostream & ostr, const DataDescriptor & descriptor)
{
  ostr<<"DataDescriptor: type = "<<descriptor.GetDataType()
      <<", key = "<<descriptor.GetDataKey()
      <<", description = "<<descriptor.GetDataDescription()
      << (descriptor.IsOptional() ? ", optional" : ", mandatory")
      << (descriptor.IsMultiple() ? ", multiple" : ", single")
      << (descriptor.IsUsed()     ? ", used"     : ", free")
      << ", number of data = "<<descriptor.GetNumberOfData();
  return ostr;
}

} // End namespace otb

#endif
