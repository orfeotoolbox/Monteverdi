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

#include "otbResampleModule.h"

#include "otbVectorImage.h"
#include "otbImageFileReader.h"
#include "otbImageFileWriter.h"

int otbResampleModuleTest(int argc, char* argv[])
{
  otb::ResampleModule::Pointer specificModule = otb::ResampleModule::New();
  otb::Module::Pointer         module = specificModule.GetPointer();

  std::cout << "Module: " << module << std::endl;

  // Put in the tests
  const char * infname = argv[1];
  const char * outfname = argv[2];

  typedef otb::ResampleModule::ImageType  ImageType;
  typedef otb::ImageFileReader<ImageType> ReaderType;
  typedef otb::ImageFileWriter<ImageType> WriterType;

  //reader
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName(infname);
  reader->GenerateOutputInformation();

  otb::DataObjectWrapper wrapperIn = otb::DataObjectWrapper::Create(reader->GetOutput());
  std::cout << "Input wrapper: " << wrapperIn << std::endl;

  module->AddInputByKey("InputImage", wrapperIn);
  module->Start();
  specificModule->guiRotAngle->value(atoi(argv[3]));
  specificModule->guiRotAngle->do_callback();
  Fl::check();

  specificModule->guiXFactor->value(atof(argv[4]));
  specificModule->guiXFactor->do_callback();
  Fl::check();

  specificModule->guiKeepProportion->value(0);
  specificModule->guiKeepProportion->do_callback();
  Fl::check();

  specificModule->guiYFactor->value(atof(argv[5]));
  specificModule->guiYFactor->do_callback();
  specificModule->bUpdateView->do_callback();
  Fl::check();

  // Simulate Ok button callback
  specificModule->bOK->do_callback();
  Fl::check();

  otb::DataObjectWrapper wrapperOut = module->GetOutputByKey("OutputImage");

  std::cout << "Output wrapper: " << wrapperOut << std::endl;

  ImageType::Pointer outImage = dynamic_cast<ImageType *>(wrapperOut.GetDataObject());

  //Write the image
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName(outfname);
  writer->SetInput(outImage);
  writer->Update();

  return EXIT_SUCCESS;

}
