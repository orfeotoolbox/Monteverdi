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

#include "otbSupervisedClassificationModule.h"

namespace otb
{
/** Constructor */
SupervisedClassificationModule::SupervisedClassificationModule()
{
  m_SupervisedClassification = SupervisedClassificationAppli::New();

  m_SupervisedClassification->RegisterListener(this);

  // Describe inputs
  this->AddInputDescriptor<SupervisedClassificationAppli::ImageType>("InputImage","Image to apply Classification on.");
}

/** Destructor */
SupervisedClassificationModule::~SupervisedClassificationModule()
{}

/** PrintSelf method */
void SupervisedClassificationModule::PrintSelf(std::ostream& os, itk::Indent indent) const
{
  // Call superclass implementation
  Superclass::PrintSelf(os,indent);
}

/** The custom run command */
void SupervisedClassificationModule::Run()
{
  InputImageType::Pointer input = this->GetInputData<InputImageType>("InputImage");

  if(input.IsNotNull())
    {
    m_SupervisedClassification->SetInputImage(input);
    m_SupervisedClassification->Build();
    m_SupervisedClassification->Show();
  
    m_SupervisedClassification->LoadImage();
  
    // Check For SVMModel
    if(!m_Model.empty())
      {
      m_SupervisedClassification->SetModelFileName(m_Model);
      m_SupervisedClassification->LoadSVMModel();
      }

    }
  else
    {
    itkExceptionMacro(<<"InputImage is NULL");
    }
}

/** The notify */
void SupervisedClassificationModule::Notify()
{
  if(m_SupervisedClassification->GetHasOutput())
    {
    this->ClearOutputDescriptors();
    this->AddOutputDescriptor(m_SupervisedClassification->GetOutput(),"OutputImage","Classified image.");
    this->NotifyOutputsChange();
    }

  if(m_SupervisedClassification->GetHasOutputVector())
    {
    this->ClearOutputDescriptors();
    this->AddOutputDescriptor(m_SupervisedClassification->GetOutputVector(),"OutputImage","Vectors of classified image.");
    this->NotifyOutputsChange();
    }
}

} // End namespace otb

