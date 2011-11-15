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
#include "otbTypeManager.h"

namespace otb
{
/** Initialize singleton */
TypeManager::Pointer TypeManager::Instance  = NULL;

/** Ensure instance uniqueness */
TypeManager * TypeManager::GetInstance()
{
  if (!Instance)
    {
    Instance = TypeManager::New();
    }
  return Instance;
}

TypeManager::TypeManager() : m_IdToNameMap()
{
  // Register basic types
  this->RegisterType<Floating_Point_Image>("Floating_Point_Image");
  this->RegisterType<Labeled_Short_Image>("Labeled_Short_Image");
  this->RegisterType<Labeled_Char_Image>("Labeled_Char_Image");
  this->RegisterType<Floating_Point_Complex_Image>("Floating_Point_Complex_Image");
  this->RegisterType<Deformation_Field_Type>("Deformation_Field_Type");

  this->RegisterType<Floating_Point_VectorImage>("Floating_Point_VectorImage");
  this->RegisterType<Floating_Point_Complex_VectorImage>("Floating_Point_Complex_VectorImage");
  this->RegisterType<Labeled_Char_VectorImage>("Labeled_Char_VectorImage");
  this->RegisterType<Labeled_Short_VectorImage>("Labeled_Short_VectorImage");

  this->RegisterType<Vector_Data>("Vector_Data");
  this->RegisterType<Labeled_Vector_Data>("Labeled_Vector_Data");

  this->RegisterType<UInt16ImageWithQuicklook>("PleiadesImage");
}

/** Destructor */
TypeManager::~TypeManager() {}

/** PrintSelf */
void TypeManager::PrintSelf(std::ostream& os, itk::Indent indent) const
{
  // Call the superclass implementation
  Superclass::PrintSelf(os, indent);

  // Display registered type info
  os << "Registered types: " << std::endl;
  for (IdToNameMapType::const_iterator it = m_IdToNameMap.begin();
       it != m_IdToNameMap.end(); ++it)
    {
    os << indent << "typeId: " << it->first << ", name: " << it->second << std::endl;
    }
}
} // End namespace otb
