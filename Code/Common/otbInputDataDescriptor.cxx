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

/** Add a type to the supported type for this input */
void InputDataDescriptor::AddSupportedType(const std::string & type)
{
  // Check if new type is already supported
  if(!IsTypeCompatible(type))
    {
    // If not, add the new type
    m_DataType = m_DataType+','+type;
    }
}

/** Check if the given type is compatible with the input */
bool InputDataDescriptor::IsTypeCompatible(const std::string & type) const
{
  // Split the string into types substring
  StringVectorType typesStringVector = SplitTypeName(m_DataType);

  // Searh for the type in the vector
  StringVectorType::const_iterator it = std::find(typesStringVector.begin(),typesStringVector.end(),type);

  // Return true if the type was found
  return (it!=typesStringVector.end());
}


InputDataDescriptor::StringVectorType InputDataDescriptor::SplitTypeName(const std::string & types, char separator)
{
  StringVectorType resp;

  size_t currentSep = 0;
  size_t nextSep = 0; 

  while(nextSep != std::string::npos)
    {
    nextSep = types.find_first_of(separator,currentSep);
    std::string newType = types.substr(currentSep,nextSep-currentSep);
    if(!newType.empty())
      {
      resp.push_back(newType);
      }
    currentSep = nextSep+1;
    }
  return resp;
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
