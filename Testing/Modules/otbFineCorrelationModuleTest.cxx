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
#include "otbFineCorrelationModule.h"

#include "otbVectorImage.h"
#include "otbImageFileWriter.h"
#include "otbImageFileReader.h"

int otbFineCorrelationModuleTest(int argc, char* argv[])
{
  otb::FineCorrelationModule::Pointer specificModule = otb::FineCorrelationModule::New();
  otb::Module::Pointer              module = specificModule.GetPointer();

  std::cout << "Module: " << module << std::endl;

  // Put in the tests
  const char * infnameRef = argv[1];
  const char * infnameMov = argv[2];
  const char * outfname1  = argv[3];
  const char * outfname2  = argv[4];
  const char * outfname3  = argv[5];
  


  typedef otb::FineCorrelationModule::ImageType ImageType;
  typedef otb::ImageFileWriter<ImageType>       WriterType;
  typedef otb::ImageFileReader<ImageType>       ReaderType;


  // Image reader
  ReaderType::Pointer readerRef = ReaderType::New();
  readerRef->SetFileName(infnameRef);
  readerRef->GenerateOutputInformation();

  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName(infnameMov);
  reader->GenerateOutputInformation();

  // Add Wrapper Input reference image
  otb::DataObjectWrapper wrapperRef = otb::DataObjectWrapper::Create(reader->GetOutput());
  std::cout << "Input image wrapper: " << wrapperRef << std::endl;
  module->AddInputByKey("ReferenceInputImage", wrapperRef);

  // Add Wrapper Input moving image
 otb::DataObjectWrapper wrapperMov = otb::DataObjectWrapper::Create(reader->GetOutput());
 std::cout << "Input image wrapper: " << wrapperMov << std::endl;
 module->AddInputByKey("SecondaryInputImage", wrapperMov);


  module->Start();

  
  // gaussian param
  specificModule->vFixedSigma->value(0.5);
  specificModule->vFixedSigma->do_callback();
  Fl::check();
  specificModule->vFixedSigma->value(0.1);
  specificModule->vFixedSigma->do_callback();
  Fl::check();
  specificModule->vSmoothFixedImage->value(0);
  specificModule->vSmoothFixedImage->do_callback();
  Fl::check();

  // Initial offset
  specificModule->vInitialOffsetX->value(2);
  specificModule->vInitialOffsetX->do_callback();
  Fl::check();
  specificModule->vInitialOffsetY->value(0.1);
  specificModule->vInitialOffsetY->do_callback();
  Fl::check();

  // Radius
  specificModule->vCorrelationRadius->value(3);
  specificModule->vCorrelationRadius->do_callback();
  Fl::check();
  specificModule->vSearchRadius->value(5);
  specificModule->vSearchRadius->do_callback();
  Fl::check();

  // Output grid
  specificModule->vGridStep->value(5);
  specificModule->vGridStep->do_callback();
  Fl::check();
  specificModule->vPrecision->value(0.2);
  specificModule->vPrecision->do_callback();
  Fl::check();

  //
  specificModule->vWarpMovingImage->value(1);
  Fl::check();

  if(argc>6 && atoi(argv[6])==1)
    Fl::run();

  // Simulate Ok button callback
  specificModule->bOk->do_callback();

  otb::DataObjectWrapper wrapperOut1 = module->GetOutputByKey("Correlation");
  std::cout << "Output wrapper: " << wrapperOut1 << std::endl;
  ImageType::Pointer out1 = dynamic_cast<ImageType *>(wrapperOut1.GetDataObject());

  //Write the output vector data
  WriterType::Pointer writer1 = WriterType::New();
  writer1->SetFileName(outfname1);
  writer1->SetInput(out1);
  writer1->Update();

  otb::DataObjectWrapper wrapperOut2 = module->GetOutputByKey("XDeformationField");
  std::cout << "Output wrapper: " << wrapperOut1 << std::endl;
  ImageType::Pointer out2 = dynamic_cast<ImageType *>(wrapperOut2.GetDataObject());

  //Write the output vector data
  WriterType::Pointer writer2 = WriterType::New();
  writer2->SetFileName(outfname2);
  writer2->SetInput(out2);
  writer2->Update();

  otb::DataObjectWrapper wrapperOut3 = module->GetOutputByKey("YDeformationField");
  std::cout << "Output wrapper: " << wrapperOut3 << std::endl;
  ImageType::Pointer out3 = dynamic_cast<ImageType *>(wrapperOut3.GetDataObject());

  //Write the output vector data
  WriterType::Pointer writer3 = WriterType::New();
  writer3->SetFileName(outfname3);
  writer3->SetInput(out3);
  writer3->Update();

  return EXIT_SUCCESS;

}
