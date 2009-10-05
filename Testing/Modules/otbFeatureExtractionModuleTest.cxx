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

#include "otbFeatureExtractionModule.h"

#include "otbImage.h"
#include "otbImageFileReader.h"
#include "otbImageFileWriter.h"

int otbFeatureExtractionModuleTest(int argc, char* argv[])
{
  if (argc != 3 )
  {
    std::cout << argv[0] << "\t" << "input image" << "\t" << "output image" << std::endl;
    return EXIT_FAILURE;
  }
  
  otb::FeatureExtractionModule::Pointer specificModule = otb::FeatureExtractionModule::New();
  otb::Module::Pointer module = specificModule.GetPointer();
  
  std::cout<<"Module: "<<module<<std::endl;

  // Put in the tests
  const char * infname = argv[1];
  typedef otb::VectorImage<double,2>  ImageType;
  typedef otb::ImageFileReader<ImageType>     ReaderType;
  typedef otb::ImageFileWriter<ImageType>     WriterType;


  //reader
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName(infname);
  reader->GenerateOutputInformation();

  otb::DataObjectWrapper wrapperIn = otb::DataObjectWrapper::Create(reader->GetOutput());
  std::cout<<"Input wrapper: "<<wrapperIn<<std::endl;
//   std::cout<<"ad key.."<<std::endl;
  module->AddInputByKey("InputImage",wrapperIn);
//   std::cout<<"start..."<<std::endl;
  module->Start();
//   std::cout<<"start over"<<std::endl;
  Fl::check();

  //Simulate use of the application
  // ------------ Chose NDVI feature
//   std::cout<<"ndvi begin..."<<std::endl;
  specificModule->GetView()->UpdateParameterArea(8);
  specificModule->GetView()->SetFeatureType(otb::NDVI);
  Fl::check();

  //Select bands 2 and 3
  specificModule->GetView()->guiRAndNIRR->value(2);
  specificModule->GetView()->guiRAndNIRR->redraw();
  Fl::check();
  specificModule->GetView()->guiRAndNIRR->do_callback();
  specificModule->GetView()->guiRAndNIRNIR->value(1);
  specificModule->GetView()->guiRAndNIRNIR->redraw();
  Fl::check();
  specificModule->GetView()->guiRAndNIRNIR->do_callback();

  //Add the feature
  specificModule->GetView()->guiAdd->do_callback();
  specificModule->GetView()->guiFeatureListAction->redraw();
  Fl::check();
  
  
  // Simulate Ok (save) button callback
  std::cout<<"save image"<<std::endl;
  specificModule->GetView()->guiOK->do_callback();
  Fl::check();

  otb::DataObjectWrapper wrapperOut = module->GetOutputByKey("OutputImage");

  std::cout<<"Output wrapper: "<<wrapperOut<<std::endl;
  //std::cout<<"..."<<std::endl;
  ImageType::Pointer outImage = dynamic_cast<ImageType *>(wrapperOut.GetDataObject());

  //Write the image
  WriterType::Pointer  writer = WriterType::New();
  writer->SetFileName(argv[2]);
  writer->SetInput(outImage);
  writer->Update();

  return 0;

}

