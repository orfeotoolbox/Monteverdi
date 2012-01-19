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
#include "otbObjectLabelingModule.h"

#include "otbImageFileReader.h"
#include "otbImageFileWriter.h"

/**
 * ObjectLabeling Open Image
 */
int otbObjectLabelingColoredOutput(int argc, char* argv[])
{
  // Input
  const char * inputFilename = argv[1];
  const char * segmentedInputFilename = argv[2];

  unsigned int x1 = atoi(argv[3]);
  unsigned int y1 = atoi(argv[4]);

  unsigned int x2 = atoi(argv[5]);
  unsigned int y2 = atoi(argv[6]);

  unsigned int x3 = atoi(argv[7]);
  unsigned int y3 = atoi(argv[8]);

  unsigned int x4 = atoi(argv[9]);
  unsigned int y4 = atoi(argv[10]);

  const char * labeledOutputFilename = argv[11];
  bool run = atoi(argv[12]);
  // Open Image
  typedef otb::TypeManager::Floating_Point_VectorImage ImageType;
  typedef otb::TypeManager::Floating_Point_Image LabeledImageType;
  typedef otb::ImageFileReader<ImageType> ReaderType;
  typedef otb::ImageFileReader<LabeledImageType> LabeledReaderType;
  typedef otb::ImageFileWriter<ImageType> WriterType;

  ReaderType::Pointer imageReader = ReaderType::New();
  LabeledReaderType::Pointer segmentedImageReader = LabeledReaderType::New();

  // Input Image
  imageReader->SetFileName(inputFilename);
  imageReader->GenerateOutputInformation();
  // Segmented Image
  segmentedImageReader->SetFileName(segmentedInputFilename);
  segmentedImageReader->GenerateOutputInformation();

  // Input Wrapper
  otb::DataObjectWrapper wrapperIn = otb::DataObjectWrapper::Create(imageReader->GetOutput());
  std::cout << "Input wrapper : " << wrapperIn << std::endl << std::endl;

  // Segmented Input Wrapper
  otb::DataObjectWrapper wrapperSegmentedIn = otb::DataObjectWrapper::Create(segmentedImageReader->GetOutput());
  std::cout << "Segmented Input wrapper : " << wrapperSegmentedIn << std::endl << std::endl;

  // Object Labeling Module
  otb::ObjectLabelingModule::Pointer objectLabelingModule = otb::ObjectLabelingModule::New();
  otb::Module::Pointer olabelingModule = objectLabelingModule.GetPointer();
  std::cout << "Module : " << olabelingModule << std::endl << std::endl;

  // Configure ObjectLabeling  module
  olabelingModule->AddInputByKey("InputImage", wrapperIn);
  olabelingModule->AddInputByKey("LabeledImage", wrapperSegmentedIn);
  olabelingModule->Start();

  // Add Two classes
  objectLabelingModule->GetView()->bAddClass->do_callback();
  objectLabelingModule->GetView()->bAddClass->do_callback();

  // Select the first class
  objectLabelingModule->GetView()->lClasses->value(1);
  objectLabelingModule->GetView()->lClasses->do_callback();

  // Simulate clicks for the polygons of the first class
  ImageType::IndexType index1, index2;
  index1[0] = x1;
  index1[1] = y1;

  index2[0] = x2;
  index2[1] = y2;

  objectLabelingModule->GetModel()->IndexClicked(index1);
  objectLabelingModule->GetModel()->IndexClicked(index1);
  objectLabelingModule->GetModel()->IndexClicked(index2);
  objectLabelingModule->GetModel()->IndexClicked(index2);

  Fl::check();

  // Select the second class
  objectLabelingModule->GetView()->lClasses->value(2);
  objectLabelingModule->GetView()->lClasses->do_callback();

  // Simulate clicks for the polygons of the second class
  ImageType::IndexType index3, index4;
  index3[0] = x3;
  index3[1] = y3;

  index4[0] = x4;
  index4[1] = y4;

  objectLabelingModule->GetModel()->IndexClicked(index3);
  objectLabelingModule->GetModel()->IndexClicked(index3);
  objectLabelingModule->GetModel()->IndexClicked(index4);
  objectLabelingModule->GetModel()->IndexClicked(index4);

  if (run)
    {
    Fl::run();
    }
  else
    {
    Fl::check();

    // Learn
    objectLabelingModule->GetView()->bALStart->do_callback();
    Fl::check();

    // Exit the module
    objectLabelingModule->GetView()->bexit->do_callback();
    Fl::check();

    // Get The output
    otb::DataObjectWrapper wrapperOut = olabelingModule->GetOutputByKey("ColoredOutput");

    std::cout << "Output wrapper: " << wrapperOut << std::endl;

    ImageType::Pointer outImage = dynamic_cast<ImageType *> (wrapperOut.GetDataObject());

    //Write the image
    WriterType::Pointer writer = WriterType::New();
    writer->SetFileName(labeledOutputFilename);
    writer->SetInput(outImage);
    writer->Update();
    }
  return EXIT_SUCCESS;
}
