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
#include "otbSarIntensityModule.h"


namespace otb
{
/** Constructor */
SarIntensityModule::SarIntensityModule()
{
  // First, do constructor stuffs

  // Then, describe inputs needed by the module

  // Add a new input
  this->AddInputDescriptor<ComplexImageType>("InputImage","Complex image to extract intensity from");
  
  m_ModulusFilter = ModulusFilterType::New();
  m_SquareFilter  = SquareFilterType::New();
  m_LogFilter     = LogFilterType::New();
}

/** Destructor */
SarIntensityModule::~SarIntensityModule()
{}

/** PrintSelf method */
void SarIntensityModule::PrintSelf(std::ostream& os, itk::Indent indent) const
{
  // Call superclass implementation
  Superclass::PrintSelf(os,indent);
}


/** The custom run command */
void SarIntensityModule::Run()
{
  
  // First step is to retrieve the inputs
  ComplexImageType::Pointer image = this->GetInputData<ComplexImageType>("InputImage");
 
  // Check if dataset is not null
  if(image.IsNotNull())
    {
    // Wire filters
    m_ModulusFilter->SetInput(image);
    m_SquareFilter->SetInput(m_ModulusFilter->GetOutput());
    m_LogFilter->SetInput(m_SquareFilter->GetOutput());

    // First, clear any previous output
    this->ClearOutputDescriptors();
    
    // Add an output (single version)
    this->AddOutputDescriptor(m_SquareFilter->GetOutput(), "Intensity", "Intensity of the complex image.");
    this->AddOutputDescriptor(m_LogFilter->GetOutput(),"LogIntensity", "Log10-intensity of the complex image.");
    
    // Last, when all outputs where declared, notify listeners
    this->NotifyOutputsChange();
    }
  else
    {
    itkExceptionMacro(<<"Dataset is Null");
    }
}
} // End namespace otb

