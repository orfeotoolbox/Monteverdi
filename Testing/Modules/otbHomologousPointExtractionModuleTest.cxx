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
#include "ConfigureMonteverdi.h"

// For internationalisation
#include "otbI18n.h"

#include "otbHomologousPointExtractionModule.h"
#include "otbImageFileReader.h"
#include "otbImageFileWriter.h"

int otbHomologousPointExtractionModuleTest(int argc, char* argv[])
{
  //Internationalization
  otbI18nMacro();
  
  // Convenient typedefs
  typedef otb::VectorImage<double,2>             InputImageType;
  typedef InputImageType::IndexType              IndexType;
  typedef otb::ImageFileReader<InputImageType>   ReaderType;
  typedef otb::ImageFileWriter<InputImageType>   WriterType;

  otb::HomologousPointExtractionModule::Pointer pointModule = otb::HomologousPointExtractionModule::New();
  otb::Module::Pointer module = pointModule.GetPointer();
  
  // Put in the tests
  typedef otb::VectorImage<double,2>      ImageType;
  typedef otb::ImageFileReader<ImageType> ReaderType;
  typedef otb::ImageFileWriter<ImageType> WriterType;


  //reader
  ReaderType::Pointer reader1 = ReaderType::New();
  ReaderType::Pointer reader2 = ReaderType::New();
  reader1->SetFileName(argv[1]);
  reader1->GenerateOutputInformation();
  reader2->SetFileName(argv[2]);
  reader2->GenerateOutputInformation();

  otb::DataObjectWrapper wrapperIn1 = otb::DataObjectWrapper::Create(reader1->GetOutput());
  otb::DataObjectWrapper wrapperIn2 = otb::DataObjectWrapper::Create(reader2->GetOutput());
 
  module->AddInputByKey("FixInputImage",wrapperIn1);
  module->AddInputByKey("MovingInputImage",wrapperIn2);

  module->Start();
 
  Fl::check();

  // First point
  pointModule->GetView()->vX1->value(atoi(argv[4]));
  pointModule->GetView()->vY1->value(atoi(argv[5]));
  pointModule->GetView()->vX2->value(atoi(argv[6]));
  pointModule->GetView()->vY2->value(atoi(argv[7]));
  pointModule->GetView()->guiAdd->do_callback();
  Fl::check();
  // Second point
  pointModule->GetView()->vX1->value(atoi(argv[8]));
  pointModule->GetView()->vY1->value(atoi(argv[9]));
  pointModule->GetView()->vX2->value(atoi(argv[10]));
  pointModule->GetView()->vY2->value(atoi(argv[11]));
  pointModule->GetView()->guiAdd->do_callback();
  Fl::check();
  // Third point
  pointModule->GetView()->vX1->value(atoi(argv[12]));
  pointModule->GetView()->vY1->value(atoi(argv[13]));
  pointModule->GetView()->vX2->value(atoi(argv[14]));
  pointModule->GetView()->vY2->value(atoi(argv[15]));
  pointModule->GetView()->guiAdd->do_callback();
  Fl::check();

  pointModule->GetView()->guiAdd->do_callback();
  Fl::check();

  pointModule->GetView()->DeletePoint();
  Fl::check();

  pointModule->GetView()->guiAdd->do_callback();
  Fl::check();

  pointModule->GetView()->lPointList->value(1);
  pointModule->GetView()->guiFocusPoint->do_callback();
  Fl::check();

  pointModule->GetView()->guiComputeTransform->do_callback();
  Fl::check();

  pointModule->GetView()->guiQuit->do_callback();

  otb::DataObjectWrapper wrapperOut = module->GetOutputByKey("Transformed Image");
  ImageType::Pointer outImage = dynamic_cast<ImageType *>(wrapperOut.GetDataObject());

    //Write the image
  WriterType::Pointer writer = WriterType::New();
  writer->SetInput(outImage);
  writer->SetFileName(argv[3]);
  writer->Update();

 return EXIT_SUCCESS;
}
