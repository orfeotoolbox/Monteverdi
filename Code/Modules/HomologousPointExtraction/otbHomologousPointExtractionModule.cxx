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
#include "otbHomologousPointExtractionModule.h"

namespace otb
{
/** Constructor */
HomologousPointExtractionModule::HomologousPointExtractionModule()
{
  // This module needs pipeline locking
  this->NeedsPipelineLockingOn();

  // First, do constructor stuffs

  m_Controller = ControllerType::New();
  m_View = ViewType::New();
  m_Model = ModelType::New();

  m_Controller->SetModel(m_Model);
  m_Controller->SetView(m_View);
  m_View->SetController(m_Controller);
  //m_View->SetModel(m_Model);
  m_View->SetFirstWidgetsController(m_Controller->GetFirstWidgetsController());
  m_View->SetSecondWidgetsController(m_Controller->GetSecondWidgetsController());

  m_Model->RegisterListener(this);

  // Then, describe inputs needed by the module

  // Add a new input
  this->AddInputDescriptor<FloatingVectorImageType>("FixInputImage", otbGetTextMacro("Fix image"));
  this->AddInputDescriptor<FloatingVectorImageType>("MovingInputImage", otbGetTextMacro("Moving Image"));
}

/** Destructor */
HomologousPointExtractionModule::~HomologousPointExtractionModule()
{}

/** PrintSelf method */
void HomologousPointExtractionModule::PrintSelf(std::ostream& os, itk::Indent indent) const
{
  // Call superclass implementation
  Superclass::PrintSelf(os, indent);
}

/** The custom run command */
void HomologousPointExtractionModule::Run()
{
  // Untill window closing, module will be busy
  this->BusyOn();

  // Here is the body of the module.
  // When the Run() method is called, necessary inputs have been
  // passed to the module.

  // First step is to retrieve the inputs

  // To handle an input with multiple supported type :
  FloatingVectorImageType::Pointer fpvImage1 = this->GetInputData<FloatingVectorImageType>("FixInputImage");
  FloatingVectorImageType::Pointer fpvImage2 = this->GetInputData<FloatingVectorImageType>("MovingInputImage");

  // One of this pointer will be NULL:
  if (fpvImage1.IsNotNull() && fpvImage2.IsNotNull())
    {
    // Process the input as an FloatingVectorImageType
    m_Model->SetFirstInputImage(fpvImage1);
    m_Model->SetSecondInputImage(fpvImage2);

    m_View->BuildInterface();
    }
  else
    {
    itkExceptionMacro(<< "Input image is NULL.");
    }

  // Once all inputs have been properly retrieved, do what the module
  // should do : show a gui, start an MVC model, trigger processing...

}

/** The Notify */
void HomologousPointExtractionModule::Notify()
{

  if (m_Model->GetOutputChanged())
    {
    this->ClearOutputDescriptors();
    // Add outputs
    FloatingVectorImageType::Pointer filteredOutput = m_Model->GetOutput();
    this->AddOutputDescriptor(filteredOutput, "Transformed Image", otbGetTextMacro("Transformed moving image"));
    }

  this->NotifyAll(MonteverdiEvent("OutputsUpdated", m_InstanceId));

  // Once module is closed, it is no longer busy
  this->BusyOff();
}
} // End namespace otb
