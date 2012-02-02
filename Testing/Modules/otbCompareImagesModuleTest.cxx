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
#include "otbCompareImagesModule.h"

#include "otbVectorImage.h"
#include "otbImageFileReader.h"
#include "otbImageFileWriter.h"

int otbCompareImagesModuleTest(int argc, char* argv[])
{
  otb::CompareImagesModule::Pointer specificModule = otb::CompareImagesModule::New();
  otb::Module::Pointer              module = specificModule.GetPointer();

  std::cout << "Module: " << module << std::endl;

  const char * rinfname = argv[1];
  const char * linfname = argv[2];

  bool run = atoi(argv[3]);

  typedef otb::CompareImagesModule::InputImageType ImageType;
  typedef otb::ImageFileReader<ImageType>          ReaderType;
  typedef otb::ImageFileWriter<ImageType>          WriterType;

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
  std::cout << "Input wrapper: " << lwrapperIn << std::endl;
  module->AddInputByKey("LeftInputImage", lwrapperIn);

  // Add Wrapper Input1
  otb::DataObjectWrapper rwrapperIn = otb::DataObjectWrapper::Create(reader1->GetOutput());
  std::cout << "Input wrapper: " << rwrapperIn << std::endl;
  module->AddInputByKey("RightInputImage", rwrapperIn);

  //Start the module
  module->Start();

  // Simulate OK Click
  specificModule->GetView()->bOK->do_callback();

  if (run)
    {
    Fl::run();
    }
  else
    {
    Fl::check();
    }

  return EXIT_SUCCESS;
}
