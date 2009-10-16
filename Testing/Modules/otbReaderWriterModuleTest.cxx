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

#include "otbReaderModule.h"
#include "otbWriterModule.h"

#include "otbVectorImage.h"
#include "otbImageFileWriter.h"

int otbReaderWriterModuleTest(int argc, char* argv[])
{
  otb::ReaderModule::Pointer specificReaderModule = otb::ReaderModule::New();
  otb::Module::Pointer readerModule = specificReaderModule.GetPointer();
  
  otb::WriterModule::Pointer specificWriterModule = otb::WriterModule::New();
  otb::Module::Pointer writerModule = specificWriterModule.GetPointer();
  
  std::cout<<"Module: "<<readerModule<<std::endl;
  
  readerModule->Start();
  Fl::check();

  // Simulate file chooser and ok callback
  specificReaderModule->vFilePath->value(argv[1]);
  specificReaderModule->vFilePath->do_callback();
  specificReaderModule->vName->value("test");
  specificReaderModule->bOk->do_callback();

  otb::DataObjectWrapper wrapperOut = readerModule->GetOutputByKey("test");

  std::cout<<"Output wrapper: "<<wrapperOut<<std::endl;

  //Writer module now
  writerModule->AddInputByKey("InputDataSet",wrapperOut);
  writerModule->Start();
  Fl::check();
  specificWriterModule->vFilePath->value(argv[2]);  
  Fl::check();
  specificWriterModule->bOk->do_callback();

  Fl::run();

  return EXIT_SUCCESS;
}

