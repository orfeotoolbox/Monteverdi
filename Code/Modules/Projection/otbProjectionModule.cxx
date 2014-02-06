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
#ifndef __otbProjectionModule_cxx
#define __otbProjectionModule_cxx

#include "otbProjectionModule.h"

namespace otb
{
/** Constructor */
ProjectionModule::ProjectionModule()
{
  // Build mvc
  m_View       = ProjectionView::New();
  m_Controller = ProjectionController::New();
  m_Model      = ProjectionModel::New();
  m_Controller->SetModel(m_Model);
  m_Controller->SetView(m_View);
  m_View->SetController(m_Controller);
  m_Model->RegisterListener(this);
  m_Model->RegisterListener(m_View);

  // Instanciation of the Image To VectorImage Filter
  m_CastFilter = CastSingleImageFilter::New();

  // Describe inputs
  this->AddInputDescriptor<InputImageType>("InputImage", "Image to project");
  this->AddTypeToInputDescriptor<SingleImageType>("InputImage");
}

/** Destructor */
ProjectionModule::~ProjectionModule()
{}

/** PrintSelf method */
void ProjectionModule::PrintSelf(std::ostream& os, itk::Indent indent) const
{
  // Call superclass implementation
  Superclass::PrintSelf(os, indent);
}

/** The custom run command */
void ProjectionModule::Run()
{
  // While thev module is shown, it is busy
  this->BusyOn();

  InputImageType::Pointer inputImage = this->GetInputData<InputImageType>("InputImage");
  
  // Try to get a single image
  // If the input image is an otb::Image instead of VectorImage then cast it
  // in Vector Image and continue the processing
  SingleImageType::Pointer singleImage = this->GetInputData<SingleImageType>("InputImage");

  if (!singleImage.IsNull() && inputImage.IsNull())
    {
    m_CastFilter->SetInput(singleImage);
    m_CastFilter->UpdateOutputInformation();
    inputImage = m_CastFilter->GetOutput();
    }

  if (inputImage.IsNotNull())
    {
    inputImage->UpdateOutputInformation();
    m_Model->SetInputImage(inputImage);
    m_View->Show();
    }
  else
    {
    itkExceptionMacro(<< "InputImage is NULL");
    }
}

/** The Notify */
void ProjectionModule::Notify()
{
  if (m_Model->GetOutputChanged())
    {
    this->ClearOutputDescriptors();
    this->AddOutputDescriptor(m_Model->GetOutput(), "OutputImage", "Projected image");
    // Send an event to Monteverdi application
    //this->NotifyAll(MonteverdiEvent("OutputsUpdated", m_InstanceId));
    this->NotifyOutputsChange();
    this->BusyOff();
    }
}

} // End namespace otb

#endif
