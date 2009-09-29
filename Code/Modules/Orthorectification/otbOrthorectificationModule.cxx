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

#include "otbOrthorectificationModule.h"

namespace otb
{
/** Constructor */
OrthorectificationModule::OrthorectificationModule()
{
  m_Orthorectification = Orthorectification::New();

  // Describe inputs
  this->AddInputDescriptor<ImageType>("InputImage","Image to apply OrthoRectification on.");
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

/** The custom run command */
void OrthorectificationModule::Run()
{
  ImageType::Pointer input = this->GetInputData<ImageType>("InputImage");

  if(input.IsNotNull())
    {
    m_Orthorectification->Show();
    this->ClearOutputDescriptors();
    this->AddOutputDescriptor(m_Orthorectification->GetOutput(),"OutputImage","Orthorectified image.");
    }
  else
    {
    itkExceptionMacro(<<"InputImage is NULL");
    }
}

} // End namespace otb

