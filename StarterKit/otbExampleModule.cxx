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
#include "otbExampleModule.h"

namespace otb
{
/** Constructor */
ExampleModule::ExampleModule()
{
  // First, do constructor stuffs

  // Then, describe inputs needed by the module

  // Add a new input
  this->AddInputDescriptor<FPVImageType>("MyInputImage","This is my input");
  
  // Add another supported type for this input
  this->AddTypeToInputDescriptor<FPImageType>("MyInputImage");

  // Add an optional input
  this->AddInputDescriptor<VectorType>("MyOptionalInput","This is my optional input",true);

  // add a multiple input (multiple can also be optional)
  this->AddInputDescriptor<VectorType>("MyMultipleInput","This is my multiple input",false,true);
}

/** Destructor */
ExampleModule::~ExampleModule()
{}

/** PrintSelf method */
void ExampleModule::PrintSelf(std::ostream& os, itk::Indent indent) const
{
  // Call superclass implementation
  Superclass::PrintSelf(os,indent);
}


/** The custom run command */
void ExampleModule::Run()
{
  // Here is the body of the module. 
  // When the Run() method is called, necessary inputs have been
  // passed to the module.

  // First step is to retrieve the inputs
  
  // To handle an input with multiple supported type :
  FPVImageType::Pointer fpvImage = this->GetInputData<FPVImageType>("MyInputImage");
  FPImageType::Pointer fpImage = this->GetInputData<FPImageType>("MyInputImage");
  
  // One of this pointer will be NULL:
  if(fpvImage.IsNotNull())
    {
    // Process the input as an FPVImageType
    }
  else if(fpImage.IsNotNull())
    {
    // Process the input as an FPImageType
    }
  else
    {
    // Rerport error
    }

  // To handle an optional input: 

  if(this->GetNumberOfInputDataByKey("MyOptionalInput") > 0)
    {
    VectorType::Pointer optionalVector = this->GetInputData<VectorType>("MyOptionalInput");

    // Again, check for pointer validity
    if(optionalVector.IsNotNull())
      {
      // Process optional input
      }
    else
      {
      // Report error
      }
    }
  
  // To handle a multiple input:

  for(unsigned int inputIdx = 0; inputIdx < this->GetNumberOfInputDataByKey("MyMultipleInput");++inputIdx)
    {
    VectorType::Pointer nthMultipleVector = this->GetInputData<VectorType>("MyMultipleInput",inputIdx);
    
    if(nthMultipleVector.IsNotNull())
      {
      // Process the nth mutliple input
      }
    else
      {
      // Report error
      }
    }
  
  // Once all inputs have been properly retrieved, do what the module
  // should do : show a gui, start an MVC model, trigger processing ...
  

  // Then, when the module did actually produce some outputs, declare
  // them.

  // First, clear any previous output
  this->ClearOutputDescriptors();

  // Add an output (single version)
  FPImageType::Pointer myBrandNewImageOutput = FPImageType::New();
  this->AddOutputDescriptor(myBrandNewImageOutput,"MyImageOutput","This is my image output");

  // Add an output (multiple version)
  
  FPPointSetType::Pointer pointSetOutput1 = FPPointSetType::New();
  this->AddOutputDescriptor(pointSetOutput1,"MyPointSetOutput", "These are my pointset outputs");
  
  // Add addional data to the same output
  for(unsigned int i = 0; i<9;++i)
    {
    FPPointSetType::Pointer pointSetOutputN = FPPointSetType::New();
    this->AddDataToOutputDescriptor(pointSetOutputN,"MyPointSetOutput");
    }

  // Last, when all outputs where declared, notify listeners
  this->NotifyOutputsChange();
}
} // End namespace otb
