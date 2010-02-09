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

#include "otbTileMapImportModule.h"

#include "otbVectorImage.h"
#include "otbImageFileWriter.h"

int otbTileMapImportModuleTest(int argc, char* argv[])
{
  otb::TileMapImportModule::Pointer specificModule = otb::TileMapImportModule::New();
  otb::Module::Pointer module = specificModule.GetPointer();
  
  std::cout<<"Module: "<<module<<std::endl;

  // Put in the tests
  const char * outfname = argv[1];
  std::string placename = argv[2];
  unsigned int sizeX = static_cast<unsigned int> (atoi(argv[3]));
  unsigned int sizeY = static_cast<unsigned int> (atoi(argv[4]));
  unsigned int depth = static_cast<unsigned int> (atoi(argv[5]));

  typedef otb::VectorImage<double,2>  ImageType;
  typedef otb::ImageFileWriter<ImageType>     WriterType;
  
  module->Start();
  specificModule->vSizeX->value(sizeX);
  specificModule->vSizeY->value(sizeY);
  specificModule->vPlacename->value(placename.c_str());
  specificModule->vDepth->value(depth - 1);
  Fl::check();
  
  // Simulate Search button callback
  specificModule->bSearch->do_callback();
  Fl::check();
  
  // Simulate Ok button callback
  specificModule->bOk->do_callback();
  Fl::check();

  otb::DataObjectWrapper wrapperOut = module->GetOutputByKey("OutputImage");

  std::cout<<"Output wrapper: "<<wrapperOut<<std::endl;

  ImageType::Pointer outImage = dynamic_cast<ImageType *>(wrapperOut.GetDataObject());

  if(outImage.IsNull())
  {
    std::cerr<<"Wrapper retrieved a null pointer."<<std::endl;
    return EXIT_FAILURE;
  }
  
  //Write the image
  WriterType::Pointer  writer = WriterType::New();
  writer->SetFileName(outfname);
  writer->SetInput(outImage);
  writer->Update();

  return EXIT_SUCCESS;

}

