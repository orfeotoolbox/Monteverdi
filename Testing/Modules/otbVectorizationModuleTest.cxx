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

#include "otbVectorizationModule.h"

#include "otbImage.h"
#include "otbImageFileReader.h"
#include "otbImageFileWriter.h"

int otbVectorizationModuleTest(int argc, char* argv[])
{
  otb::VectorizationModule::Pointer vecModule = otb::VectorizationModule::New();
  otb::Module::Pointer module = vecModule.GetPointer();
  
  std::cout<<"Module: "<<module<<std::endl;

  if (argc != 3)
  {
     return EXIT_FAILURE;
  }

  // Put in the tests
  const char * infname = argv[1];
  typedef otb::VectorImage<double,2>      ImageType;
  typedef otb::ImageFileReader<ImageType> ReaderType;
  typedef otb::ImageFileWriter<ImageType> WriterType;

  //reader
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName(infname);
  reader->GenerateOutputInformation();

  otb::DataObjectWrapper wrapperIn = otb::DataObjectWrapper::Create(reader->GetOutput());

  module->AddInputByKey("InputImage",wrapperIn);
  module->Start();
  
  return EXIT_SUCCESS;

}

