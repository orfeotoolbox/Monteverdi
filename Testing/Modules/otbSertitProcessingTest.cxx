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

#include "otbExtractROIModule.h"
#include "otbTypeManager.h"
#include "otbGCPToSensorModelModule.h"
#include "otbTileExportModule.h"

#include "otbVectorImage.h"
#include "otbImageFileReader.h"
#include "otbImageFileWriter.h"

/**
 * Sertit Processing Test
 */
int otbSertitProcessingTest(int argc, char* argv[])
{
  // Input
  const char * inputFilename  = argv[1];
  const char * GCPfilename    = argv[2];
  const char * outputFilename = argv[3];
  unsigned int useExtended    = atoi(argv[4]);
  unsigned int startx         = atoi(argv[5]);
  unsigned int starty         = atoi(argv[6]);
  unsigned int sizex          = atoi(argv[7]);
  unsigned int sizey          = atoi(argv[8]);
  unsigned int startxl        = atoi(argv[9]);
  unsigned int startyl        = atoi(argv[10]);
  unsigned int sizexl         = atoi(argv[11]);
  unsigned int sizeyl         = atoi(argv[12]);

  // Open Image
  typedef otb::TypeManager::Floating_Point_VectorImage ImageType;
  typedef otb::ImageFileReader<ImageType>              ReaderType;
  typedef otb::ImageFileWriter<ImageType>              WriterType;

  ReaderType::Pointer imageReader = ReaderType::New();
  imageReader->SetFileName(inputFilename);
  imageReader->GenerateOutputInformation();

  // Input Wrapper
  otb::DataObjectWrapper wrapperIn = otb::DataObjectWrapper::Create(imageReader->GetOutput());
  std::cout << "Input wrapper : " << wrapperIn << std::endl << std::endl;

  // Extract Legend
  otb::ExtractROIModule::Pointer extractLegend = otb::ExtractROIModule::New();
  otb::Module::Pointer           extractLModule = extractLegend.GetPointer();
  std::cout << "Module : " << extractLModule << std::endl << std::endl;

  // Configure extract legend
  extractLegend->AddInputByKey("InputImage", wrapperIn);
  extractLegend->Start();
  extractLegend->vStartX->value(startxl);
  extractLegend->vStartY->value(startyl);
  extractLegend->vSizeX->value(sizexl);
  extractLegend->vSizeY->value(sizeyl);
  Fl::check();

  // Launch extract ROI module
  extractLegend->bOk->do_callback();
  Fl::check();

  // Output wrapper
  otb::DataObjectWrapper wrapperExtractLegend = extractLegend->GetOutputByKey("OutputImage");
  std::cout << "Extract ROI output wrapper : " << wrapperExtractLegend << std::endl << std::endl;

  // Extract ROI module
  otb::ExtractROIModule::Pointer extractROImodule = otb::ExtractROIModule::New();
  otb::Module::Pointer           extractModule = extractROImodule.GetPointer();
  std::cout << "Module : " << extractModule << std::endl << std::endl;

  // Configure extract ROI module
  extractROImodule->AddInputByKey("InputImage", wrapperIn);
  extractROImodule->Start();
  extractROImodule->vStartX->value(startx);
  extractROImodule->vStartY->value(starty);
  extractROImodule->vSizeX->value(sizex);
  extractROImodule->vSizeY->value(sizey);
  Fl::check();

  // Launch extract ROI module
  extractROImodule->bOk->do_callback();
  Fl::check();

  // Output wrapper
  otb::DataObjectWrapper wrapperExtractOut = extractROImodule->GetOutputByKey("OutputImage");
  std::cout << "Extract ROI output wrapper : " << wrapperExtractOut << std::endl << std::endl;

  // GCP to Sensor Model Module
  otb::GCPToSensorModelModule::Pointer gcpToSensorModelModule = otb::GCPToSensorModelModule::New();
  otb::Module::Pointer                 GCPModule = gcpToSensorModelModule.GetPointer();
  std::cout << "Module : " << GCPModule << std::endl << std::endl;

  // Configure GCP To Sensor Model module
  gcpToSensorModelModule->AddInputByKey("InputImage", wrapperExtractOut);
  gcpToSensorModelModule->Start();

  // Import GCP from XML file
  gcpToSensorModelModule->GetController()->ImportGcpsFromXmlFile(GCPfilename);
  Fl::check();

  // Launch GCP To sensor Model module
  gcpToSensorModelModule->GetView()->bSave->do_callback();
  Fl::check();

  // Output wrapper
  otb::DataObjectWrapper wrapperGCPToSensorModuleOut = gcpToSensorModelModule->GetOutputByKey("OutputImage");
  std::cout << "GCPToSensorModel module output wrapper : " << wrapperGCPToSensorModuleOut << std::endl << std::endl;

  // GE Format Exportation Module
  otb::TileExportModule::Pointer geFormatExportationModule = otb::TileExportModule::New();
  otb::Module::Pointer           exportationModule  = geFormatExportationModule.GetPointer();
  std::cout << "Module: " << exportationModule << std::endl << std::endl;

  // Configure GE Format Exportation Module
  geFormatExportationModule->AddInputByKey("InputImage", wrapperGCPToSensorModuleOut);
  geFormatExportationModule->AddInputByKey("InputLegend", wrapperExtractLegend);
  geFormatExportationModule->Start();

  geFormatExportationModule->vFilePath->value(outputFilename);
  geFormatExportationModule->gExtended->value(useExtended);
  Fl::check();

  geFormatExportationModule->bOk->do_callback();
  Fl::check();

  return EXIT_SUCCESS;
}
