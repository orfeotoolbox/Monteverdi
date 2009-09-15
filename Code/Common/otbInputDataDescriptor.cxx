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
#ifndef __otbInputDataDescriptor_cxx
#define __otbInputDataDescriptor_cxx

#include "otbInputDataDescriptor.h"

namespace otb
{
/** Constructors */
InputDataDescriptor::InputDataDescriptor() : DataDescriptor(), m_Optional(false), m_Multiple(false), m_Used(false)
{}

InputDataDescriptor::InputDataDescriptor(const std::string & type, const std::string & key, const std::string & description) : DataDescriptor(type,key,description), m_Optional(false), m_Multiple(false), m_Used(false)
{}

/** Destructor */
InputDataDescriptor::~InputDataDescriptor()
{}

/** Is the input data optional ? */
bool InputDataDescriptor::IsOptional() const
{
  return m_Optional;
}

/** Is the input data multiple ? */
bool InputDataDescriptor::IsMultiple() const
{
  return m_Multiple;
}

/** Is the input data used ? */
bool InputDataDescriptor::IsUsed() const
{
  return m_Used;
}

/** Set the optional flag */
void InputDataDescriptor::SetOptional(bool flag)
{
  m_Optional = flag;
}

/** Set the multiple flag */
void InputDataDescriptor::SetMultiple(bool flag)
{
  m_Multiple = flag;
}

/** Set the used flag */
void InputDataDescriptor::SetUsed(bool flag)
{
  m_Used = flag;
}

/** Overloading the << operator */
std::ostream & operator<<(std::ostream & ostr, const InputDataDescriptor & descriptor)
{
  ostr<< "Input data, "
      << static_cast<DataDescriptor>(descriptor)
      << (descriptor.IsOptional() ? ", optional" : ", mandatory")
      << (descriptor.IsMultiple() ? ", multiple" : ", single")
      << (descriptor.IsUsed()     ? ", used"     : ", free");
  return ostr;
}

} // End namespace otb

#endif
