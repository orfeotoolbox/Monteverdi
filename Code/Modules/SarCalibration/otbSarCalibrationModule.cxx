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

const double SarCalibrationModule::Epsilon = 1.0E-6;

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

  m_Log10ImageFilter = LogImageFilterType::New();
  m_MultiplyByConstantImageFilter = MultiplyByConstantImageFilterType::New();
  m_MultiplyByConstantImageFilter->SetConstant(10.0);

  m_AddConstantToImageFilter = AddConstantToImageFilterType::New();
  m_AddConstantToImageFilter->SetConstant(itkGetStaticConstMacro(Epsilon));

  // Instanciate an instance of thresholder
  m_ThresholdFilter    = ThresholdImageFilterType::New();

  // Describe inputs
  this->AddInputDescriptor<ImageType>("InputImage", otbGetTextMacro("Input image"));
  this->AddTypeToInputDescriptor<ComplexImageType>("InputImage");

  m_WorkWithCplx = false;
  // Build the GUI
  this->BuildGUI();
  bEnableNoise->value(1);
  vThresholdLogDisplay->value(-30);
  bThresholdOutput->value(1);

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
  if (bCalib->value() == 1 && bBrightness->value() == 0) // Radiometric Calibration
    {
    m_ComplexCalibFilter->SetInput(m_ComplexInputImage);
    if (bEnableNoise->value() == 0)
      {
      m_ComplexCalibFilter->SetEnableNoise(false);
      msgAboutNoise.assign(", without noise");
      }

    // Output selection (linear or dB scale)
    if (bLin->value() == 1 && bdB->value() == 0) // Linear
      {
      this->AddOutputDescriptor(m_ComplexCalibFilter->GetOutput(),
                                "Radiometric Calibration Image",
                                otbGetTextMacro(msgAboutNoise.insert(0,"Calibrated image").c_str()));
      }
    else if (bLin->value() == 0 && bdB->value() == 1) // dB
        {
        m_AddConstantToImageFilter->SetInput(m_ComplexCalibFilter->GetOutput());
        m_Log10ImageFilter->SetInput(m_AddConstantToImageFilter->GetOutput());

        m_MultiplyByConstantImageFilter->SetInput(m_Log10ImageFilter->GetOutput());

        if (bThresholdOutput->value() == 1) // With Threshold
          {
        m_ThresholdFilter->SetInput(m_MultiplyByConstantImageFilter->GetOutput());
        m_ThresholdFilter->SetOutsideValue(vThresholdLogDisplay->value());
        m_ThresholdFilter->ThresholdBelow(vThresholdLogDisplay->value());

        this->AddOutputDescriptor(m_ThresholdFilter->GetOutput(),
                                  "Radiometric Calibration in dB",
                                  otbGetTextMacro(msgAboutNoise.insert(0,"Thresholded Calibrated image dB").c_str()));
          }
        else // Without Threshold
          {
          this->AddOutputDescriptor(m_MultiplyByConstantImageFilter->GetOutput(),
                                            "Radiometric Calibration in dB",
                                            otbGetTextMacro(msgAboutNoise.insert(0,"Calibrated image dB").c_str()));
          }

        }
      else // Invalid case
        {
        MsgReporter::GetInstance()->SendError("Invalid scale value ");
        return;
        }
    }
  else // Brightness Calibration
    {
    m_ComplexBrightnessFilter->SetInput(m_ComplexInputImage);
    if (bEnableNoise->value() == 0)
      {
      m_ComplexBrightnessFilter->SetEnableNoise(false);
      msgAboutNoise.assign(", without noise");
      }

    // Output selection (linear or dB scale)
    if (bLin->value() == 1 && bdB->value() == 0) // Linear
      {
      this->AddOutputDescriptor(m_ComplexBrightnessFilter->GetOutput(),
                                "Brightness Image",
                                otbGetTextMacro(msgAboutNoise.insert(0,"Calibrated image").c_str()));

      }
    else if (bLin->value() == 0 && bdB->value() == 1) // dB
        {
        m_AddConstantToImageFilter->SetInput(m_ComplexBrightnessFilter->GetOutput());
        m_Log10ImageFilter->SetInput(m_AddConstantToImageFilter->GetOutput());

        m_MultiplyByConstantImageFilter->SetInput(m_Log10ImageFilter->GetOutput());

        if (bThresholdOutput->value() == 1) // With Threshold
          {
          m_ThresholdFilter->SetInput(m_MultiplyByConstantImageFilter->GetOutput());
          m_ThresholdFilter->SetOutsideValue(vThresholdLogDisplay->value());
          m_ThresholdFilter->ThresholdBelow(vThresholdLogDisplay->value());
          this->AddOutputDescriptor(m_ThresholdFilter->GetOutput(),
                                    "Brightness Image in dB",
                                    otbGetTextMacro(msgAboutNoise.insert(0,"Thresholded Calibrated image dB").c_str()));
          }
        else // Without Threshold
          {
        this->AddOutputDescriptor(m_MultiplyByConstantImageFilter->GetOutput(),
                                  "Brightness Image in dB",
                                  otbGetTextMacro(msgAboutNoise.insert(0,"Calibrated image dB").c_str()));
          }
        }
    else // Invalid case
        {
        MsgReporter::GetInstance()->SendError("Invalid scale value ");
        return;
        }
    }
}

void
SarCalibrationModule
::CalibrationProcess()
{
  std::string msgAboutNoise(", with noise");

  if (bCalib->value() == 1 && bBrightness->value() == 0) // Radiometric Calibration
    {
    m_CalibFilter->SetInput(m_InputImage);
    if (bEnableNoise->value() == 0)
      {
      m_CalibFilter->SetEnableNoise(false);
      msgAboutNoise.assign(", without noise");
      }

    // Output selection (linear or dB scale)
    if (bLin->value() == 1 && bdB->value() == 0) // Linear
      {
      this->AddOutputDescriptor(m_CalibFilter->GetOutput(),
                                "Radiometric Calibration Image",
                                otbGetTextMacro(msgAboutNoise.insert(0,"Calibrated image").c_str()));

      }
    else if (bLin->value() == 0 && bdB->value() == 1) // dB
      {
      m_AddConstantToImageFilter->SetInput(m_CalibFilter->GetOutput());
      m_Log10ImageFilter->SetInput(m_AddConstantToImageFilter->GetOutput());
      m_MultiplyByConstantImageFilter->SetInput(m_Log10ImageFilter->GetOutput());

      if (bThresholdOutput->value() == 1) // With Threshold
        {
        m_ThresholdFilter->SetInput(m_MultiplyByConstantImageFilter->GetOutput());
        m_ThresholdFilter->SetOutsideValue(vThresholdLogDisplay->value());
        m_ThresholdFilter->ThresholdBelow(vThresholdLogDisplay->value());
        this->AddOutputDescriptor(m_ThresholdFilter->GetOutput(),
                                  "Radiometric Calibration in dB",
                                  otbGetTextMacro(msgAboutNoise.insert(0,"Thresholded Calibrated image dB").c_str()));
        }
      else // Without Threshold
        {
        this->AddOutputDescriptor(m_MultiplyByConstantImageFilter->GetOutput(),
                                  "Radiometric Calibration in dB",
                                  otbGetTextMacro(msgAboutNoise.insert(0,"Calibrated image dB").c_str()));
        }
      }
    else // Invalid case
      {
      MsgReporter::GetInstance()->SendError("Invalid scale value ");
      return;
      }
    }
  else  // Brightness
    {
    m_BrightnessCalibFilter->SetInput(m_InputImage);
    if (bEnableNoise->value() == 0)
      {
      m_BrightnessCalibFilter->SetEnableNoise(false);
      msgAboutNoise.assign(", without noise");
      }

    // Output selection (linear or dB scale)
    if (bLin->value() == 1 && bdB->value() == 0) // Linear
      {
      this->AddOutputDescriptor(m_BrightnessCalibFilter->GetOutput(),
                                "Brightness Image",
                                otbGetTextMacro(msgAboutNoise.insert(0,"Calibrated image").c_str()));
      }
    else if (bLin->value() == 0 && bdB->value() == 1) // dB
      {
      m_AddConstantToImageFilter->SetInput(m_BrightnessCalibFilter->GetOutput());
      m_Log10ImageFilter->SetInput(m_AddConstantToImageFilter->GetOutput());
      m_MultiplyByConstantImageFilter->SetInput(m_Log10ImageFilter->GetOutput());

      if (bThresholdOutput->value() == 1) // With Threshold
        {
        m_ThresholdFilter->SetInput(m_MultiplyByConstantImageFilter->GetOutput());
        m_ThresholdFilter->SetOutsideValue(vThresholdLogDisplay->value());
        m_ThresholdFilter->ThresholdBelow(vThresholdLogDisplay->value());
        this->AddOutputDescriptor(m_ThresholdFilter->GetOutput(),
                                  "Brightness Image in dB",
                                  otbGetTextMacro(msgAboutNoise.insert(0,"Thresholded Calibrated image dB").c_str()));
        }
      else // Without Threshold
        {
        this->AddOutputDescriptor(m_MultiplyByConstantImageFilter->GetOutput(),
                                  "Brightness Image in dB",
                                  otbGetTextMacro(msgAboutNoise.insert(0,"Calibrated image dB").c_str()));
        }
      }
    else // Invalid case
      {
      MsgReporter::GetInstance()->SendError("Invalid scale value ");
      return;
      }
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
