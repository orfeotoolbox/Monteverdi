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


#include "otbOpticalCalibrationModule.h"
#include "otbImageFileReader.h"
#include "otbImageFileWriter.h"

int otbOpticalCalibrationModuleTest(int argc, char* argv[])
{
  bool runIt = false;
  if (atoi(argv[2]) == 1) runIt = true;
  //Internationalization
  

  otb::OpticalCalibrationModule::Pointer pointModule = otb::OpticalCalibrationModule::New();
  otb::Module::Pointer                   module = pointModule.GetPointer();

  // Put in the tests
  typedef otb::OpticalCalibrationModule::ImageType ImageType;
  typedef otb::ImageFileReader<ImageType>          ReaderType;
  typedef otb::ImageFileWriter<ImageType>          WriterType;

  //reader
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName(argv[1]);
  reader->GenerateOutputInformation();

  otb::DataObjectWrapper wrapperIn = otb::DataObjectWrapper::Create(reader->GetOutput());

  module->AddInputByKey("InputImage", wrapperIn);

  module->Start();

  Fl::check();

  if (runIt && pointModule->CheckMetadata())
    {
    pointModule->guiAerosolModel->value(2);
    Fl::check();
    pointModule->bChangeScale->value(1);
    Fl::check();
    pointModule->bSaveQuit->do_callback();

    WriterType::Pointer writer = WriterType::New();
    writer->SetInput(pointModule->GetDifferenceFilter()->GetOutput());
    writer->SetFileName(argv[3]);
    writer->Update();
    }

  Fl::check();

  return EXIT_SUCCESS;
}
