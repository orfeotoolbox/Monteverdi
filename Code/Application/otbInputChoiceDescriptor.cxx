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
#include "otbInputChoiceDescriptor.h"





namespace otb
{

/** Is the input data optional ? */
bool
InputChoiceDescriptor
::IsOptional() const
{
  return m_Optional;
}

/** Is the input data multiple ? */
bool
InputChoiceDescriptor
::IsMultiple() const
{
 return m_Multiple;
}

InputChoiceDescriptor::StringPairType
InputChoiceDescriptor
::GetSelected() const
{
    std::cout<<"Selected item "<<m_FlChoice->value()<<std::endl;
    return m_ChoiceVector[m_FlChoice->value()];
}

bool
InputChoiceDescriptor
::HasSelected() const
{
  return m_FlChoice->value()>=0;
}

} // end namespace otb

