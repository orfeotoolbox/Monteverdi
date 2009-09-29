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
#ifndef __otbSpeckleFilteringModule_cxx
#define __otbSpeckleFilteringModule_cxx

#include "otbSpeckleFilteringModule.h"

namespace otb
{
/** Constructor */
SpeckleFilteringModule::SpeckleFilteringModule()
{
  // Build mvc
  m_View       = SpeckleFilteringView::New();
  m_Controller = SpeckleFilteringController::New();
  m_Model      = SpeckleFilteringModel::New();
  m_Controller->SetModel(m_Model);
  m_Controller->SetView(m_View);
  m_View->SetController(m_Controller);

  m_Model->RegisterListener(this);

  // Describe inputs
  this->AddInputDescriptor<InputImageType>("InputImage","Image to apply speckle filtering on.");
}

/** Destructor */
SpeckleFilteringModule::~SpeckleFilteringModule()
{}

/** PrintSelf method */
void SpeckleFilteringModule::PrintSelf(std::ostream& os, itk::Indent indent) const
{
  // Call superclass implementation
  Superclass::PrintSelf(os,indent);
}

/** The custom run command */
void SpeckleFilteringModule::Run()
{
  InputImageType::Pointer inputImage = this->GetInputData<InputImageType>("InputImage");

  if(inputImage.IsNotNull())
    {
    m_Model->SetInputImage(inputImage);
    m_View->Show();
    }
  else
    {
    itkExceptionMacro(<<"Input image is NULL");
    }
}

/** The Notify */
void SpeckleFilteringModule::Notify()
{
  if (m_Model->GetOutputChanged())
    {
    this->ClearOutputDescriptors();
    this->AddOutputDescriptor(m_Model->GetOutput(),"OutputImage","Speckle filtered image.");
    // Send an event to Monteverdi application
    this->NotifyAll(MonteverdiEvent("OutputsUpdated",m_InstanceId));
  }
}

} // End namespace otb

#endif
