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

#include "otbTypeManager.h"
#include "otbExtractROIModule.h"
#include "otbGCPToSensorModelModule.h"
#include "otbTileExportModule.h"

#include "otbVectorImage.h"
#include "otbImageFileReader.h"
#include "otbImageFileWriter.h"

/**
 * Sertit Processing Test
 */ 
int otbSertitProcessingTestNonGeoProductXT(int argc, char* argv[])
{
  // Input
  const char * outputFilename = argv[1];
  unsigned int useExtended    = atoi(argv[2]);
  
  const char * inputFilename1 = argv[3];

  // Open Image
  typedef otb::TypeManager::Floating_Point_VectorImage      ImageType;
  typedef otb::ImageFileReader<ImageType>                   ReaderType;
  typedef otb::ImageFileWriter<ImageType>                   WriterType;

/** READER 1 */
  ReaderType::Pointer imageReader1 = ReaderType::New();
  imageReader1->SetFileName(inputFilename1);
  imageReader1->GenerateOutputInformation();
  
  // Input Wrapper 1
  otb::DataObjectWrapper wrapperIn1 = otb::DataObjectWrapper::Create(imageReader1->GetOutput());
  std::cout << "Input wrapper : " << wrapperIn1 << std::endl << std::endl;
  
  
/** GE Format Exportation Module */
  otb::TileExportModule::Pointer geFormatExportationModule = otb::TileExportModule::New();
  otb::Module::Pointer exportationModule  = geFormatExportationModule.GetPointer();
  std::cout << "Module: " << exportationModule << std::endl << std::endl;

  // Configure GE Format Exportation Module
  geFormatExportationModule->AddInputByKey("InputImage", wrapperIn1);
  geFormatExportationModule->Start();
  
  geFormatExportationModule->vFilePath->value(outputFilename);
  geFormatExportationModule->gExtended->value(useExtended);

  // Add some the latlongBox coordinates
  geFormatExportationModule->vgxGELongUL->value(-72.445128);
  geFormatExportationModule->vgxGELatUL->value(18.646685);

  geFormatExportationModule->vgxGELongUR->value(-72.207388);
  geFormatExportationModule->vgxGELatUR->value(18.643636);
  
  geFormatExportationModule->vgxGELongLL->value(-72.446857);
  geFormatExportationModule->vgxGELatLL->value(18.486938);

  geFormatExportationModule->vgxGELongLR->value(-72.208712);
  geFormatExportationModule->vgxGELatLR->value(18.484373);
  geFormatExportationModule->vGELatLR->do_callback();
    
  Fl::run();
  
  geFormatExportationModule->bOk->do_callback();
  Fl::check();
  
  return EXIT_SUCCESS;
}

