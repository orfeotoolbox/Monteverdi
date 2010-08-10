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

#include "otbParserModule.h"

#include "otbImage.h"
#include "otbImageFileReader.h"
#include "otbImageFileWriter.h"

int otbParserModuleTest(int argc, char* argv[])
{
  otb::ParserModule::Pointer specificModule = otb::ParserModule::New();
  otb::Module::Pointer        module = specificModule.GetPointer();

  std::cout << "Module: " << module << std::endl;

  // Put in the tests
  const char * infname1 = argv[1];
  const char * infname2 = argv[2];
  bool         run = atoi(argv[4]);

  typedef otb::ParserModule::ImageType    ImageType;
  typedef otb::ImageFileReader<ImageType> ReaderType;
  typedef otb::ImageFileWriter<ImageType> WriterType;

  // Reader
  ReaderType::Pointer reader1 = ReaderType::New();
  ReaderType::Pointer reader2 = ReaderType::New();

  reader1->SetFileName(infname1);
  reader1->GenerateOutputInformation();
  reader2->SetFileName(infname2);
  reader2->GenerateOutputInformation();

  otb::DataObjectWrapper wrapperIn1 = otb::DataObjectWrapper::Create(reader1->GetOutput());
  std::cout << "Input wrapper: " << wrapperIn1 << std::endl;
  otb::DataObjectWrapper wrapperIn2 = otb::DataObjectWrapper::Create(reader2->GetOutput());
  std::cout << "Input wrapper: " << wrapperIn2 << std::endl;

  module->AddInputByKey("InputImage", wrapperIn1);
  module->AddInputByKey("InputImage", wrapperIn2);
  module->Start();

  // Simulate first variable renaming
  specificModule->ui_NewVarName->value("band1");
  specificModule->ui_ChangeVarName->do_callback();
   
  // Simulate Addition Operation
  specificModule->ui_Expression->value("band1+b2");
 
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
  writer->SetFileName(argv[3]);
  writer->SetInput(outImage);
  writer->Update();

  return EXIT_SUCCESS;

}
