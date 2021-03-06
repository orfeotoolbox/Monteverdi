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


#include "otbSarCalibrationModule.h"
#include "otbImageFileReader.h"
#include "otbImageFileWriter.h"

int otbSarCalibrationModuleTest(int argc, char* argv[])
{

  if (argc != 5)
    {
    std::cout << "Invalid Parameters" << std::endl;
    }

  bool withComplex = false;
  if (atoi(argv[2]) == 1)
    {
    withComplex = true;
    }

  //Internationalization
  

  otb::SarCalibrationModule::Pointer pointModule = otb::SarCalibrationModule::New();
  otb::Module::Pointer               module = pointModule.GetPointer();

  // Put in the tests
  typedef otb::SarCalibrationModule::ImageType        ImageType;
  typedef otb::SarCalibrationModule::ComplexImageType ComplexImageType;
  typedef otb::ImageFileReader<ImageType>             ReaderType;
  typedef otb::ImageFileReader<ComplexImageType>      ComplexReaderType;

  //reader
  ReaderType::Pointer        reader    = ReaderType::New();
  ComplexReaderType::Pointer cplxReader = ComplexReaderType::New();

  otb::DataObjectWrapper wrapperIn;
  if (!withComplex)
    {
    reader->SetFileName(argv[1]);
    reader->GenerateOutputInformation();
    wrapperIn = otb::DataObjectWrapper::Create(reader->GetOutput());
    }
  else
    {
    cplxReader->SetFileName(argv[1]);
    cplxReader->GenerateOutputInformation();
    wrapperIn = otb::DataObjectWrapper::Create(cplxReader->GetOutput());
    }

  bool isLinear = atoi(argv[3]);
  bool withNoise = atoi(argv[4]);

  module->AddInputByKey("InputImage", wrapperIn);

  module->Start();

  Fl::check();

  if (pointModule->CheckMetadata())
    {
    pointModule->bLin->value(isLinear);
    pointModule->bdB->value(!isLinear);
    pointModule->bEnableNoise->value(withNoise);
    pointModule->bEnableNoise->value(!withNoise);
    pointModule->vThresholdLogDisplay->value(-25);
    if ( pointModule->vThresholdLogDisplay->value() != -25 )
      return EXIT_FAILURE;

    Fl::check();
    pointModule->bOK->do_callback();
    Fl::check();
    }

  Fl::check();

  return EXIT_SUCCESS;
}
