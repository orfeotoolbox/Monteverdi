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
    this->AddDataDescriptor("Floating_Point_Image","InputImage","test input image");
    this->AddDataDescriptor("Vector","InputVector","test input vector",true);
  }
  /** Destructor */
  virtual ~ModuleTest(){}

  // Reimplement AssignDataByKey
  virtual void AssignDataByKey(const std::string & key, const otb::DataObjectWrapper & data)
  {
    if(key == "InputImage" || key == "InputVector")
      {
      std::cout<<"Received data "<<data<<" for input with key "<<key<<std::endl;
      }
  }

  // Reimplement RetrieveDataByKey
  const otb::DataObjectWrapper RetrieveDataByKey(const std::string & key) const
  {
    otb::DataObjectWrapper wrapper;

    if(key == "OutputImage")
      {
      wrapper.Set("Labeled_Image",otb::Image<unsigned short,2>::New());

      std::cout<<"Sending data "<<wrapper<<" for output with key "<<key<<std::endl;
      }

    return wrapper;
  }

  virtual void Run()
  {
    std::cout<<"Run method was called, inputs parameters are valid"<<std::endl;

    // Add some outputs
    // Outputs can be defined once the run method has been called
    this->AddDataDescriptor("Labeled_Image","OutputImage","test output image");

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

  // Building inputs
  otb::DataObjectWrapper input1("Floating_Point_Image",otb::Image<double,2>::New());
  otb::DataObjectWrapper input2("Vector",otb::VectorData<double>::New());


  // Testing the input/output method
  myModuleTest->AddDataByKey("InputImage",input1);
  myModuleTest->AddDataByKey("InputVector",input2);
  myModuleTest->Start();
  std::cout<<"Test class PrintSelf after run; "<<myModuleTest<<std::endl;
  otb::DataObjectWrapper output1 =  myModuleTest->GetDataByKey("OutputImage");

  std::cout<<"Data received in main: "<<output1<<std::endl;

  // Testing Getters for data descriptor
  const ModuleTest::DataDescriptorMapType & inputs = myModuleTest->GetDataMap();

  std::cout<<"Found "<<inputs.size()<<" inputs."<<std::endl;

  return EXIT_SUCCESS;
}
