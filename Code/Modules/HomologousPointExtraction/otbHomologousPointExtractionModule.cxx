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
  // First, do constructor stuffs

  m_Controller = ControllerType::New();
  m_View = ViewType::New();
  m_Model = ModelType::New();

  m_Controller->SetModel(m_Model);
  m_Controller->SetView(m_View);
  m_View->SetController(m_Controller);
  m_View->SetModel(m_Model);
  m_View->SetWidgetsController(m_Controller->GetWidgetsController());

  m_Model->RegisterListener(this);

  // Then, describe inputs needed by the module

  // Add a new input
  this->AddInputDescriptor<FloatingVectorImageType>("FirstInputImage","Image to apply HomologousPointExtraction on");
  this->AddInputDescriptor<FloatingVectorImageType>("SecondInputImage","Image to apply HomologousPointExtraction on");  
}

/** Destructor */
HomologousPointExtractionModule::~HomologousPointExtractionModule()
{}

/** PrintSelf method */
void HomologousPointExtractionModule::PrintSelf(std::ostream& os, itk::Indent indent) const
{
  // Call superclass implementation
  Superclass::PrintSelf(os,indent);
}


/** The custom run command */
void HomologousPointExtractionModule::Run()
{
  // Here is the body of the module. 
  // When the Run() method is called, necessary inputs have been
  // passed to the module.

  // First step is to retrieve the inputs
  
  // To handle an input with multiple supported type :
  FloatingVectorImageType::Pointer fpvImage1 = this->GetInputData<FloatingVectorImageType>("FirstInputImage");
  FloatingVectorImageType::Pointer fpvImage2 = this->GetInputData<FloatingVectorImageType>("SecondInputImage");
  
  // One of this pointer will be NULL:
  if(fpvImage1.IsNotNull() && fpvImage2.IsNotNull() )
    {
    // Process the input as an FloatingVectorImageType
      m_Model->SetFirstInputImage( fpvImage1 );
      m_Model->SetSecondInputImage( fpvImage2 );

      m_View->BuildInterface();
    }
  else
    {
      itkExceptionMacro(<<"Input image is NULL.");
    }
  
  // Once all inputs have been properly retrieved, do what the module
  // should do : show a gui, start an MVC model, trigger processing ...

}

/** The Notify */
void HomologousPointExtractionModule::Notify()
{

  if (m_Model->GetOutputChanged())
    {
      this->ClearOutputDescriptors();
      
      // Add outputs

      //FloatingVectorImageType::Pointer filteredOutput = m_Model->GetOutput();
      //this->AddOutputDescriptor(filteredOutput,"Filtered Image", "Result of the HomologousPointExtraction filtering");
    }

  this->NotifyAll(MonteverdiEvent("OutputsUpdated",m_InstanceId));
}
} // End namespace otb

