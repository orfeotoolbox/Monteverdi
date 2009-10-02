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

#include "otbModule.h"
#include "otbImage.h"
#include "otbVectorData.h"

// This class is created only for tests purpose
class ModuleTest
  : public otb::Module
{
public:
  /** Standard class typedefs */
  typedef ModuleTest                    Self;
  typedef otb::Module                   Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** New macro */
  itkNewMacro(Self);

  /** Type macro */
  itkTypeMacro(ModuleTest,Module);

protected:
  /** Constructor */
  ModuleTest()
  {
    // Add some inputs
    this->AddInputDescriptor<otb::VectorImage<double,2> >("InputImage","test input image");
    this->AddTypeToInputDescriptor<otb::Image<double,2> >("InputImage");

    this->AddInputDescriptor<otb::VectorData<double> >("InputVector","test input vector",true);
  }
  /** Destructor */
  virtual ~ModuleTest(){}

  virtual void Run()
  {
    std::cout<<"Run method was called, inputs parameters are valid"<<std::endl;
    std::cout<<"Trying to retrieve input data ..."<<std::endl;
    
    otb::VectorImage<double,2>::Pointer firstPossibleInput = this->GetInputData<otb::VectorImage<double,2> >("InputImage");
    otb::Image<double,2>::Pointer secondPossibleInput = this->GetInputData<otb::Image<double,2> >("InputImage");
    otb::VectorData<double>::Pointer thirdPossibleInput = this->GetInputData<otb::VectorData<double> >("InputVector");

    std::cout<<"1st Input as VectorImage: "<<firstPossibleInput.IsNotNull()<<std::endl;
    std::cout<<"1st Input as Image: "<<secondPossibleInput.IsNotNull()<<std::endl;
    std::cout<<"2nd input as Vector: "<<thirdPossibleInput.IsNotNull()<<std::endl;

    // Clear previous descriptors
    this->ClearOutputDescriptors();
    
    // Add some outputs
    // Outputs can be defined once the run method has been called
    this->AddOutputDescriptor(otb::Image<unsigned short, 2>::New(),"OutputImage","test output image");
  }
};

int main(int argc, char * argv[])
{
  // Checking the Module base class construction
  otb::Module::Pointer myModule = otb::Module::New();
  std::cout<<"Base class PrintSelf: "<<myModule<<std::endl;

  // Checking protected methods using the ModuleTest implementation
  ModuleTest::Pointer myModuleTest = ModuleTest::New();
  std::cout<<"Test class PrintSelf: "<<myModuleTest<<std::endl;

  myModuleTest->SetInstanceId("MyModuleInstance");

  // Building inputs
  otb::DataObjectWrapper input1;
  input1.Set(otb::VectorImage<double,2>::New());

  otb::DataObjectWrapper input2;
  input2.Set(otb::VectorData<double>::New());


  // Testing the input/output method
  myModuleTest->AddInputByKey("InputImage",input1);
  myModuleTest->AddInputByKey("InputVector",input2);
  myModuleTest->Start();
  std::cout<<"Test class PrintSelf after run; "<<myModuleTest<<std::endl;
  otb::DataObjectWrapper output1 =  myModuleTest->GetOutputByKey("OutputImage");

  std::cout<<"Data received in main: "<<output1<<std::endl;

  // Testing Getters for data descriptor
  const ModuleTest::InputDataDescriptorMapType & inputs = myModuleTest->GetInputsMap();
  const ModuleTest::OutputDataDescriptorMapType & outputs = myModuleTest->GetOutputsMap();

  std::cout<<"Found "<<inputs.size()<<" inputs."<<std::endl;
  std::cout<<"Found "<<outputs.size()<<" outputs."<<std::endl;

  return EXIT_SUCCESS;
}
