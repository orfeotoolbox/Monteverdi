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
#include "otbMeanShiftModule.h"

namespace otb
{
/** Constructor */
MeanShiftModule::MeanShiftModule()
{
  // First, do constructor stuffs

  // Then, describe inputs needed by the module

  // Add a new input
  this->AddInputDescriptor<FloatingVectorImageType>("InputImage","Image to apply MeanShift on");
  
  // Add another supported type for this input
  this->AddTypeToInputDescriptor<FloatingImageType>("InputImage");

}

/** Destructor */
MeanShiftModule::~MeanShiftModule()
{}

/** PrintSelf method */
void MeanShiftModule::PrintSelf(std::ostream& os, itk::Indent indent) const
{
  // Call superclass implementation
  Superclass::PrintSelf(os,indent);
}


/** The custom run command */
void MeanShiftModule::Run()
{
  // Here is the body of the module. 
  // When the Run() method is called, necessary inputs have been
  // passed to the module.

  // First step is to retrieve the inputs
  
  // To handle an input with multiple supported type :
  FloatingVectorImageType::Pointer fpvImage = this->GetInputData<FloatingVectorImageType>("InputImage");
  FloatingImageType::Pointer fpImage = this->GetInputData<FloatingImageType>("InputImage");
  
  // One of this pointer will be NULL:
  if(fpvImage.IsNotNull())
    {
    // Process the input as an FloatingVectorImageType
    }
  else if(fpImage.IsNotNull())
    {
    // Process the input as an FloatingImageType
    }
  else
    {
    // Rerport error
    }
  
  // Once all inputs have been properly retrieved, do what the module
  // should do : show a gui, start an MVC model, trigger processing ...

  typedef otb::MeanShiftModuleController ControllerType;
  typedef otb::MeanShiftModuleView ViewType;
  otb::MsgReporter::GetInstance()->SetTitle("MeanShift");

  ControllerType::Pointer m_Controller = ControllerType::New();
  ViewType::Pointer m_View = ViewType::New();
  m_Controller->SetView(m_View);
  m_View->SetController(m_Controller);
  m_View->SetWidgetsController(m_Controller->GetWidgetsController());
  m_View->Build();


  // Then, when the module did actually produce some outputs, declare
  // them.

  // First, clear any previous output
  this->ClearOutputDescriptors();

  // Add an output (single version)
  FloatingImageType::Pointer myBrandNewImageOutput = FloatingImageType::New();
  this->AddOutputDescriptor(myBrandNewImageOutput,"MyImageOutput","This is my image output");

  // Add an output (multiple version)
  
  FloatingPointSetType::Pointer pointSetOutput1 = FloatingPointSetType::New();
  this->AddOutputDescriptor(pointSetOutput1,"MyPointSetOutput", "These are my pointset outputs");
  
  // Add addional data to the same output
  for(unsigned int i = 0; i<9;++i)
    {
    FloatingPointSetType::Pointer pointSetOutputN = FloatingPointSetType::New();
    this->AddDataToOutputDescriptor(pointSetOutputN,"MyPointSetOutput");
    }

  // Last, when all outputs where declared, notify listeners
  this->NotifyOutputsChange();
}
} // End namespace otb

