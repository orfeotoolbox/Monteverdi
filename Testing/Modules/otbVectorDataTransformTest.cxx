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
#include "otbVectorDataTransformModule.h"

#include "otbVectorImage.h"
#include "otbImageFileReader.h"
#include "otbVectorDataFileReader.h"
#include "otbVectorDataFileWriter.h"

/**
 * Sertit Processing Test
 */
int otbVectorDataTransformTest(int argc, char* argv[])
{
  // Input
  const char * inputFilename           = argv[1];
  const char * inputVectorDataFileName = argv[2];
  const char * outputFilename          = argv[3];
  const char * translationX            = argv[4];
  const char * translationY            = argv[5];
  const char * rotationAngle           = argv[6];
  
  // Open Image
  typedef otb::TypeManager::Floating_Point_VectorImage ImageType;
  typedef otb::ImageFileReader<ImageType>              ReaderType;
  
  typedef otb::TypeManager::Vector_Data                VectorDataType;
  typedef otb::VectorDataFileReader<VectorDataType>    VdReaderType;
  typedef otb::VectorDataFileWriter<VectorDataType>    WriterType;

  ReaderType::Pointer imageReader = ReaderType::New();
  imageReader->SetFileName(inputFilename);
  imageReader->GenerateOutputInformation();

  // Input Wrapper
  otb::DataObjectWrapper wrapperIn = otb::DataObjectWrapper::Create(imageReader->GetOutput());
  std::cout << "Input wrapper : " << wrapperIn << std::endl << std::endl;

  // Open VectorData
  VdReaderType::Pointer vdreader = VdReaderType::New();
  vdreader->SetFileName(inputVectorDataFileName);
  vdreader->Update();

  // Input VectorData Wrapper
  otb::DataObjectWrapper wrapperVdIn = otb::DataObjectWrapper::Create(vdreader->GetOutput());
  std::cout << "Input wrapper : " << wrapperVdIn << std::endl << std::endl;
  
  //  VectorDataTransformModule module
  otb::VectorDataTransformModule::Pointer vdTransformModule = otb::VectorDataTransformModule::New();
  otb::Module::Pointer           vdtModule = vdTransformModule.GetPointer();
  std::cout << "Module : " <<vdtModule  << std::endl << std::endl;

  // Configure Vector Data Transform Module
  vdtModule->AddInputByKey("InputImage", wrapperIn);
  vdtModule->AddInputByKey("InputVectorData", wrapperVdIn);
  vdtModule->Start();
  
  vdTransformModule->GetView()->guiTranslationX->value(atof(translationX));
  vdTransformModule->GetView()->guiTranslationY->value(atof(translationY));
  vdTransformModule->GetView()->guiRotationAngle->value(atof(rotationAngle));
  vdTransformModule->GetView()->guiRotationAngle->do_callback();
  Fl::check();
  
  // Save the transformed VectorData
  vdTransformModule->GetView()->bSave->do_callback();
  Fl::check();

  // VectorData transform module output
  otb::DataObjectWrapper wrapperOut = vdtModule->GetOutputByKey("TransformedVectorData");
  std::cout << "Output wrapper: " << wrapperOut << std::endl;
  VectorDataType::Pointer transformedVd = dynamic_cast<VectorDataType *>(wrapperOut.GetDataObject());

  // Write the output
  WriterType::Pointer writer = WriterType::New();
  writer->SetInput(transformedVd);
  writer->SetFileName(outputFilename);
  writer->Update();
  
  return EXIT_SUCCESS;
}
