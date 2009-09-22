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
#ifndef __otbOrthoRectifModule_cxx
#define __otbOrthoRectifModule_cxx

#include "otbOrthoRectifModule.h"

namespace otb
{
/** Constructor */
OrthoRectifModule::OrthoRectifModule()
{
  m_OrthoRectif = OrthoRectif::New();

  // Describe inputs
  this->AddInputDescriptor("Short_Point_VectorImage","InputImage","Image to apply OrthoRectification on.");
}

/** Destructor */
OrthoRectifModule::~OrthoRectifModule()
{}

/** PrintSelf method */
void OrthoRectifModule::PrintSelf(std::ostream& os, itk::Indent indent) const
{
  // Call superclass implementation
  Superclass::PrintSelf(os,indent);
}

/** Assign input by key. This method must be reimplemented in subclasses.
 *  When this method is called, key checking and data type matching
 *  is already done. */
void OrthoRectifModule::AssignDataByKey(const std::string & key, const DataObjectWrapper & data)
{
  typedef OrthoRectif::ImageType InputImageType;

  if(key == "InputImage")
    {
    InputImageType * image = dynamic_cast<InputImageType *>(data.GetDataObject());
    m_OrthoRectif->SetInputImage(image);
    }
}

  /** Retrieve output by key  This method must be reimplemented in subclasses.
   *  When this method is called, key checking and data type matching
   *  is already done. */
const DataObjectWrapper OrthoRectifModule::RetrieveDataByKey(const std::string & key) const
{
  DataObjectWrapper wrapper;
  if(key == "OutputImage")
    {
    wrapper.Set("Short_Point_VectorImage",m_OrthoRectif->GetOutput());
    }
  return wrapper;
}

/** The custom run command */
void OrthoRectifModule::Run()
{
  m_OrthoRectif->Show();
  this->AddOutputDescriptor("Short_Point_VectorImage","OutputImage","OrthoRectified image.");
}

} // End namespace otb

#endif
