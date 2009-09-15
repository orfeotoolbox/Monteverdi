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
DataDescriptor::DataDescriptor() : m_DataType("Unknown"), m_DataKey("Unknown"), m_DataDescription("Unknown")
{}

DataDescriptor::DataDescriptor(const std::string & type, const std::string & key, const std::string & description)
{
  // Call to the set method
  Set(type,key,description);
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

/** Overloading the << operator */
std::ostream & operator<<(std::ostream & ostr, const DataDescriptor & descriptor)
{
  ostr<<"DataDescriptor: type = "<<descriptor.GetDataType()
      <<", key = "<<descriptor.GetDataKey()
      <<", description = "<<descriptor.GetDataDescription()<<std::endl;
  return ostr;
}

} // End namespace otb

#endif
