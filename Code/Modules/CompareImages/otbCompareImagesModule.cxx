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
#ifndef __otbCompareImagesModule_cxx
#define __otbCompareImagesModule_cxx

#include "otbCompareImagesModule.h"

namespace otb
{
/** Constructor */
CompareImagesModule::CompareImagesModule()
{
  // First, do constructor stuffs

  // Build mvc
  m_View       = CompareImagesView::New();
  m_Controller = CompareImagesController::New();
  m_Model      = CompareImagesModel::New();
  
  m_Controller->SetModel(m_Model);
  m_Controller->SetView(m_View);
  
  m_View->SetController(m_Controller);
  m_View->InitVisu();
  
  m_Model->RegisterListener(this);

  // Then, describe inputs needed by the module

  // Add a new input
  this->AddInputDescriptor<InputImageType>("LeftInputImage", otbGetTextMacro("First image to compare"));
  this->AddInputDescriptor<InputImageType>("RightInputImage", otbGetTextMacro("Second image to compare"));

}

/** Destructor */
CompareImagesModule::~CompareImagesModule()
{}

/** PrintSelf method */
void CompareImagesModule::PrintSelf(std::ostream& os, itk::Indent indent) const
{
  // Call superclass implementation
  Superclass::PrintSelf(os,indent);
}

/** The custom run command */
void CompareImagesModule::Run()
{
  // First step is to retrieve the inputs

  // To handle an input with multiple supported type :
  InputImageType::Pointer inputLeftImage = this->GetInputData<InputImageType>("LeftInputImage");
  InputImageType::Pointer inputRightImage = this->GetInputData<InputImageType>("RightInputImage");
  
  inputRightImage->UpdateOutputInformation();
  inputLeftImage->UpdateOutputInformation();

  // One of this pointer will be NULL:
  if (inputLeftImage.IsNotNull() and inputRightImage.IsNotNull())
    {
    // Process the input as an FloatingImageType
    m_View->Show();
    m_Model->SetInputLeftImage(inputLeftImage);
    m_Model->SetInputRightImage(inputRightImage);
    }
  else
    {
    itkExceptionMacro(<< "Input image is NULL.");
    }

}



/** The notify */
void CompareImagesModule::Notify(const std::string& event)
{
  if (event == "SaveQuit")
    {
    this->ClearOutputDescriptors();
    //this->AddOutputDescriptor(m_Model->GetOutput(), "OutputImage", otbGetTextMacro("Speckle filtered image"));
    // Send an event to Monteverdi application
    this->NotifyAll(MonteverdiEvent("OutputsUpdated", m_InstanceId));
    }
}


} // End namespace otb

#endif
