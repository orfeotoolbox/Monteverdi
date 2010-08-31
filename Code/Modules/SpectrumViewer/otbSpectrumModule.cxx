/*=========================================================================

  Program:   ORFEO Toolbox
  Language:  C++
  Date:      $Date$
  Version:   $Revision$


  Copyright (c) Centre National d'Etudes Spatiales. All rights reserved.
  See OTBCopyright.txt for details.

  Copyright (c) CS Systemes d'information. All rights reserved.
  See CSCopyright.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "otbSpectrumModule.h"

namespace otb {

SpectrumModule
::SpectrumModule()
{

  this->NeedsPipelineLockingOn();

  /** Members construction (MVC) */
  m_SpectrumModuleController = SpectrumModuleControllerType::New();
  m_SpectrumModuleModel = SpectrumModuleModelType::New();
  m_SpectrumModuleView = SpectrumModuleViewType::New();

  /** Members associations (MVC) */
  m_SpectrumModuleController->SetSpectrumModuleModel(m_SpectrumModuleModel);
  m_SpectrumModuleController->SetSpectrumModuleView(m_SpectrumModuleView);
  m_SpectrumModuleView->SetSpectrumModuleModel(m_SpectrumModuleModel);
  m_SpectrumModuleView->SetSpectrumModuleController(m_SpectrumModuleController);

  m_SpectrumModuleModel->RegisterListener(this);

  this->AddInputDescriptor<ImageType>("InputImage", otbGetTextMacro("Image for spectrum display."));

}

SpectrumModule
::~SpectrumModule()
{
}

void
SpectrumModule
::PrintSelf(std::ostream& os, itk::Indent indent) const
{
  // Call superclass implementation
  Superclass::PrintSelf(os, indent);
}

void
SpectrumModule
::Notify(const std::string& event)
{

  if (event == "OutputsUpdated")
    {
    this->ClearOutputDescriptors();

    // Add outputs
    if (m_SpectrumModuleModel->GetspectralImage())
      {
      ImageType::Pointer filteredOutput = m_SpectrumModuleModel->GetspectralImage();
      this->AddOutputDescriptor(filteredOutput, "Filtered Image",
                                otbGetTextMacro("Result of the spectral angle filtering"));
      }

    // Send an event to Monteverdi application
    this->NotifyAll(MonteverdiEvent("OutputsUpdated", m_InstanceId));

    // Once module is closed, it is no longer busy
    this->BusyOff();
    }
  else
    {
    if (event == "BusyOff")
      {
      this->BusyOff();
      }
    }
}

void SpectrumModule
::Run()
{
  // Untill window closing, module will be busy
  this->BusyOn();
  // Here is the body of the module.
  // When the Run() method is called, necessary inputs have been
  // passed to the module.
  // First step is to retrieve the inputs
  // To handle an input with multiple supported type :
  ImageType::Pointer fpvImage = this->GetInputData<ImageType>("InputImage");

  // One of this pointer will be NULL:
  if (fpvImage.IsNotNull())
    {
    // Process the input as an FloatingVectorImageType
    m_SpectrumModuleModel->SetInputImage(fpvImage);

    if (fpvImage->GetNumberOfComponentsPerPixel() <= 1) m_SpectrumModuleView->TriggerMonoChannelException();

    m_SpectrumModuleView->Build();

    }
  else
    {
    itkExceptionMacro(<< "Input image is NULL.");
    }
}

}
