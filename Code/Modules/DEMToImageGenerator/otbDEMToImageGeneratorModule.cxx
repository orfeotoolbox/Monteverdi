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
#ifndef __otbDEMToImageGeneratorModule_cxx
#define __otbDEMToImageGeneratorModule_cxx

#include "otbDEMToImageGeneratorModule.h"

namespace otb
{
/** Constructor */
DEMToImageGeneratorModule::DEMToImageGeneratorModule()
{
  // Build mvc
  m_View       = DEMToImageGeneratorView::New();
  m_Controller = DEMToImageGeneratorController::New();
  m_Model      = DEMToImageGeneratorModel::New();
  m_Controller->SetModel(m_Model);
  m_Controller->SetView(m_View);
  m_View->SetController(m_Controller);
  m_Model->RegisterListener(this);
  m_Model->RegisterListener(m_View);

  // Describe inputs
  this->AddInputDescriptor<InputImageType>("InputImage","DEM image ",true,false);
}

/** Destructor */
DEMToImageGeneratorModule::~DEMToImageGeneratorModule()
{}

/** PrintSelf method */
void DEMToImageGeneratorModule::PrintSelf(std::ostream& os, itk::Indent indent) const
{
  // Call superclass implementation
  Superclass::PrintSelf(os, indent);
}

/** The custom run command */
void DEMToImageGeneratorModule::Run()
{
  InputImageType::Pointer inputImage = this->GetInputData<InputImageType>("InputImage");

  if (inputImage.IsNotNull())
    {
    inputImage->UpdateOutputInformation();
    std::cout << "DEMToImageGeneratorModule::Run()"
        << inputImage->GetProjectionRef()
        << std::endl;

    m_Model->SetInputImage(inputImage);
    m_Model->UpdateOutputParameters();
    m_Model->SetUseInputImage(true);
    m_View->Show();
    /* hide a part of a menu */
    }
  else
    {
    //itkExceptionMacro(<< "Input image is NULL");
    /*show InputMenu()*/
    m_Model->SetUseInputImage(false);
    m_View->Show();
    }
}

/** The Notify */
void DEMToImageGeneratorModule::Notify()
{
  if (m_Model->GetOutputChanged())
    {
    this->ClearOutputDescriptors();
    this->AddOutputDescriptor(m_Model->GetOutput(), "DEMImage", otbGetTextMacro("DEM image"));
    // Send an event to Monteverdi application
    //this->NotifyAll(MonteverdiEvent("OutputsUpdated",m_InstanceId));
    this->NotifyOutputsChange();
    }
}

} // End namespace otb

#endif
