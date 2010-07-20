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
#include "otbFineCorrelationModule.h"

namespace otb
{
/** Constructor */
FineCorrelationModule::FineCorrelationModule()
{
  // First, do constructor stuffs

  // Then, describe inputs needed by the module

  // Add a new input
  this->AddInputDescriptor<ImageType>("ReferenceInputImage", otbGetTextMacro("Reference input image"));
  this->AddInputDescriptor<ImageType>("SecondaryInputImage", otbGetTextMacro("Secondary input image"));
  m_CorrelationFilter = CorrelationFilterType::New();
  m_ExtractXField = ExtractFilterType::New();
  m_ExtractXField->GetFunctor().SetIndex(0);
  m_ExtractYField = ExtractFilterType::New();
  m_ExtractYField->GetFunctor().SetIndex(1);

  // Build the GUI
  this->CreateGUI();

  gMode->add("Coarse");
  gMode->add("LsqrQuadFit");
  gMode->add("SubPixel");
  gMode->value(0);
  gPrecision->deactivate();
}

/** Destructor */
FineCorrelationModule::~FineCorrelationModule()
{}

/** PrintSelf method */
void FineCorrelationModule::PrintSelf(std::ostream& os, itk::Indent indent) const
{
  // Call superclass implementation
  Superclass::PrintSelf(os,indent);
}


/** The custom run command */
void FineCorrelationModule::Run()
{
  // First step is to retrieve the inputs

    ImageType::Pointer referenceImage = this->GetInputData<ImageType>("ReferenceInputImage",0);
    if( referenceImage.IsNull())
    {
      itkExceptionMacro(<<"The input reference image is Null.");
    }

    ImageType::Pointer secondaryImage = this->GetInputData<ImageType>("SecondaryInputImage",0);
    if( secondaryImage.IsNull())
      {
      itkExceptionMacro(<<"The input secondary image is Null.");
    }

    // Show the GUI
    this->Show();
    
    m_CorrelationFilter->SetFixedInput(referenceImage);
    m_CorrelationFilter->SetMovingInput(secondaryImage);
     
    m_ExtractXField->SetInput(m_CorrelationFilter->GetOutputDeformationField());
    m_ExtractYField->SetInput(m_CorrelationFilter->GetOutputDeformationField());
}


/** The custom run command */
void FineCorrelationModule::OK()
  {
    this->Hide();
    
    m_CorrelationFilter->SetRadius(gCorrelationRadius->value());
    m_CorrelationFilter->SetSearchRadius(gSearchRadius->value());

    if(gMode->value()==0)
      {
	// Coarse
      m_CorrelationFilter->SetRefinementModeToCoarse();
      }
    else if(gMode->value()==1)
      {
	// LSQR
      m_CorrelationFilter->SetRefinementModeToLSQRQuadFit();
      }
    else
      {
	// SUBPixel
      m_CorrelationFilter->SetRefinementModeToSubPixel();
      m_CorrelationFilter->SetSubPixelPrecision(gPrecision->value());
      }

    // First, clear any previous output
    this->ClearOutputDescriptors();
    // Add an output (single version)
    this->AddOutputDescriptor(m_CorrelationFilter->GetOutputDeformationField(), "DeformationField", otbGetTextMacro("Deformation field"));
    this->AddOutputDescriptor(m_CorrelationFilter->GetOutput(), "Correlation", otbGetTextMacro("Correlation value"));
    this->AddOutputDescriptor(m_ExtractXField->GetOutput(), "XDeformationField", otbGetTextMacro("X Deformation field"));
    this->AddOutputDescriptor(m_ExtractYField->GetOutput(), "YDeformationField", otbGetTextMacro("Y Deformation field"));
    // Last, when all outputs where declared, notify listeners
    this->NotifyAll(MonteverdiEvent("OutputsUpdated",m_InstanceId));

  };

} // End namespace otb
