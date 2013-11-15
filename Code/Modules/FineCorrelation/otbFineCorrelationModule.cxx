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
  m_CorrelationFilter    = CorrelationFilterType::New();
  m_FixedGaussianFilter  = GaussianFilterType::New();
  m_MovingGaussianFilter = GaussianFilterType::New();
  m_WarpFilter           = WarpFilterType::New();
  m_ExtractXField        = ExtractFilterType::New();
  m_ExtractYField        = ExtractFilterType::New();
  m_AbsImageFilter       = AbsImageFilterType::New();

  m_ExtractXField->GetFunctor().SetIndex(0);
  m_ExtractYField->GetFunctor().SetIndex(1);

  m_FixedGaussianFilter->SetUseImageSpacingOff();
  m_MovingGaussianFilter->SetUseImageSpacingOff();

  // Build the GUI
  this->CreateGUI();
}

/** Destructor */
FineCorrelationModule::~FineCorrelationModule()
{}

/** PrintSelf method */
void FineCorrelationModule::PrintSelf(std::ostream& os, itk::Indent indent) const
{
  // Call superclass implementation
  Superclass::PrintSelf(os, indent);
}


/** The custom run command */
void FineCorrelationModule::Run()
{
  // First step is to retrieve the inputs

    ImageType::Pointer referenceImage = this->GetInputData<ImageType>("ReferenceInputImage", 0);
    if( referenceImage.IsNull())
    {
      itkExceptionMacro(<<"The input reference image is Null.");
    }

    ImageType::Pointer secondaryImage = this->GetInputData<ImageType>("SecondaryInputImage", 0);
    if( secondaryImage.IsNull())
      {
      itkExceptionMacro(<<"The input secondary image is Null.");
    }

    // Show the GUI
    this->Show();
}


/** The custom run command */
void FineCorrelationModule::OK()
  {
    this->Hide();
    
    ImageType::Pointer referenceImage = this->GetInputData<ImageType>("ReferenceInputImage", 0);
    if( referenceImage.IsNull())
      {
      itkExceptionMacro(<<"The input reference image is Null.");
      }

    // Update reference image output information
    referenceImage->UpdateOutputInformation();

    ImageType::Pointer secondaryImage = this->GetInputData<ImageType>("SecondaryInputImage", 0);
    if( secondaryImage.IsNull())
      {
      itkExceptionMacro(<<"The input secondary image is Null.");
      }

    // Smooth fixed image if needed
    if(vSmoothFixedImage)
      {
      m_FixedGaussianFilter->SetInput(referenceImage);
      m_FixedGaussianFilter->SetVariance(vFixedSigma->value());
      m_CorrelationFilter->SetFixedInput(m_FixedGaussianFilter->GetOutput());
      }
    else
      {
      m_CorrelationFilter->SetFixedInput(referenceImage);
      }

    // Smooth moving image if needed
    if(vSmoothMovingImage)
      {
      m_MovingGaussianFilter->SetInput(secondaryImage);
      m_MovingGaussianFilter->SetVariance(vMovingSigma->value());
      m_CorrelationFilter->SetMovingInput(m_MovingGaussianFilter->GetOutput());
      }
    else
      {
      m_CorrelationFilter->SetMovingInput(secondaryImage);
      }

    // Set correlation parameters
    m_CorrelationFilter->SetRadius(vCorrelationRadius->value());
    m_CorrelationFilter->SetSearchRadius(vSearchRadius->value());
    m_CorrelationFilter->SetSubPixelAccuracy(vPrecision->value());

    CorrelationFilterType::OffsetType gridStep;
    gridStep.Fill(vGridStep->value());
    m_CorrelationFilter->SetGridStep(gridStep);

    CorrelationFilterType::SpacingType initialOffset;
    initialOffset[0] = vInitialOffsetX->value();
    initialOffset[1] = vInitialOffsetY->value();
    m_CorrelationFilter->SetInitialOffset(initialOffset);


    m_ExtractXField->SetInput(m_CorrelationFilter->GetOutputDisplacementField());
    m_ExtractYField->SetInput(m_CorrelationFilter->GetOutputDisplacementField());

    m_WarpFilter->SetInput(secondaryImage);
    m_WarpFilter->SetDisplacementField(m_CorrelationFilter->GetOutputDisplacementField());
    m_WarpFilter->SetOutputParametersFromImage(referenceImage);

    m_AbsImageFilter->SetInput(m_CorrelationFilter->GetOutput());

    // First, clear any previous output
    this->ClearOutputDescriptors();
    // Add an output (single version)
    //this->AddOutputDescriptor(m_CorrelationFilter->GetOutputDisplacementField(), "DisplacementField", otbGetTextMacro("Displacement field"));
    this->AddOutputDescriptor(m_AbsImageFilter->GetOutput(), "Correlation", otbGetTextMacro("Correlation value"));
    this->AddOutputDescriptor(m_ExtractXField->GetOutput(), "XDisplacementField", otbGetTextMacro("X Displacement field"));
    this->AddOutputDescriptor(m_ExtractYField->GetOutput(), "YDisplacementField", otbGetTextMacro("Y Displacement field"));

    // If needed, expose the warped moving image
    if(vWarpMovingImage)
      {
      this->AddOutputDescriptor(m_WarpFilter->GetOutput(), "RegisteredImage", otbGetTextMacro("Registered image"));
      }

    // Last, when all outputs where declared, notify listeners
    this->NotifyAll(MonteverdiEvent("OutputsUpdated", m_InstanceId));

  };

} // End namespace otb
