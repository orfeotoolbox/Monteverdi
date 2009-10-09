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

#ifndef __otbOutputDataDescriptor_cxx
#define __otbOutputDataDescriptor_cxx

#include "otbOutputDataDescriptor.h"

namespace otb
{
/** Constructors */
OutputDataDescriptor::OutputDataDescriptor() : DataDescriptor(), m_Cached(false)
{}

OutputDataDescriptor::OutputDataDescriptor(const DataObjectWrapper& data, const std::string & key, const std::string & description, bool cached) : DataDescriptor(data.GetDataType(),key,description), m_Cached(false)
{
  m_Data.push_back(data);
}

/** Destructor */
OutputDataDescriptor::~OutputDataDescriptor()
{}

bool OutputDataDescriptor::IsCached() const
{
  return m_Cached;
}



void OutputDataDescriptor::CacheNthData(unsigned int idx, const DataObjectWrapper & data)
{
  if(idx>=m_Data.size())
    {
    itkGenericExceptionMacro(<<"Data idx out of bounds.");
    }
  m_Data[idx] = data;
  m_Cached = true;
}

/** Overloading the << operator */
std::ostream & operator<<(std::ostream & ostr, const OutputDataDescriptor & descriptor)
{
  ostr<< "Output data, "
      << static_cast<DataDescriptor>(descriptor)
      << ", number of data = "<<descriptor.GetNumberOfData()
      <<", "<<(descriptor.IsCached() ? "Cached" : "Streamed");
  return ostr;
}

} // End namespace otb

#endif
