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

/**
 * ObjectLabeling Open Image
 */
int otbObjectLabelingOpenImage(int argc, char* argv[])
{
  // Input
  const char * inputFilename  = argv[1];
  const char * segmentedInputFilename  = argv[2];
  bool         run = atoi(argv[3]);
  
  // Open Image
  typedef otb::TypeManager::Floating_Point_VectorImage ImageType;
  typedef otb::TypeManager::Floating_Point_Image       LabeledImageType;
  typedef otb::ImageFileReader<ImageType>              ReaderType;
  typedef otb::ImageFileReader<LabeledImageType>       LabeledReaderType;

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
  otb::Module::Pointer      olabelingModule    = objectLabelingModule.GetPointer();
  std::cout << "Module : " << olabelingModule << std::endl << std::endl;

  // Configure module
  olabelingModule->AddInputByKey("InputImage", wrapperIn);
  olabelingModule->AddInputByKey("LabeledImage", wrapperSegmentedIn );
  olabelingModule->Start();


  if (run)
    {
    Fl::run();
    }
  else
    {
    Fl::check();

    // Exit the module
    objectLabelingModule->GetView()->bexit->do_callback();
    Fl::check();
    }


  return EXIT_SUCCESS;
}
