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
#include "otbDEMToImageGeneratorModule.h"

#include "otbImage.h"
#include "otbImageFileReader.h"
#include "otbImageFileWriter.h"

int otbDEMToImageGeneratorModuleTest(int argc, char* argv[])
{
  otb::DEMToImageGeneratorModule::Pointer specificModule = otb::DEMToImageGeneratorModule::New();
  otb::Module::Pointer                   module = specificModule.GetPointer();

  std::cout << "Module: " << module << std::endl;

  // Put in the tests
  const char * infname = argv[1];
  const char * demrep  = argv[2];

  typedef otb::DEMToImageGeneratorModule::InputImageType  ImageType;
  typedef otb::ImageFileReader<ImageType>                 ReaderType;
  typedef otb::ImageFileWriter<ImageType>                 WriterType;

  // reader
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName(infname);
  reader->GenerateOutputInformation();

  // Add Wrapper Input
  otb::DataObjectWrapper wrapperIn = otb::DataObjectWrapper::Create(reader->GetOutput());
  std::cout << "Input wrapper: " << wrapperIn << std::endl;
  module->AddInputByKey("InputImage", wrapperIn);

  module->Start();

  // Simulate the update of the output parameters :
  specificModule->GetView()->iDEMPath->value(demrep);

  itk::OStringStream oss;
  oss.str("500");
  specificModule->GetView()->oSizeX->value(oss.str().c_str());
  specificModule->GetView()->oSizeY->value(oss.str().c_str());
  oss.str("50");
  specificModule->GetView()->oSpacingX->value(oss.str().c_str()); ;
  specificModule->GetView()->oSpacingY->value(oss.str().c_str()); ;
//  specificModule->GetView()->oRadiusHillShading
  Fl::check();

  // Simulate Ok button callback
  specificModule->GetView()->guiOK->do_callback();

  // Refresh
  Fl::check();

  otb::DataObjectWrapper wrapperHillShading = module->GetOutputByKey("Hill Shading Image");

  std::cout << "Output HillShading: " << wrapperHillShading << std::endl;

  ImageType::Pointer outImage = dynamic_cast<ImageType *>(wrapperHillShading.GetDataObject());

  //Write the image
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName(argv[3]);
  writer->SetInput(outImage);
  writer->Update();

  return EXIT_SUCCESS;

}
