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

#include "otbGCPToSensorModelModule.h"

#include "otbImage.h"
#include "otbImageFileReader.h"
#include "otbImageFileWriter.h"
#include "otbCurlHelperInterface.h"

int otbGCPToSensorModelModuleTest(int argc, char* argv[])
{
  otb::GCPToSensorModelModule::Pointer gcpModule = otb::GCPToSensorModelModule::New();
  otb::Module::Pointer                 module = gcpModule.GetPointer();

  std::cout << "Module: " << module << std::endl;

  if (argc != 4)
    {
    return EXIT_FAILURE;
    }

  // Put in the tests
  const char * infname = argv[1];
  typedef otb::GCPToSensorModelModule::FloatingVectorImageType ImageType;
  typedef otb::ImageFileReader<ImageType>                      ReaderType;
  typedef otb::ImageFileWriter<ImageType>                      WriterType;

  //reader
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName(infname);
  reader->GenerateOutputInformation();
  //  std::cout<<reader->GetOutput()->GetImageKeywordlist()<<std::endl;
  otb::DataObjectWrapper wrapperIn = otb::DataObjectWrapper::Create(reader->GetOutput());

  module->AddInputByKey("InputImage", wrapperIn);
  module->Start();

  if (otb::CurlHelperInterface::IsCurlAvailable() )
    {
    // Simulate mapSearch
    // Set placename and update lat/long
    std::string parisFrance = "Paris 01 Louvre France";
    std::string paris = "Paris";

    gcpModule->GetView()->vMPlaceName->value(paris.c_str());
    Fl::check();

    gcpModule->GetView()->bMUpdateLonLat->do_callback();
    Fl::check();

    gcpModule->GetView()->bMUpdatePlaceName->do_callback();
    Fl::check();

    // Check result
    if (parisFrance != gcpModule->GetView()->vMPlaceName->value())
      {
        std::cout<<"WARNING, placename error..."<<std::endl;
        std::cout<<"Waiting for \""<<parisFrance<<"\" and received \""<<gcpModule->GetView()->vMPlaceName->value()<<"\"."<<std::endl;
      }

    gcpModule->GetView()->vMDepth->value(13);
    Fl::check();
    // Refresh map
    gcpModule->GetView()->bRefreshMap->do_callback();
    Fl::check();
    }

  // Simulate add point
  // P1
  gcpModule->GetView()->vX->value(3061);
  gcpModule->GetView()->vY->value(529);
  gcpModule->GetView()->vLong->value(0.072505);
  gcpModule->GetView()->vLat->value(43.212386);
  gcpModule->GetView()->vElev->value(1.);
  Fl::check();
  gcpModule->GetView()->guiAdd->do_callback();
  Fl::check();

  // Simulate focus click
  gcpModule->GetView()->guiFocusRound->do_callback();
  Fl::check();

  // P2
  gcpModule->GetView()->vX->value(2633);
  gcpModule->GetView()->vY->value(447);
  gcpModule->GetView()->vLong->value(0.017059);
  gcpModule->GetView()->vLat->value(43.218954);
  gcpModule->GetView()->vElev->value(2.);
  Fl::check();
  gcpModule->GetView()->guiAdd->do_callback();
  Fl::check();

  // Simulate Change Elevation mode to Mean value
  gcpModule->GetView()->guiDEM->do_callback();
  Fl::check();
  gcpModule->GetView()->cMean->value(1);
  gcpModule->GetView()->cMean->do_callback();
  Fl::check();
  gcpModule->GetView()->vMeanElev->value(15);
  gcpModule->GetView()->bOkDEM->do_callback();
  Fl::check();

  // Simulate Change Elevation mode to DEM
  gcpModule->GetView()->guiDEM->do_callback();
  Fl::check();
  gcpModule->GetView()->cDEM->value(1);
  gcpModule->GetView()->cDEM->do_callback();
  Fl::check();
  std::string DEMPath = argv[2];
  gcpModule->GetController()->SetDEMPath(DEMPath);
  gcpModule->GetView()->tDEMFile->value(argv[2]);
  gcpModule->GetView()->bOkDEM->do_callback();
  Fl::check();

  // Simulate export
  gcpModule->GetController()->ExportGcpsToXmlFile(argv[3]);
  Fl::check();

  // Simulate Focus
  gcpModule->GetView()->lPointList->value(1);
  Fl::check();
  gcpModule->GetView()->guiFocusPoint->do_callback();

  // Simulate delete point
  gcpModule->GetView()->guiDelete->do_callback();
  Fl::check();

  // Simulate Reload
  gcpModule->GetView()->guiReload->do_callback();
  Fl::check();

  // Simulate clear
  gcpModule->GetView()->guiClear->do_callback();
  Fl::check();

  // Simulate import
  gcpModule->GetController()->ImportGcpsFromXmlFile(argv[3]);
  Fl::check();

  // Simulate Ok button callback
  gcpModule->GetView()->bSave->do_callback();
  Fl::check();

  otb::DataObjectWrapper wrapperOut = module->GetOutputByKey("OutputImage");

  std::cout << "Output wrapper: " << wrapperOut << std::endl;

  ImageType::Pointer outImage = dynamic_cast<ImageType *>(wrapperOut.GetDataObject());

//  std::cout<<outImage->GetImageKeywordlist()<<std::endl;

  return EXIT_SUCCESS;

}
