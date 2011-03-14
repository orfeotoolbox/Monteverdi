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

#include "otbRasterizationModule.h"

#include "otbImage.h"
#include "otbVectorImage.h"
#include "otbImageFileReader.h"
#include "otbVectorDataFileReader.h"
#include "otbImageFileWriter.h"



int otbRasterizationModuleTest(int argc, char* argv[])
{
  otb::RasterizationModule::Pointer specificModule = otb::RasterizationModule::New();
  otb::Module::Pointer            module = specificModule.GetPointer();

  std::cout << "Module: " << module << std::endl;


  // Put in the tests
  const char * infname  = argv[1];
  const char * vdname   = argv[2];
  const char * outfname = argv[3];

  typedef otb::RasterizationModule::ImageType       ImageType;
  typedef otb::RasterizationModule::VectorDataType  VectorDataType;
  typedef otb::ImageFileReader<ImageType>           ImageReaderType;
  typedef otb::VectorDataFileReader<VectorDataType> VectorDataReaderType;
  typedef otb::ImageFileWriter<ImageType>           ImageWriterType;


  // Vector data reader
  VectorDataReaderType::Pointer vdReader = VectorDataReaderType::New();
  vdReader->SetFileName(vdname);
  vdReader->Update();

  // Image reader
  ImageReaderType::Pointer reader = ImageReaderType::New();
  reader->SetFileName(infname);
  reader->GenerateOutputInformation();

  // Add Wrapper Input
   otb::DataObjectWrapper wrapperIn = otb::DataObjectWrapper::Create(reader->GetOutput());
   std::cout << "Input wrapper: " << wrapperIn << std::endl;
   module->AddInputByKey("InputImage", wrapperIn);

   otb::DataObjectWrapper vdWrapperIn = otb::DataObjectWrapper::Create(vdReader->GetOutput());
   std::cout << "Input wrapper: " << vdWrapperIn << std::endl;
   module->AddInputByKey("VectorData", vdWrapperIn);


   module->Start();

  otb::DataObjectWrapper wrapperOut = module->GetOutputByKey("OutputImage");

  std::cout << "Output wrapper: " << wrapperOut << std::endl;

  ImageType::Pointer outImage = dynamic_cast<ImageType *>(wrapperOut.GetDataObject());

  //Write the image
  std::cout << "Write output image " << outfname << std::endl;
  ImageWriterType::Pointer writer = ImageWriterType::New();
  writer->SetFileName(outfname);
  writer->SetInput(outImage);
  writer->Update();

  return EXIT_SUCCESS;

}
