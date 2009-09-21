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
OutputDataDescriptor::OutputDataDescriptor() : DataDescriptor(), m_NumberOfData(1)
{}

OutputDataDescriptor::OutputDataDescriptor(const std::string & type, const std::string & key, const std::string & description) : DataDescriptor(type,key,description), m_NumberOfData(1)
{}

/** Destructor */
OutputDataDescriptor::~OutputDataDescriptor()
{}

/** Get the number of data */
unsigned int OutputDataDescriptor::GetNumberOfData() const
{
  return m_NumberOfData;
}

/** Set the number of data */
void OutputDataDescriptor::SetNumberOfData(unsigned int nb)
{
  m_NumberOfData = nb;
}

/** Overloading the << operator */
std::ostream & operator<<(std::ostream & ostr, const OutputDataDescriptor & descriptor)
{
  ostr<< "Output data, "
      << static_cast<DataDescriptor>(descriptor)
      << ", number of data = "<<descriptor.GetNumberOfData();
  return ostr;
}

} // End namespace otb

#endif
