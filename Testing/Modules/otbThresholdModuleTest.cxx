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
#include "otbThresholdModule.h"

#include "otbVectorImage.h"
#include "otbImageFileReader.h"
#include "otbImageFileWriter.h"


int otbThresholdModuleTest(int argc, char* argv[])
{
  otb::ThresholdModule::Pointer specificModule = otb::ThresholdModule::New();
  otb::Module::Pointer module = specificModule.GetPointer();
  
  std::cout<<"Module: "<<module<<std::endl;

  // Inputs
  const char * infname = argv[1];
  bool         run     = atoi(argv[2]);
  
  typedef otb::Image<double,2>                ImageType;
  typedef otb::ImageFileReader<ImageType>     ReaderType;
  typedef otb::ImageFileWriter<ImageType>     WriterType;
  
  //reader
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName(infname);
  reader->GenerateOutputInformation();
  
  otb::DataObjectWrapper wrapperIn = otb::DataObjectWrapper::Create(reader->GetOutput());
  std::cout<<"Input wrapper: "<<wrapperIn<<std::endl;

  module->AddInputByKey("InputImage",wrapperIn);
  module->Start();
  
  // Get the extremum of the image
  double min = specificModule->guiMinDetails->value();
  
  specificModule->guiMinDetails->value(min + 50);
  specificModule->guiMaxDetails->value(min + 100);
  specificModule->guiGenericThreshold->do_callback();
  // Simulate OK click
  specificModule->bOK->do_callback();
  
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
  writer->SetFileName(argv[3]);
  writer->SetInput(outImage);
  writer->Update();
  
  return EXIT_SUCCESS;
}
