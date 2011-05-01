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
#include "otbProjectionModule.h"

#include "otbImage.h"
#include "otbImageFileReader.h"
#include "otbImageFileWriter.h"

int otbProjectionModuleTest(int argc, char* argv[])
{
  otb::ProjectionModule::Pointer specificModule = otb::ProjectionModule::New();
  otb::Module::Pointer           module = specificModule.GetPointer();

  std::cout << "Module: " << module << std::endl;

  // Put in the tests
  const char * infname = argv[1];
  int          run = atoi(argv[2]);

  typedef otb::ProjectionModule::InputImageType ImageType;
  typedef otb::ImageFileReader<ImageType>       ReaderType;
  typedef otb::ImageFileWriter<ImageType>       WriterType;

  // Image reader
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName(infname);
  reader->GenerateOutputInformation();

  otb::DataObjectWrapper wrapperIn = otb::DataObjectWrapper::Create(reader->GetOutput());
  wrapperIn.SetDescription(itksys::SystemTools::GetFilenameName(infname));
  std::cout << "Input wrapper: " << wrapperIn << std::endl;

  module->AddInputByKey("InputImage", wrapperIn);
  module->Start();

  // Put OutputSize to 100, 100
  /*module*/ specificModule->GetView()->guiSizeX->value("100");
  /*module*/ specificModule->GetView()->guiSizeY->value("100");

  //Simulate click button
  if (run)
    {
    Fl::run();
    }
  else
    {
    Fl::check();
    }
  specificModule->GetView()->guiOK->do_callback();

  otb::DataObjectWrapper wrapperOut = module->GetOutputByKey("OutputImage");

  std::cout << "Output wrapper: " << wrapperOut << std::endl;

  ImageType::Pointer outImage = dynamic_cast<ImageType *>(wrapperOut.GetDataObject());

  //Write the image
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName(argv[3]);
  writer->SetInput(outImage);
  writer->Update();

  return EXIT_SUCCESS;

}
