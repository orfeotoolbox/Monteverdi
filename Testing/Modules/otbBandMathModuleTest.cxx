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

#include "otbBandMathModule.h"

#include "otbImage.h"
#include "otbImageFileReader.h"
#include "otbImageFileWriter.h"

int otbBandMathModuleTest(int argc, char* argv[])
{
  otb::BandMathModule::Pointer specificModule = otb::BandMathModule::New();
  otb::Module::Pointer        module = specificModule.GetPointer();

  std::cout << "Module: " << module << std::endl;

  // Put in the tests
  const char * infname1 = argv[1];
  const char * infname2 = argv[2];
  const char * infname3 = argv[3];
  bool run = atoi(argv[5]);

  typedef otb::BandMathModule::ImageType          ImageType;
  typedef otb::BandMathModule::VectorImageType    VectorImageType;
  typedef otb::ImageFileReader<ImageType>         ImageReaderType;
  typedef otb::ImageFileReader<VectorImageType>   VectorImageReaderType;
  typedef otb::ImageFileWriter<ImageType>         WriterType;

  // Reader
  ImageReaderType::Pointer reader1 = ImageReaderType::New();
  ImageReaderType::Pointer reader2 = ImageReaderType::New();
  VectorImageReaderType::Pointer reader3 = VectorImageReaderType::New();

  reader1->SetFileName(infname1);
  reader1->GenerateOutputInformation();
  reader2->SetFileName(infname2);
  reader2->GenerateOutputInformation();
  reader3->SetFileName(infname3);
  reader3->GenerateOutputInformation();

  otb::DataObjectWrapper wrapperIn1 = otb::DataObjectWrapper::Create(reader1->GetOutput());
  std::cout << "Input wrapper: " << wrapperIn1 << std::endl;
  otb::DataObjectWrapper wrapperIn2 = otb::DataObjectWrapper::Create(reader2->GetOutput());
  std::cout << "Input wrapper: " << wrapperIn2 << std::endl;
  otb::DataObjectWrapper wrapperIn3 = otb::DataObjectWrapper::Create(reader3->GetOutput());
  std::cout << "Input wrapper: " << wrapperIn3 << std::endl;

  module->AddInputByKey("InputImage", wrapperIn1);
  module->AddInputByKey("InputImage", wrapperIn2);
  module->AddInputByKey("InputImage", wrapperIn3);
  module->Start();

  // Simulate first variable renaming
  specificModule->ui_NewVarName->value("band1");
  specificModule->ui_ChangeVarName->do_callback();

  // Simulate use of the help window
  specificModule->ui_Help->value(1);
  specificModule->ui_Help->do_callback();
   
  // Simulate the indexes use
  specificModule->ui_AddIndexes->do_callback();

  // Simulate Operation
  specificModule->ui_Expression->value("band1+im2b1 / (im3b1+im3b2+im3b3) * im3b4 * idxX");
 
  Fl::check();

  if (run)
     {
     Fl::run();
     }
  else
     {
     Fl::check();
     }

  // Exit the GUI and save the result
  specificModule->ui_Ok->do_callback();

  otb::DataObjectWrapper wrapperOut = module->GetOutputByKey("OutputImage");
  std::cout << "Output wrapper: " << wrapperOut << std::endl;
  ImageType::Pointer outImage = dynamic_cast<ImageType *>(wrapperOut.GetDataObject());

  //Write the image
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName(argv[4]);
  writer->SetInput(outImage);
  writer->Update();

  return EXIT_SUCCESS;
}
