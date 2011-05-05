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
#include "otbSpectrumModule.h"

#include "otbImage.h"
#include "otbImageFileReader.h"
#include "otbImageFileWriter.h"

#include "otbVectorData.h"
#include "otbVectorDataFileReader.h"

int otbSpectrumViewerModuleTest(int argc, char* argv[])
{
  otb::SpectrumModule::Pointer specificModule = otb::SpectrumModule::New();
  otb::Module::Pointer         module = specificModule.GetPointer();

  std::cout << "Module: " << module << std::endl;

  // Put in the tests
  const char * infname = argv[1];
  bool         run = atoi(argv[2]);

  typedef otb::SpectrumModule::ImageType    ImageType;
  typedef otb::ImageFileReader<ImageType>   ReaderType;
  typedef otb::ImageFileWriter<ImageType>   WriterType;

  // Image reader
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName(infname);
  reader->GenerateOutputInformation();

  otb::DataObjectWrapper wrapperIn = otb::DataObjectWrapper::Create(reader->GetOutput());
  wrapperIn.SetDescription(itksys::SystemTools::GetFilenameName(infname));
  std::cout << "Input wrapper: " << wrapperIn << std::endl;

  module->AddInputByKey("InputImage", wrapperIn);
  module->Start();

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
