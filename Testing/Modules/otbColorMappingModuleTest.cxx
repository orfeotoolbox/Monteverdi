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
#include "ConfigureMonteverdi.h"

// For internationalisation


#include "otbColorMappingModule.h"
#include "otbImageFileReader.h"
#include "otbImageFileWriter.h"

int otbColorMappingModuleTest(int argc, char* argv[])
{

  // bool withComplex = false;
  // if (atoi(argv[2]) == 1) withComplex = true;

  //Internationalization
  

  otb::ColorMappingModule::Pointer pointModule = otb::ColorMappingModule::New();
  otb::Module::Pointer             module = pointModule.GetPointer();

  // Put in the tests
  typedef otb::ColorMappingModule::SingleImageType        ImageType;
  typedef otb::ImageFileReader<ImageType>                 ReaderType;

  //reader
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName(argv[1]);
  reader->GenerateOutputInformation();

  unsigned int  colorMapValue = atoi(argv[2]);
  otb::DataObjectWrapper wrapperIn = otb::DataObjectWrapper::Create(reader->GetOutput());

  module->AddInputByKey("InputImage", wrapperIn);

  module->Start();

  Fl::check();

  pointModule->iColorMap->value(colorMapValue);
  Fl::check();
  pointModule->bOK->do_callback();
  Fl::check();

  return EXIT_SUCCESS;
}
