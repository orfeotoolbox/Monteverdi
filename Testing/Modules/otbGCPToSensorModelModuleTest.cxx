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

#include "otbGCPToSensorModelModule.h"

#include "otbImage.h"
#include "otbImageFileReader.h"
#include "otbImageFileWriter.h"

int otbGCPToSensorModelModuleTest(int argc, char* argv[])
{
  otb::GCPToSensorModelModule::Pointer gcpModule = otb::GCPToSensorModelModule::New();
  otb::Module::Pointer module = gcpModule.GetPointer();
  
  std::cout<<"Module: "<<module<<std::endl;

  // Put in the tests
  const char * infname = argv[1];
  typedef otb::VectorImage<double,2>      ImageType;
  typedef otb::ImageFileReader<ImageType> ReaderType;
  typedef otb::ImageFileWriter<ImageType> WriterType;


  //reader
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName(infname);
  reader->GenerateOutputInformation();
  std::cout<<reader->GetOutput()->GetImageKeywordlist()<<std::endl;
  otb::DataObjectWrapper wrapperIn = otb::DataObjectWrapper::Create(reader->GetOutput());
  std::cout<<"Input wrapper: "<<wrapperIn<<std::endl;

  module->AddInputByKey("InputImage",wrapperIn);
  module->Start();

  //Fl::run();
  
  // Simulate Ok button callback
  gcpModule->GetView()->bOk->do_callback();
  Fl::check();

  otb::DataObjectWrapper wrapperOut = module->GetOutputByKey("OutputImage");

  std::cout<<"Output wrapper: "<<wrapperOut<<std::endl;

  ImageType::Pointer outImage = dynamic_cast<ImageType *>(wrapperOut.GetDataObject());

  std::cout<<outImage->GetImageKeywordlist()<<std::endl;
  
  return EXIT_SUCCESS;

}

