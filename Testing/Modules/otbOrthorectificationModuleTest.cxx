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
#include "otbOrthorectificationModule.h"

#include "otbVectorImage.h"
#include "otbImageFileReader.h"
#include "otbImageFileWriter.h"

int main(int argc, char* argv[])
{
  otb::OrthorectificationModule::Pointer specificModule = otb::OrthorectificationModule::New();
  otb::Module::Pointer module = specificModule.GetPointer();
  
  std::cout<<"Module: "<<module<<std::endl;

  // Put in the tests
  const char * infname = argv[1];
  typedef otb::VectorImage<short,2>           ImageType;
  typedef otb::ImageFileReader<ImageType>     ReaderType;
  typedef otb::ImageFileWriter<ImageType>     WriterType;

  // reader
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName(infname);
  reader->GenerateOutputInformation();

  // Add Wrapper Input 
  otb::DataObjectWrapper wrapperIn("Short_Point_VectorImage",reader->GetOutput());
  std::cout<<"Input wrapper: "<<wrapperIn<<std::endl;
  module->AddInputByKey("InputImage",wrapperIn);

  module->Start();

  // Simulate the update of the output parameters : 
  itk::OStringStream oss;
  oss.str("100");
  specificModule->GetOrthorectification()->guiSizeX->value(oss.str().c_str());
  specificModule->GetOrthorectification()->guiSizeY->value(oss.str().c_str());
  oss.str("");
  specificModule->GetOrthorectification()->guiSizeX->do_callback();
  specificModule->GetOrthorectification()->guiSizeY->do_callback();
  Fl::check();

  // Simulate Ok button callback
  specificModule->GetOrthorectification()->guiOK->do_callback();
  
  // Refresh
  Fl::check();

  otb::DataObjectWrapper wrapperOut = module->GetOutputByKey("OutputImage");

  std::cout<<"Output wrapper: "<<wrapperOut<<std::endl;

  ImageType::Pointer outImage = dynamic_cast<ImageType *>(wrapperOut.GetDataObject());

  //Write the image
  WriterType::Pointer  writer = WriterType::New();
  writer->SetFileName(argv[2]);
  writer->SetInput(outImage);
  writer->Update();

  return 0;

}

