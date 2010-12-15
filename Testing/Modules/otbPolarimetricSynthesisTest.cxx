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
#include "otbPolarimetricSynthesisModule.h"
#include "itkComplexToRealImageFilter.h"
#include "itkComplexToRealImageFilter.h"
#include "otbImageFileReader.h"

/**
 * ObjectLabeling Open Image
 */
int otbPolarimetricSynthesisTest(int argc, char* argv[])
{
  // Input
  const char * inputFilename  = argv[1];
  bool         run     = atoi(argv[2]);

  // Open Image
  typedef otb::TypeManager::Floating_Point_Complex_Image ComplexImageType;
  typedef otb::TypeManager::Floating_Point_Image         FloatImageType;
  typedef otb::ImageFileReader<ComplexImageType>         ReaderType;
  typedef itk::ComplexToRealImageFilter<ComplexImageType, FloatImageType> ComplexToRealImageType;
  typedef itk::ComplexToImaginaryImageFilter<ComplexImageType, FloatImageType> ComplexToImaginaryImageType;

  ReaderType::Pointer imageReader = ReaderType::New();
  
  // Input Image 
  imageReader->SetFileName(inputFilename);
  imageReader->GenerateOutputInformation();

  ComplexToRealImageType::Pointer imageReal = ComplexToRealImageType::New();
  imageReal->SetInput(imageReader->GetOutput());

  ComplexToImaginaryImageType::Pointer imageImaginary = ComplexToImaginaryImageType::New();
  imageImaginary->SetInput(imageReader->GetOutput());

  // Input Wrapper RealPart
  otb::DataObjectWrapper wrapperRealPart = otb::DataObjectWrapper::Create(imageReal->GetOutput());
  std::cout << "Input wrapper RealPart: " << wrapperRealPart << std::endl << std::endl;

  otb::DataObjectWrapper wrapperImagPart = otb::DataObjectWrapper::Create(imageImaginary->GetOutput());
  std::cout << "Input wrapper RealPart: " << wrapperImagPart << std::endl << std::endl;
  
  // Polarimetric Synthesis Module
  otb::PolarimetricSynthesisModule::Pointer polarimetricSynthesisModule = otb::PolarimetricSynthesisModule::New();
  otb::Module::Pointer      polarModule    = polarimetricSynthesisModule.GetPointer();
  std::cout << "Module : " << polarModule << std::endl << std::endl;

  // Configure module
  polarModule->AddInputByKey("InputImageHHReal", wrapperRealPart);
  polarModule->AddInputByKey("InputImageHVReal", wrapperImagPart);
  polarModule->Start();


  if (run)
    {
      Fl::run();
    }
  else
    {
      Fl::check();
      // Exit the module
      // Simulate OK click
      //polarModule->bOK->do_callback();

      polarimetricSynthesisModule->GetView()->SaveAndQuitCallback();
      Fl::check();
    }


  return EXIT_SUCCESS;
}
