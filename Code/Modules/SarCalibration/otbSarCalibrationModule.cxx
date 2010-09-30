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
#ifndef __otbSarCalibrationModule_cxx
#define __otbSarCalibrationModule_cxx

#include "otbSarCalibrationModule.h"

#include "otbMsgReporter.h"
#include "otbSarImageMetadataInterface.h"
#include "otbSarImageMetadataInterfaceFactory.h"


namespace otb
{

const double SarCalibrationModule::Epsilon = 1.0E-6;

/**
 * Constructor
 */
SarCalibrationModule
::SarCalibrationModule()
{
  m_ComplexCalibFilter = CalibrationComplexFilterType::New();
  m_CalibFilter = CalibrationFilterType::New();
  m_Log10ImageFilter = LogImageFilterType::New();
  m_MultiplyByConstantImageFilter = MultiplyByConstantImageFilterType::New();
  m_MultiplyByConstantImageFilter->SetConstant(10.0);

  m_AddConstantToImageFilter = AddConstantToImageFilterType::New();
  m_AddConstantToImageFilter->SetConstant(itkGetStaticConstMacro(Epsilon));

  // Describe inputs
  this->AddInputDescriptor<ImageType>("InputImage", otbGetTextMacro("Input image"));
  this->AddTypeToInputDescriptor<ComplexImageType>("InputImage");

  m_WorkWithCplx = false;
  // Build the GUI
  this->BuildGUI();
}

/**
 * Destructor
 */
SarCalibrationModule
::~SarCalibrationModule()
{}

/** The custom run command */
void
SarCalibrationModule
::Run()
{
  this->BusyOn();

  m_InputImage = this->GetInputData<ImageType>("InputImage");
  if (m_InputImage.IsNull())
    {
    m_ComplexInputImage = this->GetInputData<ComplexImageType>("InputImage");
    if (m_ComplexInputImage.IsNull())
      {
      this->BusyOff();
      itkExceptionMacro(<< "Input image is NULL");
      }
    else m_WorkWithCplx = true;
    }

  if (this->CheckMetadata())
    {
    // open the GUI
    this->Show();
    }
  else
    {
    this->BusyOff();
    }
}

bool
SarCalibrationModule
::CheckMetadata()
{
  SarImageMetadataInterface::Pointer lImageMetadata;

  if (!m_WorkWithCplx)
    {
    m_InputImage->UpdateOutputInformation();
    lImageMetadata = otb::SarImageMetadataInterfaceFactory::CreateIMI(m_InputImage->GetMetaDataDictionary());
    }
  else
    {
    m_ComplexInputImage->UpdateOutputInformation();
    lImageMetadata = otb::SarImageMetadataInterfaceFactory::CreateIMI(m_ComplexInputImage->GetMetaDataDictionary());
    }

  if (!lImageMetadata->CanRead())
    {
    MsgReporter::GetInstance()->SendError("Invalid Image: No Sar metadata Interface detected");
    return false;
    }

  return true;
}

void
SarCalibrationModule
::ComplexCalibrationProcess()
{
  m_ComplexCalibFilter->SetInput(m_ComplexInputImage);

  // Output selection (linear or dB scale)
  if (bLin->value() == 1 && bdB->value() == 0)
    {
    this->AddOutputDescriptor(m_ComplexCalibFilter->GetOutput(),
                              "CalibOutputImage",
                              otbGetTextMacro("Calibrated image"));

    }
  else if (bLin->value() == 0 && bdB->value() == 1)
    {
    m_AddConstantToImageFilter->SetInput(m_ComplexCalibFilter->GetOutput());
    m_Log10ImageFilter->SetInput(m_AddConstantToImageFilter->GetOutput());

    m_MultiplyByConstantImageFilter->SetInput(m_Log10ImageFilter->GetOutput());

    this->AddOutputDescriptor(m_MultiplyByConstantImageFilter->GetOutput(),
                              "CalibOutputImage dB",
                              otbGetTextMacro("Calibrated image dB"));
    }
  else
    {
    MsgReporter::GetInstance()->SendError("Invalid scale value ");
    return;
    }
}

void
SarCalibrationModule
::CalibrationProcess()
{
  m_CalibFilter->SetInput(m_InputImage);

  // Output selection (linear or dB scale)
  if (bLin->value() == 1 && bdB->value() == 0)
    {
    this->AddOutputDescriptor(m_CalibFilter->GetOutput(),
                              "CalibOutputImage",
                              otbGetTextMacro("Calibrated image"));

    }
  else if (bLin->value() == 0 && bdB->value() == 1)
    {


    m_AddConstantToImageFilter->SetInput(m_CalibFilter->GetOutput());
    m_Log10ImageFilter->SetInput(m_AddConstantToImageFilter->GetOutput());

    m_MultiplyByConstantImageFilter->SetInput(m_Log10ImageFilter->GetOutput());

    this->AddOutputDescriptor(m_MultiplyByConstantImageFilter->GetOutput(),
                              "CalibOutputImage dB",
                              otbGetTextMacro("Calibrated image dB"));
    }
  else
    {
    MsgReporter::GetInstance()->SendError("Invalid scale value ");
    return;
    }
}

/** Cancel */
void
SarCalibrationModule
::Cancel()
{
  this->Hide();
}

void
SarCalibrationModule
::OK()
{
  this->ClearOutputDescriptors();
  if (!m_WorkWithCplx)
    {
      this->CalibrationProcess();
    }
  else
    {
    this->ComplexCalibrationProcess();
    }

  this->NotifyOutputsChange();

  // close the GUI
  this->Hide();
  this->BusyOff();
}

} // End namespace otb

#endif
