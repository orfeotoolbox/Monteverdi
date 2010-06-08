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
#include "otbChangeDetectionModule.h"

#include "otbVectorImage.h"
#include "otbImageFileReader.h"
#include "otbImageFileWriter.h"

int otbChangeDetectionModuleTest(int argc, char* argv[])
{
  otb::ChangeDetectionModule::Pointer specificModule = otb::ChangeDetectionModule::New();
  otb::Module::Pointer module = specificModule.GetPointer();
  
  std::cout<<"Module: "<<module<<std::endl;

  // 
  const char * rinfname = argv[1];
  const char * linfname = argv[2];
  const char * svmmodel = argv[3];
  
  bool  run = atoi(argv[5]);
  
  typedef otb::ChangeDetectionModule::ImageType ImageType;
  typedef otb::ImageFileReader<ImageType>       ReaderType;
  typedef otb::ImageFileWriter<ImageType>       WriterType;

  // reader Right
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName(rinfname);
  reader->GenerateOutputInformation();

  // reader Left
  ReaderType::Pointer reader1 = ReaderType::New();
  reader1->SetFileName(linfname);
  reader1->GenerateOutputInformation();

  // Add Wrapper Input
  otb::DataObjectWrapper lwrapperIn = otb::DataObjectWrapper::Create(reader->GetOutput());
  std::cout<<"Input wrapper: "<<lwrapperIn<<std::endl;
  module->AddInputByKey("RightImage",lwrapperIn);

  // Add Wrapper Input1
  otb::DataObjectWrapper rwrapperIn = otb::DataObjectWrapper::Create(reader1->GetOutput());
  std::cout<<"Input wrapper: "<<rwrapperIn<<std::endl;
  module->AddInputByKey("LeftImage",rwrapperIn);

  //Start the module
  module->Start();
  
  // Load SVMModel
  specificModule->GetChangeDetection()->SetModelFileName(svmmodel);
  specificModule->GetChangeDetection()->LoadSVMModel();
  // Simulate OK Click
  specificModule->GetChangeDetection()->bOK->do_callback();

  if(run)
    {
      Fl::run();
    }
  else
    {
      Fl::check();
    }


  otb::DataObjectWrapper wrapperOut = module->GetOutputByKey("OutputImage");
  std::cout<<"Output wrapper: "<<wrapperOut<<std::endl;
  ImageType::Pointer outImage = dynamic_cast<ImageType *>(wrapperOut.GetDataObject());

  //Write the image
  WriterType::Pointer  writer = WriterType::New();
  writer->SetFileName(argv[4]);
  writer->SetInput(outImage);
  writer->Update();

  return EXIT_SUCCESS;

}

