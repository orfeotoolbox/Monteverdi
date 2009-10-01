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
#include "otbViewerModule.h"

#include "otbImage.h"
#include "otbImageFileReader.h"
#include "otbImageFileWriter.h"

#include "otbVectorData.h"
#include "otbVectorDataFileReader.h"

int main(int argc, char* argv[])
{
  otb::ViewerModule::Pointer specificModule = otb::ViewerModule::New();
  otb::Module::Pointer module = specificModule.GetPointer();
  
  std::cout<<"Module: "<<module<<std::endl;

  // Put in the tests
  const char * infname = argv[1];
  bool run = atoi(argv[2]);


  typedef otb::VectorImage<double,2>          ImageType;
  typedef otb::ImageFileReader<ImageType>     ReaderType;
  typedef otb::ImageFileWriter<ImageType>     WriterType;

  typedef otb::VectorData<double>                    VectorDataType;
  typedef otb::VectorDataFileReader<VectorDataType>  VReaderType;


  // Image reader
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName(infname);
  reader->GenerateOutputInformation();

  otb::DataObjectWrapper wrapperIn = otb::DataObjectWrapper::Create(reader->GetOutput());
  std::cout<<"Input wrapper: "<<wrapperIn<<std::endl;

  module->AddInputByKey("InputImage",wrapperIn);

  VReaderType::Pointer vreader = VReaderType::New();
  VReaderType::Pointer vreader1 = VReaderType::New();

  if(argc > 3)
  {
    const char * vfname  = argv[3];
    // VectorData Reader 
    vreader->SetFileName(vfname);
    vreader->GenerateOutputInformation();

    otb::DataObjectWrapper wrapperVector = otb::DataObjectWrapper::Create(vreader->GetOutput());
    std::cout <<"Input VectorData Wrapper " <<wrapperVector << std::endl;
    module->AddInputByKey("VectorData",wrapperVector);

    const char * vfname1  = argv[4];
    // VectorData Reader 
    vreader1->SetFileName(vfname1);
    vreader1->GenerateOutputInformation();

    otb::DataObjectWrapper wrapperVector1 = otb::DataObjectWrapper::Create(vreader1->GetOutput());
    std::cout <<"Input VectorData Wrapper " <<wrapperVector1 << std::endl;
    module->AddInputByKey("VectorData",wrapperVector1);

  }

  module->Start();

  if(run)
    Fl::run();
  else
    Fl::check();

  return EXIT_SUCCESS;

}
