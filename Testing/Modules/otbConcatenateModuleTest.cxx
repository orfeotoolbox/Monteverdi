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

#include "otbConcatenateModule.h"

#include "otbImage.h"
#include "otbVectorImage.h"
#include "otbImageFileReader.h"
#include "otbImageFileWriter.h"

int otbConcatenateModuleTest(int argc, char* argv[])
{
  otb::ConcatenateModule::Pointer specificModule = otb::ConcatenateModule::New();
  otb::Module::Pointer module = specificModule.GetPointer();
  
  std::cout<<"Module: "<<module<<std::endl;
  const unsigned int NbImages(argc-2);
  // Put in the tests
  typedef otb::ConcatenateModule::ImageType        ImageType;
  typedef otb::ConcatenateModule::VectorImageType  VectorImageType;
  typedef otb::ImageFileReader<ImageType>          ReaderType;
  typedef otb::ImageFileWriter<VectorImageType>    WriterType;

  std::cout << "Concat of " << NbImages << " images into a multi-band image " << std::endl;

  //reader
  std::vector<ReaderType::Pointer> listReader;
  unsigned int cpt(1);
  for(unsigned int i=0 ; i<NbImages ; i++,cpt++)
  {
    std::cout << "Adding image " <<argv[cpt]<<" image." << std::endl;
    ReaderType::Pointer imageReader = ReaderType::New();
    listReader.push_back(imageReader);
    imageReader->SetFileName(argv[cpt]);
    imageReader->GenerateOutputInformation();
    otb::DataObjectWrapper wrapperIn = otb::DataObjectWrapper::Create(imageReader->GetOutput());
    module->AddInputByKey("InputImage",wrapperIn);
  }
  module->Start();

  otb::DataObjectWrapper wrapperOut = module->GetOutputByKey("OutputImage");

  std::cout<<"Output wrapper: "<<wrapperOut<<std::endl;

  VectorImageType::Pointer outImage = dynamic_cast<VectorImageType *>(wrapperOut.GetDataObject());

  //Write the image
  std::cout << "Write output image " <<argv[cpt]<<std::endl;
  WriterType::Pointer  writer = WriterType::New();
  writer->SetFileName(argv[cpt]);
  writer->SetInput(outImage);
  writer->Update();

  return EXIT_SUCCESS;

}

