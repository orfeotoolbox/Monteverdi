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
#ifndef __otbOrthorectificationModule_cxx
#define __otbOrthorectificationModule_cxx

#include "otbOrthorectificationModule.h"

namespace otb
{
/** Constructor */
OrthorectificationModule::OrthorectificationModule()
{
  m_Orthorectification = Orthorectification::New();

  // Describe inputs
  this->AddInputDescriptor("Floating_Point_VectorImage","InputImage","Image to apply OrthoRectification on.");
}

/** Destructor */
OrthorectificationModule::~OrthorectificationModule()
{}

/** PrintSelf method */
void OrthorectificationModule::PrintSelf(std::ostream& os, itk::Indent indent) const
{
  // Call superclass implementation
  Superclass::PrintSelf(os,indent);
}

/** Assign input by key. This method must be reimplemented in subclasses.
 *  When this method is called, key checking and data type matching
 *  is already done. */
void OrthorectificationModule::AssignInputByKey(const std::string & key, const DataObjectWrapper & data)
{
  typedef Orthorectification::ImageType InputImageType;

  if(key == "InputImage")
    {
    InputImageType * image = dynamic_cast<InputImageType *>(data.GetDataObject());
    m_Orthorectification->SetInputImage(image);
    }
}

  /** Retrieve output by key  This method must be reimplemented in subclasses.
   *  When this method is called, key checking and data type matching
   *  is already done. */
const DataObjectWrapper OrthorectificationModule::RetrieveOutputByKey(const std::string & key) const
{
  DataObjectWrapper wrapper;
  if(key == "OutputImage")
    {
    wrapper.Set("Short_Point_VectorImage",m_Orthorectification->GetOutput());
    }
  return wrapper;
}

/** The custom run command */
void OrthorectificationModule::Run()
{
  m_Orthorectification->Show();
  this->AddOutputDescriptor("Floating_Point_VectorImage","OutputImage","Orthorectified image.");
}

} // End namespace otb

#endif
