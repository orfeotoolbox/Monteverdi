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
#include <string>

namespace otb
{

/**
 * Constructor
 */
SarCalibrationModule
::SarCalibrationModule()
{
  m_ComplexCalibFilter = CalibrationComplexFilterType::New();
  m_CalibFilter = CalibrationFilterType::New();

  m_ComplexBrightnessFilter = BrightnessComplexFilterType::New();
  m_BrightnessCalibFilter = BrightnessFilterType::New();

  m_Log10TImageFilterRC = Log10TImageFilterType::New();
  m_Log10TImageFilterB = Log10TImageFilterType::New();

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
    else
      {
      m_WorkWithCplx = true;
      }
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
  std::string msgAboutNoise(", with noise");
  std::string msgDescriptor;
  double valueT = pow(10.0, static_cast<double>(vThresholdLogDisplay->value())/10.0);

  m_ComplexCalibFilter->SetInput(m_ComplexInputImage);
  m_ComplexBrightnessFilter->SetInput(m_ComplexInputImage);
  if (bEnableNoise->value() == 0)
    {
    m_ComplexCalibFilter->SetEnableNoise(false);
    m_ComplexBrightnessFilter->SetEnableNoise(false);
    msgAboutNoise.assign(", without noise");
    }

  // Output selection (linear or dB scale)
  if (bLin->value() == 1 && bdB->value() == 0) // Linear
    {
    msgDescriptor = "Calibrated image" + msgAboutNoise;
    this->AddOutputDescriptor(m_ComplexCalibFilter->GetOutput(), "Radiometric Calibration Image",
                              otbGetTextMacro(msgDescriptor.c_str()));

    this->AddOutputDescriptor(m_ComplexBrightnessFilter->GetOutput(), "Brightness Image",
                              otbGetTextMacro(msgDescriptor.c_str()));
    }
  else if (bLin->value() == 0 && bdB->value() == 1) // dB
    {
    msgDescriptor = "Calibrated image dB" + msgAboutNoise;
    m_Log10TImageFilterRC->SetInput(m_ComplexCalibFilter->GetOutput());
    m_Log10TImageFilterRC->SetThresholdedValue(valueT);
    this->AddOutputDescriptor(m_Log10TImageFilterRC->GetOutput(), "Radiometric Calibration in dB",
                              otbGetTextMacro(msgDescriptor.c_str()));

    m_Log10TImageFilterB->SetInput(m_ComplexBrightnessFilter->GetOutput());
    m_Log10TImageFilterB->SetThresholdedValue(valueT);
    this->AddOutputDescriptor(m_Log10TImageFilterB->GetOutput(), "Brightness Image in dB",
                              otbGetTextMacro(msgDescriptor.c_str()));
    }
  else // Invalid case
    {
    MsgReporter::GetInstance()->SendError("Invalid scale value ");
    return;
    }
}

void
SarCalibrationModule
::CalibrationProcess()
{
  std::string msgAboutNoise(", with noise");
  std::string msgDescriptor;
  double valueT = pow(10.0, static_cast<double>(vThresholdLogDisplay->value())/10.0);

  m_CalibFilter->SetInput(m_InputImage);
  m_BrightnessCalibFilter->SetInput(m_InputImage);
  if (bEnableNoise->value() == 0)
    {
    m_CalibFilter->SetEnableNoise(false);
    m_BrightnessCalibFilter->SetEnableNoise(false);
    msgAboutNoise.assign(", without noise");
    }

    // Output selection (linear or dB scale)
  if (bLin->value() == 1 && bdB->value() == 0) // Linear
    {
    msgDescriptor = "Calibrated image" + msgAboutNoise;
    this->AddOutputDescriptor(m_CalibFilter->GetOutput(), "Radiometric Calibration Image",
                              otbGetTextMacro(msgDescriptor.c_str()));

    this->AddOutputDescriptor(m_BrightnessCalibFilter->GetOutput(), "Brightness Image",
                              otbGetTextMacro(msgDescriptor.c_str()));

    }
  else if (bLin->value() == 0 && bdB->value() == 1) // dB
    {
    msgDescriptor = "Calibrated image in dB" + msgAboutNoise;
    m_Log10TImageFilterRC->SetInput(m_CalibFilter->GetOutput());
    m_Log10TImageFilterRC->SetThresholdedValue(valueT);
    this->AddOutputDescriptor(m_Log10TImageFilterRC->GetOutput(), "Radiometric Calibration in dB",
                              otbGetTextMacro(msgDescriptor.c_str()));

    m_Log10TImageFilterB->SetInput(m_BrightnessCalibFilter->GetOutput());
    m_Log10TImageFilterB->SetThresholdedValue(valueT);
    this->AddOutputDescriptor(m_Log10TImageFilterB->GetOutput(), "Brightness Image in dB",
                              otbGetTextMacro(msgDescriptor.c_str()));
    }
  else // Invalid case
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
