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
#include "otbDEMToImageGeneratorModule.h"

#include "otbImage.h"
#include "otbImageFileReader.h"
#include "otbImageFileWriter.h"
#include "otbExtractROI.h"

int otbDEMToImageGeneratorModuleTest(int argc, char* argv[])
{
  otb::DEMToImageGeneratorModule::Pointer specificModule = otb::DEMToImageGeneratorModule::New();
  otb::Module::Pointer                   module = specificModule.GetPointer();

  std::cout << "Module: " << module << std::endl;

  // Put in the tests
  const char * demrep  = argv[1];

  typedef otb::DEMToImageGeneratorModule::InputImageType  ImageType;
  typedef ImageType::PixelType                            PixelType;
  typedef otb::ImageFileReader<ImageType>                 ReaderType;
  typedef otb::ImageFileWriter<ImageType>                 WriterType;
  typedef otb::ExtractROI<PixelType, PixelType>           ExtractType;

  otb::DataObjectWrapper wrapperIn;

  if(argc == 4)
    {
      const char * infname = argv[2];
      // reader
      ReaderType::Pointer reader = ReaderType::New();
      reader->SetFileName(infname);
      reader->GenerateOutputInformation();
      
      ExtractType::Pointer extract = ExtractType::New();
      extract->SetInput( reader->GetOutput() );
      ImageType::RegionType region;
      ImageType::SizeType   sz;
      ImageType::IndexType  id;

      sz.Fill(100);
      id.Fill(100);
      region.SetSize(sz);
      region.SetIndex(id);
      
      extract->SetExtractionRegion(region);
      extract->UpdateOutputInformation();
      
      // Add Wrapper Input
      wrapperIn = otb::DataObjectWrapper::Create(extract->GetOutput());
      std::cout << "Input wrapper: " << wrapperIn << std::endl;
      module->AddInputByKey("InputImage", wrapperIn);
      
      module->Start();
      
    }
  else if(argc == 9)
    {
      module->Start();
      
      specificModule->GetView()->oOriginX->value(argv[2]);
      specificModule->GetView()->oOriginY->value(argv[3]);
      specificModule->GetView()->oSizeX->value(argv[4]);
      specificModule->GetView()->oSizeY->value(argv[5]);
      specificModule->GetView()->oSpacingX->value(argv[6]);
      specificModule->GetView()->oSpacingY->value(argv[7]);

      Fl::check();
    }
 else
    {
      std::cout<<"Invalid input argument."<<std::endl;
      return EXIT_FAILURE;
    }
  
  // Simulate the update of the output parameters :
  specificModule->GetView()->iDEMPath->value(demrep);


  Fl::check();

  // Simulate Ok button callback
  specificModule->GetView()->guiOK->do_callback();

  // Refresh
  Fl::check();

  otb::DataObjectWrapper wrapperHillShading = module->GetOutputByKey("Hill Shading Image");

  std::cout << "Output HillShading: " << wrapperHillShading << std::endl;

  ImageType::Pointer outImage = dynamic_cast<ImageType *>(wrapperHillShading.GetDataObject());

  //Write the image
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName(argv[argc-1]);
  writer->SetInput(outImage);
  writer->Update();

  return EXIT_SUCCESS;

}
