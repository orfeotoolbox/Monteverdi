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

  m_Orthorectification->RegisterListener(this);

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
    m_Orthorectification->SetInputImage(input);
    m_Orthorectification->Show();
    
    }
  else
    {
    itkExceptionMacro(<<"InputImage is NULL");
    }
}

/** The notify */
void OrthorectificationModule::Notify()
{
  if(m_Orthorectification->GetHasOutput())
    {
    this->ClearOutputDescriptors();
    this->AddOutputDescriptor(m_Orthorectification->GetOutput(),"OutputImage","Orthorectified image.");
    this->NotifyOutputsChange();
    }
}

} // End namespace otb

