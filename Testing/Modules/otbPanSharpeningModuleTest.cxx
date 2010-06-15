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


#include "otbPanSharpeningModule.h"

#include "otbVectorImage.h"
#include "otbImageFileReader.h"
#include "otbImageFileWriter.h"

int otbPanSharpeningModuleTest(int argc, char* argv[])
{
  otb::PanSharpeningModule::Pointer specificModule = otb::PanSharpeningModule::New();
  otb::Module::Pointer module = specificModule.GetPointer();

  std::cout<<"Module: "<<module<<std::endl;

  // Put in the tests
  const char * panfname = argv[1];
  const char * xsfname = argv[2];

  typedef otb::PanSharpeningModule::FloatingImageType       PanImageType;
  typedef otb::PanSharpeningModule::FloatingVectorImageType XsImageType;
  typedef otb::ImageFileReader<PanImageType>  ReaderPanType;
  typedef otb::ImageFileReader<XsImageType>   ReaderXsType;
  typedef otb::ImageFileWriter<XsImageType>   WriterType;

  //reader pan
  ReaderPanType::Pointer readerPan = ReaderPanType::New();
  readerPan->SetFileName(panfname);
  readerPan->GenerateOutputInformation();

  //reader xs
  ReaderXsType::Pointer readerXs = ReaderXsType::New();
  readerXs->SetFileName(xsfname);
  readerXs->GenerateOutputInformation();

  otb::DataObjectWrapper wrapperPan = otb::DataObjectWrapper::Create(readerPan->GetOutput());
  std::cout<<"Pan wrapper: "<< wrapperPan << std::endl;

  module->AddInputByKey("PanImage",wrapperPan);

  otb::DataObjectWrapper wrapperXs = otb::DataObjectWrapper::Create(readerXs->GetOutput());
  std::cout<<"Xs wrapper: "<< wrapperXs << std::endl;

  module->AddInputByKey("XsImage",wrapperXs);


  module->Start();

  otb::DataObjectWrapper wrapperOut = module->GetOutputByKey("PanSharpenedImageOutput");

  std::cout<<"Output wrapper: "<<wrapperOut<<std::endl;

  XsImageType::Pointer outImage = dynamic_cast<XsImageType *>(wrapperOut.GetDataObject());

  //Write the image
  WriterType::Pointer  writer = WriterType::New();
  writer->SetFileName(argv[3]);
  writer->SetInput(outImage);
  writer->Update();

  return EXIT_SUCCESS;

}

