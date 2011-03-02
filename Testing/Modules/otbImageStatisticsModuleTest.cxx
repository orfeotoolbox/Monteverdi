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

#include "otbImageStatisticsModule.h"

#include "otbImage.h"
#include "otbImageFileReader.h"
#include "otbImageFileWriter.h"


template<class TImageType>
int otbExtractROIModuleGenericTest(int argc, char* argv[])
{
  otb::ImageStatisticsModule::Pointer specificModule = otb::ImageStatisticsModule::New();
  otb::Module::Pointer           module = specificModule.GetPointer();
  
  std::cout << "Module: " << module << std::endl;
  
  typedef otb::ImageFileReader<TImageType>   ReaderType;
  
  // Put in the tests
  const char * infname = argv[2];
  
  //reader
  typename ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName(infname);
  reader->GenerateOutputInformation();

  otb::DataObjectWrapper wrapperIn = otb::DataObjectWrapper::Create(reader->GetOutput());
  std::cout << "Input wrapper: " << wrapperIn << std::endl;

  module->AddInputByKey("InputImage", wrapperIn);
  module->Start();
  
  // Simulate channel selection
  specificModule->vChannelId->value(1);
  Fl::check();

  // If multi channel
  if(atoi(argv[1]) != 0)
  {
	specificModule->vChannelId->value(2);
	Fl::check();
	specificModule->vChannelId->value(3);
	Fl::check();
  }

  //Compute full stat
  specificModule->bFullStat->do_callback();
  // If multi channel
  if(atoi(argv[1]) != 0)
  {
	specificModule->vChannelId->value(1);
	Fl::check();
	specificModule->vChannelId->value(3);
	Fl::check();
  }
  
  // Show covariance matrix
  specificModule->bCovMatrix->do_callback();
  Fl::check();

  // Hide covariance matrix window
  specificModule->bCloseCovWindow->do_callback();
  Fl::check();


  // Close all
  specificModule->bOk->do_callback();

  
  return EXIT_SUCCESS;
}

int otbImageStatisticsModuleTest(int argc, char* argv[])
{
  int res = EXIT_FAILURE;
  if( argv[0] == 0)
  {
	typedef otb::ImageStatisticsModule::ImageType ImageType;
	res = otbExtractROIModuleGenericTest<ImageType>(argc, argv);
  }
  else
  {
	typedef otb::ImageStatisticsModule::VectorImageType VectorImageType;
	res = otbExtractROIModuleGenericTest<VectorImageType>(argc, argv);
  }

  return res;
}
