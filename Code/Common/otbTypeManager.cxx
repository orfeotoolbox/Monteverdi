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

#include "otbImage.h"
#include "otbVectorImage.h"
#include "otbVectorData.h"

namespace otb
{
/** Initialize singleton */
TypeManager::Pointer TypeManager::Instance  = NULL;

/** Ensure instance uniqueness */
TypeManager * TypeManager::GetInstance()
  {
    if(!Instance)
      {
      Instance = TypeManager::New();
      }
    return Instance;
  }

TypeManager::TypeManager() : m_IdToNameMap()
{
  // Some basic typedefs
  typedef double                                       Floating_Point_Precision;
  typedef unsigned short                               Label_Precision;
  typedef otb::Image<Floating_Point_Precision,2>       Floating_Point_Image;
  typedef otb::Image<Label_Precision,2>                Labeled_Image;
  typedef otb::VectorImage<Floating_Point_Precision,2> Floating_Point_VectorImage;
  typedef otb::VectorData<Floating_Point_Precision>    Vector_Data;
  
  // Register basic types
  this->RegisterType<Floating_Point_Image>("Floating_Point_Image");
  this->RegisterType<Labeled_Image>("Labeled_Image");
  this->RegisterType<Floating_Point_VectorImage>("Floating_Point_VectorImage");
  this->RegisterType<Vector_Data>("Vector_Data");
}

/** Destructor */
TypeManager::~TypeManager() {}
 
/** PrintSelf */
void TypeManager::PrintSelf(std::ostream& os, itk::Indent indent) const
{
  // Call the superclass implementation
  Superclass::PrintSelf(os,indent);

  // Display registered type info
  os<<"Registered types: "<<std::endl;
  for(IdToNameMapType::const_iterator it = m_IdToNameMap.begin();
      it!=m_IdToNameMap.end();++it)
    {
    os<<indent<<"typeId: "<<it->first<<", name: "<<it->second<<std::endl;
    }
}
} // End namespace otb
