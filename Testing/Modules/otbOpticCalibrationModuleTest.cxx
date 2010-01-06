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

#include "otbOpticCalibrationModule.h"
#include "otbImageFileReader.h"
#include "otbImageFileWriter.h"

int otbOpticCalibrationModuleTest(int argc, char* argv[])
{
  //Internationalization
  otbI18nMacro();
  
  // Convenient typedefs
  typedef otb::VectorImage<double,2>             InputImageType;
  typedef InputImageType::IndexType              IndexType;
  typedef otb::ImageFileReader<InputImageType>   ReaderType;
  typedef otb::ImageFileWriter<InputImageType>   WriterType;

  otb::OpticCalibrationModule::Pointer pointModule = otb::OpticCalibrationModule::New();
  otb::Module::Pointer module = pointModule.GetPointer();
  
  // Put in the tests
  typedef otb::VectorImage<double,2>      ImageType;
  typedef otb::ImageFileReader<ImageType> ReaderType;
  typedef otb::ImageFileWriter<ImageType> WriterType;


  //reader
  ReaderType::Pointer reader1 = ReaderType::New();
  reader1->SetFileName(argv[1]);
  reader1->GenerateOutputInformation();

  otb::DataObjectWrapper wrapperIn1 = otb::DataObjectWrapper::Create(reader1->GetOutput());
 
  module->AddInputByKey("InputImage",wrapperIn1);

  module->Start();
  
  Fl::check();
 return EXIT_SUCCESS;
}
