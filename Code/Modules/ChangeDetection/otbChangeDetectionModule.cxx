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
#ifndef __otbChangeDetectionModule_cxx
#define __otbChangeDetectionModule_cxx

#include "otbChangeDetectionModule.h"

namespace otb
{
/** Constructor */
ChangeDetectionModule::ChangeDetectionModule()
{
  m_ChangeDetection = ChangeDetectionType::New();
  m_ChangeDetection->RegisterListener(this);

  // Instanciation of the Image To VectorImage Filter
  m_CastFilter = CastSingleImageFilter::New();

  // Describe inputs
  this->AddInputDescriptor<ImageType>("LeftImage", otbGetTextMacro("Left image"));
  this->AddInputDescriptor<ImageType>("RightImage", otbGetTextMacro("Right image"));
  this->AddTypeToInputDescriptor<SingleImageType>("LeftImage");
  this->AddTypeToInputDescriptor<SingleImageType>("RightImage");
}

/** Destructor */
ChangeDetectionModule::~ChangeDetectionModule()
{}

/** PrintSelf method */
void ChangeDetectionModule::PrintSelf(std::ostream& os, itk::Indent indent) const
{
  // Call superclass implementation
  Superclass::PrintSelf(os, indent);
}

/** The custom run command */
void ChangeDetectionModule::Run()
{
  // Build the GUI
  m_ChangeDetection->Build();

  ImageType::Pointer linput = this->GetInputData<ImageType>("LeftImage");
  ImageType::Pointer rinput = this->GetInputData<ImageType>("RightImage");

  // Try to get a single image
  // If the input image is an otb::Image instead of VectorImage then cast it
  // in Vector Image and continue the processing
  SingleImageType::Pointer lsingleImage = this->GetInputData<SingleImageType>("LeftImage");
  SingleImageType::Pointer rsingleImage = this->GetInputData<SingleImageType>("RightImage");

  if (!lsingleImage.IsNull() && linput.IsNull())
    {
    m_CastFilter->SetInput(lsingleImage);
    m_CastFilter->UpdateOutputInformation();
    linput = m_CastFilter->GetOutput();
    }
  if (!rsingleImage.IsNull() && rinput.IsNull())
    {
    m_CastFilter->SetInput(rsingleImage);
    m_CastFilter->UpdateOutputInformation();
    rinput = m_CastFilter->GetOutput();
    }

  if (rinput.IsNotNull() &&  linput.IsNotNull())
    {
    m_ChangeDetection->SetRightImage(rinput);
    m_ChangeDetection->SetLeftImage(linput);
    m_ChangeDetection->SetFiltersInputs();
    m_ChangeDetection->LoadLeftImage();
    m_ChangeDetection->LoadRightImage();
    m_ChangeDetection->LoadCenterImage();
    m_ChangeDetection->Show();
    }
  else
    {
    itkExceptionMacro(<< "InputImage are NULL");
    }
}

/** PrintSelf method */
void ChangeDetectionModule::Hide()
{
  m_ChangeDetection->Hide();
}

/** The notify */
void ChangeDetectionModule::Notify()
{
  if (m_ChangeDetection->GetHasOutput())
    {
    this->ClearOutputDescriptors();
    this->AddOutputDescriptor(m_ChangeDetection->GetOutput(), "OutputImage", otbGetTextMacro("Change image Label"));
    this->NotifyOutputsChange();
    }
}

} // End namespace otb

#endif
