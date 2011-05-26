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
#include "otbVectorizationModule.h"

#include "otbVectorImage.h"
#include "otbVectorDataFileReader.h"
#include "otbVectorDataFileWriter.h"
#include "otbImageFileReader.h"

int otbVectorizationModuleTest(int argc, char* argv[])
{
  otb::VectorizationModule::Pointer specificModule = otb::VectorizationModule::New();
  otb::Module::Pointer              module = specificModule.GetPointer();

  // Put in the tests
  const char * infname  = argv[1];
  const char * vdname   = argv[2];
  const char * outfname = argv[3];
  
  const char * DEMfname = "";
  if(argc>4)
    {
      DEMfname = argv[4];
    }

  typedef otb::VectorizationModule::FloatingVectorImageType ImageType;
  typedef otb::VectorizationModule::VectorDataType          VectorDataType;
  typedef otb::VectorDataFileReader<VectorDataType>         VDReaderType;
  typedef otb::VectorDataFileWriter<VectorDataType>         VDWriterType;
  typedef otb::ImageFileReader<ImageType>                   ReaderType;

  // Vector data reader
  VDReaderType::Pointer vdReader = VDReaderType::New();
  vdReader->SetFileName(vdname);
  vdReader->Update();

  // Image reader
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName(infname);
  reader->GenerateOutputInformation();

  // Add Wrapper Input image
  otb::DataObjectWrapper wrapperIn = otb::DataObjectWrapper::Create(reader->GetOutput());
  module->AddInputByKey("InputImage", wrapperIn);

  module->Start();
  
  if(argc>4)
    {
      // Add a DEM
      specificModule->GetModel()->SetUseDEM(true);
      specificModule->GetModel()->SetDEMPath(DEMfname);
    }
  

  // Add the vector data
  specificModule->GetController()->AddVectorData(vdReader->GetOutput());
  Fl::check();
  // simulated opacity
  specificModule->GetView()->vAlpha->value(1.);
  specificModule->GetView()->vAlpha->do_callback();
  Fl::check();
  // Simulated navigation mode
  specificModule->GetView()->vNavigationMode->value(1);
  specificModule->GetView()->vNavigationMode->do_callback();
  Fl::check();
  // Simulate polygon structure
  specificModule->GetView()->bLine->value(1);
  specificModule->GetView()->bLine->do_callback();
  Fl::check();
  specificModule->GetView()->bPoint->value(1);
  specificModule->GetView()->bPoint->do_callback();
  Fl::check();


  if(argc>5 && atoi(argv[5])==1)
    Fl::run();

  // Simulate Ok button callback
  specificModule->GetView()->bOK->do_callback();

  otb::DataObjectWrapper wrapperOut = module->GetOutputByKey("VectorData");

  std::cout << "Output wrapper: " << wrapperOut << std::endl;

  VectorDataType::Pointer outVD = dynamic_cast<VectorDataType *>(wrapperOut.GetDataObject());

  //Write the output vector data
  VDWriterType::Pointer writer = VDWriterType::New();
  writer->SetFileName(outfname);
  writer->SetInput(outVD);
  writer->Update();

  return EXIT_SUCCESS;

}
