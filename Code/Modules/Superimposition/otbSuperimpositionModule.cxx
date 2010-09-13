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
#ifndef __otbSuperimpositionModule_cxx
#define __otbSuperimpositionModule_cxx

#include "otbSuperimpositionModule.h"
#include <FLU/Flu_File_Chooser.h>
#include "base/ossimFilename.h"
#include "otbMsgReporter.h"

namespace otb
{
/** Constructor */
SuperimpositionModule::SuperimpositionModule()
{
  m_Resampler       = ResampleFilterType::New();
  m_PerBanderFilter = PerBandFilterType::New();
  m_Transform       = TransformType::New();

  // Describe inputs
  this->AddInputDescriptor<VectorImageType>("ReferenceImage", otbGetTextMacro("Reference image for reprojection"));
  this->AddTypeToInputDescriptor<ImageType>("ReferenceImage");
  this->AddInputDescriptor<VectorImageType>("InputImage", otbGetTextMacro("Image to reproject"));
  this->AddTypeToInputDescriptor<ImageType>("InputImage");

  this->BuildGUI();
}

/** Destructor */
SuperimpositionModule::~SuperimpositionModule()
{
  wFileChooserWindow->hide();
}

/** PrintSelf method */
void SuperimpositionModule::PrintSelf(std::ostream& os, itk::Indent indent) const
{
  // Call superclass implementation
  Superclass::PrintSelf(os, indent);
}

/** The custom run command */
void SuperimpositionModule::Run()
{
  wFileChooserWindow->show();
}

void SuperimpositionModule::Ok()
{
  VectorImageType::Pointer vfixed = this->GetInputData<VectorImageType>("ReferenceImage");
  VectorImageType::Pointer vmoving = this->GetInputData<VectorImageType>("InputImage");

  ImageType::Pointer fixed = this->GetInputData<ImageType>("ReferenceImage");
  ImageType::Pointer moving = this->GetInputData<ImageType>("InputImage");

  // Handle reference image
  if (fixed.IsNotNull())
    {
    fixed->UpdateOutputInformation();
    m_Transform->SetInputProjectionRef(fixed->GetProjectionRef());
    m_Transform->SetInputDictionary(fixed->GetMetaDataDictionary());
    m_Transform->SetInputKeywordList(fixed->GetImageKeywordlist());
    m_Resampler->SetOutputSize(fixed->GetLargestPossibleRegion().GetSize());
    m_Resampler->SetOutputStartIndex(fixed->GetLargestPossibleRegion().GetIndex());
    m_Resampler->SetOutputSpacing(fixed->GetSpacing());
    m_Resampler->SetOutputOrigin(fixed->GetOrigin());
    }
  else if (vfixed.IsNotNull())
    {
    vfixed->UpdateOutputInformation();
    m_Transform->SetInputProjectionRef(vfixed->GetProjectionRef());
    m_Transform->SetInputDictionary(vfixed->GetMetaDataDictionary());
    m_Transform->SetInputKeywordList(vfixed->GetImageKeywordlist());
    m_Resampler->SetOutputSize(vfixed->GetLargestPossibleRegion().GetSize());
    m_Resampler->SetOutputStartIndex(vfixed->GetLargestPossibleRegion().GetIndex());
    m_Resampler->SetOutputSpacing(vfixed->GetSpacing());
    m_Resampler->SetOutputOrigin(vfixed->GetOrigin());
    }
  else
    {
    itkExceptionMacro(<< "Fixed input is null");
    }

  // Handle moving inputs
  if (moving.IsNotNull())
    {
    moving->UpdateOutputInformation();
    m_Transform->SetOutputProjectionRef(moving->GetProjectionRef());
    m_Transform->SetOutputDictionary(moving->GetMetaDataDictionary());
    m_Transform->SetOutputKeywordList(moving->GetImageKeywordlist());
    }
  else if (vmoving.IsNotNull())
    {
    vmoving->UpdateOutputInformation();
    m_Transform->SetOutputProjectionRef(vmoving->GetProjectionRef());
    m_Transform->SetOutputDictionary(vmoving->GetMetaDataDictionary());
    m_Transform->SetOutputKeywordList(vmoving->GetImageKeywordlist());
    }
  else
    {
    itkExceptionMacro(<< "Moving input is null");
    }

  if (choiceDEM->value() == 1)
    {
    m_Transform->SetDEMDirectory(vDEMPath->value());
    }
  else
    {
    m_Transform->SetAverageElevation(vAverageElevation->value());
    }

  m_Transform->InstanciateTransform();

  // Copy parameters from reference image
  m_Resampler->SetTransform(m_Transform);

  // Do we have to resample a vector image ?
  if (vmoving.IsNotNull())
    {
    m_PerBanderFilter->SetInput(vmoving);
    m_PerBanderFilter->SetFilter(m_Resampler);
    VectorImageType::Pointer output = m_PerBanderFilter->GetOutput();
    output->UpdateOutputInformation();

    // Report projection ref (not done by the resample filter)
    if (fixed.IsNotNull())
      {
      output->CopyInformation(fixed);
      }
    else if (vfixed.IsNotNull())
      {
      output->CopyInformation(vfixed);
      }

    output->SetNumberOfComponentsPerPixel(vmoving->GetNumberOfComponentsPerPixel());

    this->ClearOutputDescriptors();
    this->AddOutputDescriptor(output, "Reprojected image", otbGetTextMacro("Image superimposable to reference"));
    this->NotifyOutputsChange();
    }
  // Else produce only a single image
  else if (moving.IsNotNull())
    {
    m_Resampler->SetInput(moving);
    ImageType::Pointer output = m_Resampler->GetOutput();
    output->UpdateOutputInformation();

    // Report projection ref (not done by the resample filter)
    if (fixed.IsNotNull())
      {
      output->CopyInformation(fixed);
      }
    else if (vfixed.IsNotNull())
      {
      output->CopyInformation(vfixed);
      }

    output->SetNumberOfComponentsPerPixel(moving->GetNumberOfComponentsPerPixel());

    this->ClearOutputDescriptors();
    this->AddOutputDescriptor(output, "Reprojected image", otbGetTextMacro("Image superimposable to reference"));
    this->NotifyOutputsChange();
    }

  wFileChooserWindow->hide();
}

void SuperimpositionModule::Browse()
{
  const char * filename = NULL;

  filename = flu_dir_chooser(otbGetTextMacro("Choose the dataset dir..."), "*.*", "");

  if (filename == NULL)
    {
    otbMsgDebugMacro(<< "Empty file name!");
    return;
    }
  vDEMPath->value(filename);
}

void SuperimpositionModule::Cancel()
{
  wFileChooserWindow->hide();
}

} // End namespace otb

#endif
