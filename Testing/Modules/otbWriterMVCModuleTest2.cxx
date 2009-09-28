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


#include "otbWriterMVCModule.h"

#include "otbVectorImage.h"
#include "otbImageFileReader.h"


int main(int argc, char* argv[])
{
  otb::WriterModule::Pointer specificModule = otb::WriterModule::New();
  otb::Module::Pointer module = specificModule.GetPointer();

  std::cout<<"Module: "<<module<<std::endl;

  // Put in the tests
  const char * infname = argv[1];
  typedef otb::VectorImage<double,2>  ImageType;
  typedef otb::ImageFileReader<ImageType>     ReaderType;

  //reader
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName(infname);
  reader->GenerateOutputInformation();

  otb::DataObjectWrapper wrapperIn("Floating_Point_VectorImage",reader->GetOutput());
  std::cout<<"Input wrapper: "<< wrapperIn << std::endl;
  
  module->AddInputByKey("InputImageDataSet",wrapperIn);
  
  module->Start();
  specificModule->GetView()->Show();
  Fl::run();
  /*
  // Simulate file chooser and ok callback
  specificModule->GetView()->vFilePath->value(argv[2]);
  //Select band to write
  Fl::check();
  specificModule->GetView()->guiFeatureList->value(1);
  specificModule->GetView()->guiAddChannel->do_callback();
  Fl::check();
  specificModule->GetView()->guiOutputFeatureList->value(2);
  specificModule->GetView()->guiOutputFeatureList->do_callback();
  Fl::check();
  
  specificModule->GetView()->guiOK->do_callback();
  */
  return EXIT_SUCCESS;

}

