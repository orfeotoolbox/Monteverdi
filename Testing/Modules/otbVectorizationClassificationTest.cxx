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
#include "otbSupervisedClassificationModule2.h"

#include "otbVectorImage.h"
#include "otbVectorDataFileReader.h"
#include "otbVectorDataFileWriter.h"
#include "otbImageFileReader.h"
#include "otbImageFileWriter.h"

int otbVectorizationClassification(int argc, char* argv[])
{
  otb::VectorizationModule::Pointer specificModule1 = otb::VectorizationModule::New();
  otb::Module::Pointer                      module1 = specificModule1.GetPointer();

  // Put in the tests
  const char * infname  = argv[1];
  const char * vdname   = argv[2];
  const char * tmpVdName= argv[3];
  const char * outfname = argv[4];

  const char * DEMfname;
  if(argc>5)
    {
      DEMfname = argv[5];
    }
  
  typedef otb::VectorizationModule::FloatingVectorImageType ImageType;
  typedef otb::VectorizationModule::VectorDataType          VectorDataType;
  typedef otb::ImageFileReader<ImageType>                   ReaderType;  
  typedef otb::VectorDataFileReader<VectorDataType>         VDReaderType;
  typedef otb::VectorDataFileWriter<VectorDataType>         VDWriterType;


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
  module1->AddInputByKey("InputImage", wrapperIn);

  module1->Start();

  std::cout << "##### Vectorization Module #####" << std::endl;
  if(argc>5)
    {
      // Add a DEM
      specificModule1->GetModel()->SetUseDEM(true);
      specificModule1->GetModel()->SetDEMPath(DEMfname);
    }

  // Add the vector data
  specificModule1->GetController()->AddVectorData(vdReader->GetOutput());
  Fl::check();
  // simulated opacity
  specificModule1->GetView()->vAlpha->value(1.);
  specificModule1->GetView()->vAlpha->do_callback();
  Fl::check();
  // Simulated navigation mode
  specificModule1->GetView()->vNavigationMode->value(1);
  specificModule1->GetView()->vNavigationMode->do_callback();
  Fl::check();
  // Simulate polygon structure
  specificModule1->GetView()->bLine->value(1);
  specificModule1->GetView()->bLine->do_callback();
  Fl::check();
  specificModule1->GetView()->bPoint->value(1);
  specificModule1->GetView()->bPoint->do_callback();
  Fl::check();
  // Simulate Ok button callback
  specificModule1->GetView()->bOK->do_callback();

  otb::DataObjectWrapper wrapperOut = module1->GetOutputByKey("VectorData");
  VectorDataType::Pointer outVD = dynamic_cast<VectorDataType *>(wrapperOut.GetDataObject());

  //Write the output vector data
  VDWriterType::Pointer writer = VDWriterType::New();
  writer->SetFileName(tmpVdName);
  writer->SetInput(outVD);
  writer->Update();


  
  otb::SupervisedClassificationModule2::Pointer specificModule2 = otb::SupervisedClassificationModule2::New();
  otb::Module::Pointer                                  module2 = specificModule2.GetPointer();

  typedef otb::ImageFileWriter<ImageType>            WriterType;

  // Vector data reader
  VDReaderType::Pointer vdReader2 = VDReaderType::New();
  vdReader2->SetFileName(tmpVdName);
  vdReader2->Update();
  otb::DataObjectWrapper wrapperVdIn = otb::DataObjectWrapper::Create(vdReader2->GetOutput()); 

  // Add Input Image
  module2->AddInputByKey("InputImage", wrapperIn);
  module2->AddInputByKey("VectorDataROIs", wrapperVdIn);
  Fl::check();
  module2->Start();
  std::cout << "##### Classification Module #####" << std::endl;

  specificModule2->GetView()->Train();
  specificModule2->GetView()->Validate();
  
  specificModule2->GetView()->Ok();
 

  otb::DataObjectWrapper wOut = module2->GetOutputByKey("OutputImage");
  ImageType::Pointer out = dynamic_cast<ImageType *>(wOut.GetDataObject());

  WriterType::Pointer LabelWriter = WriterType::New();
  LabelWriter->SetInput(out);
  LabelWriter->SetFileName(outfname);
  LabelWriter->Update();

  return EXIT_SUCCESS;

}
