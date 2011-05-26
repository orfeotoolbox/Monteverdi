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
  otb::Module::Pointer module = specificModule.GetPointer();

  std::cout << "Module: " << module << std::endl;

  // Put in the tests
  const char * infname = argv[1];
  const char * outvdname = argv[2];
  bool run = atoi(argv[3]);

  const char * maskExpression = argv[4];
  const char * ccExpression = argv[5];
  const char * obiaExpression = argv[6];
  int minObjSize = atoi(argv[7]);
  const char * DEMDirectory;

  if (argc == 9)
    {
    DEMDirectory = argv[8];
    }
  else
    {
    DEMDirectory = NULL;
    }

  typedef otb::ConnectedComponentSegmentationModule::VectorImageType VectorImageType;
  typedef otb::ConnectedComponentSegmentationModule::VectorDataType VectorDataType;
  typedef otb::ConnectedComponentSegmentationModule::LabelImageType LabelImageType;

  typedef otb::ImageFileReader<VectorImageType> ImageReaderType;
  typedef otb::VectorDataFileWriter<VectorDataType> VectorDataWriterType;
  typedef otb::ImageFileWriter<LabelImageType> ImageWriterType;

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
  specificModule->ui_MaskExpression->value(maskExpression);
  specificModule->ui_CCExpression->value(ccExpression);
  specificModule->ui_OBIAExpression->value(obiaExpression);

  specificModule->LiveCheckMask();
  specificModule->LiveCheckCC();
  specificModule->LiveCheckOBIA();

  specificModule->uiMinSize->value(minObjSize);
  specificModule->uiTmpOutputSelection->value(otb::ConnectedComponentSegmentationModule::OUTPUT);

  if (!run)
    {
    Fl::check();
    }
  else
    {
    Fl::run();
    }

  // Exit the GUI and save the result
  ///specificModule->ui_Update->do_callback();

  // test DEMDictory

  if (DEMDirectory != NULL)
    {
    specificModule->SetUseDEM(true);
    specificModule->SetDEMPath(DEMDirectory);
    }

  specificModule->ui_Ok->do_callback();

  otb::DataObjectWrapper vdWrapperOut = module->GetOutputByKey("OutputVectorData");

  std::cout << "Output wrappers: " << vdWrapperOut << std::endl;
  VectorDataType::Pointer outVectorData = dynamic_cast<VectorDataType *> (vdWrapperOut.GetDataObject());

  //Write the vector data
  std::cout << "Write output vector data" << outvdname << std::endl;
  VectorDataWriterType::Pointer vdWriter = VectorDataWriterType::New();
  vdWriter->SetFileName(outvdname);
  vdWriter->SetInput(outVectorData);
  vdWriter->Update();

  return EXIT_SUCCESS;

}
