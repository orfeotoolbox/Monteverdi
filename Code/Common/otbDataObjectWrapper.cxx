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
DataObjectWrapper::DataObjectWrapper() : m_DataType("Unknown"), m_DataObject()
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
const std::string & DataObjectWrapper::GetDataType() const
{
  return m_DataType;
}

/** Overloading the << operator */
std::ostream & operator<<(std::ostream & ostr, const DataObjectWrapper & wrapper)
{
  ostr<<"DataObjectWrapper: type = "<<wrapper.GetDataType()
      <<", object = "<<wrapper.GetDataObject();
  return ostr;
}

} // End namespace otb

#endif
