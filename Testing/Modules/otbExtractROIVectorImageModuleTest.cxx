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

#include "otbExtractROIModule.h"

#include "otbVectorImage.h"
#include "otbImageFileReader.h"
#include "otbImageFileWriter.h"

int otbExtractROIVectorImageModuleTest(int argc, char* argv[])
{
  otb::ExtractROIModule::Pointer specificModule = otb::ExtractROIModule::New();
  otb::Module::Pointer module = specificModule.GetPointer();
  
  std::cout<<"Module: "<<module<<std::endl;

  // Put in the tests
  const char * infname = argv[1];
  const char * outfname = argv[2];

  typedef otb::VectorImage<double,2>  ImageType;
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
  specificModule->vStartX->value(atof(argv[3]));
  specificModule->vStartY->value(atof(argv[4]));
  specificModule->vSizeX->value(atof(argv[5]));
  specificModule->vSizeY->value(atof(argv[6]));
  Fl::check();

  // Simulate Ok button callback
  specificModule->bOk->do_callback();
  Fl::check();

  otb::DataObjectWrapper wrapperOut = module->GetOutputByKey("OutputImage");

  std::cout<<"Output wrapper: "<<wrapperOut<<std::endl;

  ImageType::Pointer outImage = dynamic_cast<ImageType *>(wrapperOut.GetDataObject());

  //Write the image
  WriterType::Pointer  writer = WriterType::New();
  writer->SetFileName(outfname);
  writer->SetInput(outImage);
  writer->Update();

  return EXIT_SUCCESS;

}

