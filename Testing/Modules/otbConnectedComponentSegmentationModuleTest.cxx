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

#include "otbConnectedComponentSegmentationModule.h"

#include "otbImage.h"
#include "otbVectorImage.h"
#include "otbImageFileReader.h"
#include "otbVectorDataFileWriter.h"
#include "otbImageFileWriter.h"



int otbConnectedComponentSegmentationModuleTest(int argc, char* argv[])
{
  otb::ConnectedComponentSegmentationModule::Pointer specificModule = otb::ConnectedComponentSegmentationModule::New();
  otb::Module::Pointer            module = specificModule.GetPointer();

  std::cout << "Module: " << module << std::endl;


  // Put in the tests
  const char * infname  = argv[1];
  const char * outvdname   = argv[2];
  const char * outfname   = argv[3];

  typedef otb::ConnectedComponentSegmentationModule::VectorImageType       VectorImageType;
  typedef otb::ConnectedComponentSegmentationModule::VectorDataType  VectorDataType;
  typedef otb::ConnectedComponentSegmentationModule::LabelImageType  LabelImageType;

 
  typedef otb::ImageFileReader<VectorImageType>           ImageReaderType;
  typedef otb::VectorDataFileWriter<VectorDataType>       VectorDataWriterType;
  typedef otb::ImageFileWriter<LabelImageType>      ImageWriterType;

  // Image reader
  ImageReaderType::Pointer reader = ImageReaderType::New();
  reader->SetFileName(infname);
  reader->GenerateOutputInformation();

  // Add Wrapper Input
   otb::DataObjectWrapper wrapperIn = otb::DataObjectWrapper::Create(reader->GetOutput());
   std::cout << "Input wrapper: " << wrapperIn << std::endl;
   module->AddInputByKey("InputImage", wrapperIn);

   module->Start();



   Fl::check();


  // Simulate Operation
  specificModule->ui_MaskExpression->value("intensity > 50");
  specificModule->ui_CCExpression->value("distance < 15");
  specificModule->ui_OBIAExpression->value("SHAPE_Elongation > 5");

  specificModule->uiMinSize->value(15);
  specificModule->uiTmpOutputSelection->value(OUTPUT);
  
  Fl::check();

  // Fl::run();
  // Exit the GUI and save the result
  specificModule->ui_Update->do_callback();

  specificModule->ui_Ok->do_callback();

  otb::DataObjectWrapper vdWrapperOut = module->GetOutputByKey("OutputVectorData");
  otb::DataObjectWrapper imageWrapperOut = module->GetOutputByKey("OutputLabelImage");


  std::cout << "Output wrappers: " << vdWrapperOut << " " << imageWrapperOut << std::endl;

  VectorDataType::Pointer outVectorData = dynamic_cast<VectorDataType *>(vdWrapperOut.GetDataObject());
  LabelImageType::Pointer outLabelImage = dynamic_cast<LabelImageType *>(imageWrapperOut.GetDataObject());
  
  //Write the vector data
  std::cout << "Write output vector data" << outvdname << std::endl;
  VectorDataWriterType::Pointer vdWriter = VectorDataWriterType::New();
  vdWriter->SetFileName(outvdname);
  vdWriter->SetInput(outVectorData);
  vdWriter->Update();

  //Write the vector data
   std::cout << "Write output image" << outfname << std::endl;
   ImageWriterType::Pointer imageWriter = ImageWriterType::New();
   imageWriter->SetFileName(outfname);
   imageWriter->SetInput(outLabelImage);
   imageWriter->Update();
  return EXIT_SUCCESS;

}
