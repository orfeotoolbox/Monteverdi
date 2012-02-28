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

#include "otbRadiometricIndicesExtractionModule.h"
#include "otbFeatureRI.h"

#include "otbImage.h"
#include "otbImageFileReader.h"
#include "otbImageFileWriter.h"

int otbRadiometricIndicesExtractionModuleTest(int argc, char* argv[])
{
  if (argc != 3)
    {
    std::cout << argv[0] << "\t" << "input image" << "\t" << "output image" << std::endl;
    return EXIT_FAILURE;
    }

  otb::RadiometricIndicesExtractionModule::Pointer specificModule = otb::RadiometricIndicesExtractionModule::New();
  otb::Module::Pointer                  module = specificModule.GetPointer();

  std::cout << "Module: " << module << std::endl;

  // Put in the tests
  const char * infname = argv[1];
  typedef otb::RadiometricIndicesExtractionModule::InputImageType ImageType;
  typedef otb::ImageFileReader<ImageType>                         ReaderType;
  typedef otb::ImageFileWriter<ImageType>                         WriterType;

  //reader
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName(infname);
  reader->GenerateOutputInformation();

  otb::DataObjectWrapper wrapperIn = otb::DataObjectWrapper::Create(reader->GetOutput());
  std::cout << "Input wrapper: " << wrapperIn << std::endl;
//   std::cout<<"ad key.."<<std::endl;
  module->AddInputByKey("InputImage", wrapperIn);
//   std::cout<<"start..."<<std::endl;
  module->Start();
//   std::cout<<"start over"<<std::endl;
  Fl::check();

  //Simulate use of the application
  // ------------ Chose NDVI feature
//   std::cout<<"ndvi begin..."<<std::endl;
  specificModule->GetViewRI()->UpdateParameterArea(1);
  specificModule->GetViewRI()->SetFeatureType(otb::FeatureInfoRI::NDVI);
  Fl::check();

  //Select bands 2 and 3
  specificModule->GetViewRI()->m_SpecificGUI->guiRAndNIRR->value(2);
  specificModule->GetViewRI()->m_SpecificGUI->guiRAndNIRR->redraw();
  Fl::check();
  specificModule->GetViewRI()->m_SpecificGUI->guiRAndNIRR->do_callback();
  specificModule->GetViewRI()->m_SpecificGUI->guiRAndNIRNIR->value(1);
  specificModule->GetViewRI()->m_SpecificGUI->guiRAndNIRNIR->redraw();
  Fl::check();
  specificModule->GetViewRI()->m_SpecificGUI->guiRAndNIRNIR->do_callback();

  //Add the feature
  specificModule->GetViewRI()->guiAdd->do_callback();
  specificModule->GetViewRI()->guiFeatureListAction->redraw();
  Fl::check();

  // Simulate Ok (save) button callback
  std::cout << "save image" << std::endl;
  specificModule->GetViewRI()->guiOK->do_callback();
  Fl::check();

  otb::DataObjectWrapper wrapperOut = module->GetOutputByKey("OutputImage");

  std::cout << "Output wrapper: " << wrapperOut << std::endl;
  //std::cout<<"..."<<std::endl;
  ImageType::Pointer outImage = dynamic_cast<ImageType *>(wrapperOut.GetDataObject());

  //Write the image
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName(argv[2]);
  writer->SetInput(outImage);
  writer->Update();

  return 0;

}
