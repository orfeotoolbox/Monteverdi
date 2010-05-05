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
#include "otbObjectCountingModule.h"

namespace otb
{
/** Constructor */
ObjectCountingModule::ObjectCountingModule()
{
  // This module needs pipeline locking
  this->NeedsPipelineLockingOn();

  // First, do constructor stuffs

  m_Controller = ControllerType::New();
  m_View = ViewType::New();
  
  m_Controller->SetView(m_View);
  m_View->SetController(m_Controller);

  m_Model = ObjectCountingModel::GetInstance();
  // Then, describe inputs needed by the module

  // Add a new input
  this->AddInputDescriptor<FloatingVectorImageType>("InputImage", otbGetTextMacro("Image to apply ObjectCounting on"));

}

/** Destructor */
ObjectCountingModule::~ObjectCountingModule()
{}

/** PrintSelf method */
void ObjectCountingModule::PrintSelf(std::ostream& os, itk::Indent indent) const
{
  // Call superclass implementation
  Superclass::PrintSelf(os,indent);
}


/** The custom run command */
void ObjectCountingModule::Run()
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
  if(fpvImage.IsNotNull())
    {
    // Process the input as an FloatingVectorImageType
      m_Model->SetInputImage( fpvImage );
      std::cout << "model setinput image ok" << std::endl;
      m_View->Build();
      std::cout << "Build view over" << std::endl;
    }
  else
    {
      itkExceptionMacro(<<"Input image is NULL.");
    }

  // Once all inputs have been properly retrieved, do what the module
  // should do : show a gui, start an MVC model, trigger processing...

}

/** The Notify */
void ObjectCountingModule::Notify(const std::string & event)
{

  if (event == "OutputsUpdated")
  {
    this->ClearOutputDescriptors();

    // Add outputs
    LabeledImageType::Pointer labeledOutput = m_Model->GetOutputLabeledImage();
    this->AddOutputDescriptor(labeledOutput,"Labeled Image", otbGetTextMacro("Result of the ObjectCounting labeling"));
    
    // Send an event to Monteverdi application
    this->NotifyAll(MonteverdiEvent("OutputsUpdated",m_InstanceId));

      // Once module is closed, it is no longer busy
    this->BusyOff();
  }
  else if (event == "BusyOff")
  {
    this->BusyOff();
  }
  else
  {
  }

}
} // End namespace otb

