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

#include "otbKMeansModule.h"
#include "otbVectorImage.h"
#include "otbImageFileReader.h"
#include "otbImageFileWriter.h"

int otbKMeansModuleTest(int argc, char* argv[])
{
  otb::KMeansModule::Pointer pointModule = otb::KMeansModule::New(); 
  otb::Module::Pointer             module = pointModule.GetPointer();

  std::cout << "Module: " << module << std::endl;

  // Put in the tests
  //typedef otb::VectorImage<float,2>        ImageType;
  typedef otb::KMeansModule::FloatingVectorImageType        ImageType;
  typedef otb::KMeansModule::LabeledImageType             LabelImageType;
  typedef otb::ImageFileReader<ImageType>                 ReaderType;
  typedef otb::ImageFileWriter<LabelImageType>         WriterType;

  //reader
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName(argv[1]);
  reader->GenerateOutputInformation();

  otb::DataObjectWrapper wrapperIn = otb::DataObjectWrapper::Create(reader->GetOutput());

  module->AddInputByKey("InputImage", wrapperIn);

  module->Start();

  Fl::check();

  pointModule->bOk->do_callback();
  Fl::check();
  Fl::run();

  otb::DataObjectWrapper wrapperOut = module->GetOutputByKey("KMeans labeled image");
  std::cout << "Output wrapper: " << wrapperOut << std::endl;
  LabelImageType::Pointer outImage = dynamic_cast<LabelImageType *>(wrapperOut.GetDataObject());

  //Write the image
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName(argv[2]);
  writer->SetInput(outImage);
  writer->Update();

  return EXIT_SUCCESS;
}
