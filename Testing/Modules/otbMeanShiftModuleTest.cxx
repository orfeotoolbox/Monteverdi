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
#include "otbMeanShiftModule.h"

#include "otbImageFileReader.h"
#include "otbImageFileWriter.h"

int otbMeanShiftModuleTest(int argc, char* argv[])
{
  otb::MeanShiftModule::Pointer specificModule = otb::MeanShiftModule::New();
  otb::Module::Pointer          module = specificModule.GetPointer();

  std::cout << "Module: " << module << std::endl;

  // Put in the tests
  const char * infname  = argv[1];
  const char * oufname1 = argv[2];
  const char * oufname2 = argv[3];
  const char * oufname3 = argv[4];
  const char * oufname4 = argv[5];
  const unsigned int spaRad     = atoi(argv[6]);
  const unsigned int speRad     = atoi(argv[7]);
  const unsigned int minRegSize = atoi(argv[8]);
 
  typedef otb::MeanShiftModule::FloatingVectorImageType  ImageType;
  typedef otb::MeanShiftModule::LabelImageType           LabelImageType;
  typedef otb::ImageFileReader<ImageType>                ReaderType;
  typedef otb::ImageFileWriter<ImageType>                ImageWriterType;
  typedef otb::ImageFileWriter<LabelImageType>           LabelImageWriterType;
  
  // Reader
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName(infname);
  reader->GenerateOutputInformation();

  otb::DataObjectWrapper wrapperIn = otb::DataObjectWrapper::Create(reader->GetOutput());
  std::cout << "Input wrapper: " << wrapperIn << std::endl;
  
  module->AddInputByKey("InputImage", wrapperIn);
 
  module->Start();

  Fl::check();

  // Simulate adjusting parameters
  specificModule->GetView()->mSpatialRadius->value(spaRad);
  specificModule->GetView()->mSpatialRadius->do_callback();
  specificModule->GetView()->mSpectralRadius->value(speRad);
  specificModule->GetView()->mSpectralRadius->do_callback();
  specificModule->GetView()->mMinRegionSize->value(minRegSize);
  specificModule->GetView()->mMinRegionSize->do_callback();
  Fl::check();

  // Simulate display options
  specificModule->GetView()->mOpacity->value(0.35);
  specificModule->GetView()->mOpacity->do_callback();  
  specificModule->GetView()->mBoundButton->value(1);
  specificModule->GetView()->mBoundButton->do_callback();
  specificModule->GetView()->mClustersButton->value(1);
  specificModule->GetView()->mClustersButton->do_callback();
  
  Fl::check();

  specificModule->GetView()->RunSegmentation();
  Fl::check();
  
  // Exit the GUI and save the result
  specificModule->GetView()->Exit();
  
  otb::DataObjectWrapper wrapperOut1 = module->GetOutputByKey("Filtered Image");
  std::cout << "Output wrapper: " << wrapperOut1 << std::endl;
  ImageType::Pointer outImage1 = dynamic_cast<ImageType *>(wrapperOut1.GetDataObject());

  otb::DataObjectWrapper wrapperOut2 = module->GetOutputByKey("Clustered Image");
  std::cout << "Output wrapper: " << wrapperOut2 << std::endl;
  ImageType::Pointer outImage2 = dynamic_cast<ImageType *>(wrapperOut2.GetDataObject());

  otb::DataObjectWrapper wrapperOut3 = module->GetOutputByKey("Labeled Image");
  std::cout << "Output wrapper: " << wrapperOut3 << std::endl;
  LabelImageType::Pointer outImage3 = dynamic_cast<LabelImageType *>(wrapperOut3.GetDataObject());

  otb::DataObjectWrapper wrapperOut4 = module->GetOutputByKey("Boundaries Image");
  std::cout << "Output wrapper: " << wrapperOut4 << std::endl;
  LabelImageType::Pointer outImage4 = dynamic_cast<LabelImageType *>(wrapperOut4.GetDataObject());


  //Write the images
  ImageWriterType::Pointer writer1 = ImageWriterType::New();
  writer1->SetFileName(argv[2]);
  writer1->SetInput(outImage1);
  writer1->Update();
  
  ImageWriterType::Pointer writer2 = ImageWriterType::New();
  writer2->SetFileName(argv[3]);
  writer2->SetInput(outImage2);
  writer2->Update();
  
  LabelImageWriterType::Pointer writer3 = LabelImageWriterType::New();
  writer3->SetFileName(argv[4]);
  writer3->SetInput(outImage3);
  writer3->Update();

  LabelImageWriterType::Pointer writer4 = LabelImageWriterType::New();
  writer4->SetFileName(argv[5]);
  writer4->SetInput(outImage4);
  writer4->Update();

  return EXIT_SUCCESS;
}
