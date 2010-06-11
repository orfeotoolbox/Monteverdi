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
#include "otbSupervisedClassificationModule2.h"

namespace otb
{
/** Constructor */
SupervisedClassificationModule2::SupervisedClassificationModule2()
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

  m_Model->RegisterListener(m_View);
  m_Model->RegisterListener(this);

  // Then, describe inputs needed by the module

  // Add a new input
  this->AddInputDescriptor<FloatingVectorImageType>("InputImage",otbGetTextMacro("Input image"));
  this->AddInputDescriptor<VectorDataType>("VectorDataROIs",otbGetTextMacro("Training/validation ROIs"));
}

/** Destructor */
SupervisedClassificationModule2::~SupervisedClassificationModule2()
{}

/** PrintSelf method */
void SupervisedClassificationModule2::PrintSelf(std::ostream& os, itk::Indent indent) const
{
  // Call superclass implementation
  Superclass::PrintSelf(os,indent);
}


/** The custom run command */
void SupervisedClassificationModule2::Run()
{
  // Untill window closing, module will be busy
  this->BusyOn();

  // Here is the body of the module.
  // When the Run() method is called, necessary inputs have been
  // passed to the module.

  // First step is to retrieve the inputs

  // To handle an input with multiple supported type :
  FloatingVectorImageType::Pointer fpvImage = this->GetInputData<FloatingVectorImageType>("InputImage");
  VectorDataType::Pointer vectorDataROIs = this->GetInputData<VectorDataType>("VectorDataROIs");

  // One of this pointer will be NULL:
  if(fpvImage.IsNotNull() && vectorDataROIs.IsNotNull())
    {
    // Process the input as an FloatingVectorImageType
      m_View->BuildInterface();
      m_Model->SetImage( fpvImage );
      m_Model->SetVectorROIs( vectorDataROIs );
    }
  else
    {
      itkExceptionMacro(<<"Input image is NULL.");
    }

  // Once all inputs have been properly retrieved, do what the module
  // should do : show a gui, start an MVC model, trigger processing...
}

/** The Notify */
void SupervisedClassificationModule2::Notify()
{

  if (m_Model->GetOutputChanged())
    {
      this->ClearOutputDescriptors();
      // Add outputs
      FloatingVectorImageType::Pointer filteredOutput = m_Model->GetOutput();
      this->AddOutputDescriptor(filteredOutput,"OutputImage", otbGetTextMacro("Input image with new keyword list"));
    }

  this->NotifyAll(MonteverdiEvent("OutputsUpdated",m_InstanceId));

  // Once module is closed, it is no longer busy
  this->BusyOff();
}
} // End namespace otb
