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
#include "otbSupervisedClassificationModule.h"

#include "otbImage.h"
#include "otbVectorImage.h"
#include "otbImageFileReader.h"
#include "otbImageFileWriter.h"

int otbSupervisedClassificationModuleTest(int argc, char* argv[])
{
  otb::SupervisedClassificationModule::Pointer specificModule = otb::SupervisedClassificationModule::New();
  otb::Module::Pointer module = specificModule.GetPointer();
  
  std::cout<<"Module: "<<module<<std::endl;

  // Put in the tests
  const char * infname = argv[1];
  typedef otb::VectorImage<double,2>                 ImageType;
  typedef otb::VectorImage<unsigned char ,2>         LabeledImageType;
  
  typedef otb::ImageFileReader<ImageType>            ReaderType;
  typedef otb::ImageFileWriter<LabeledImageType>     WriterType;

  // reader
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName(infname);
  reader->GenerateOutputInformation();

  // Add Wrapper Input
  otb::DataObjectWrapper wrapperIn = otb::DataObjectWrapper::Create(reader->GetOutput());
  std::cout<<"Input wrapper: "<<wrapperIn<<std::endl;
  module->AddInputByKey("InputImage",wrapperIn);
  specificModule->SetModel(argv[3]);
  module->Start();
  Fl::check();

  //Simulate Ok button callback
  specificModule->GetSupervisedClassification()->bDisplay->do_callback();

  // Get Output DataWrapper
  otb::DataObjectWrapper wrapperOut = module->GetOutputByKey("OutputImage");
  
  std::cout<<"Output wrapper: "<<wrapperOut<<std::endl;
  
  LabeledImageType::Pointer outImage = dynamic_cast<LabeledImageType *>(wrapperOut.GetDataObject());
  
  //Write the image
  WriterType::Pointer  writer = WriterType::New();
  writer->SetFileName(argv[2]);
  writer->SetInput(outImage);
  writer->Update();
  
  return 0;

}

