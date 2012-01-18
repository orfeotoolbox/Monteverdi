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

#include "otbUncompressJpeg2000Module.h"
#include "otbReaderModule.h"

int otbUncompressJpeg2000ModuleTest(int argc, char* argv[])
{
  // Load a jpeg2000 image
  otb::ReaderModule::Pointer specificModuleReader = otb::ReaderModule::New();
  otb::Module::Pointer       moduleReader = specificModuleReader.GetPointer();

  // for multithreading
  Fl::lock();
  
  moduleReader->Start();
  Fl::check();

  // Simulate file chooser and ok callback
  specificModuleReader->vFilePath->value(argv[1]);
  specificModuleReader->vFilePath->do_callback();
  specificModuleReader->vName->value("Input");
  specificModuleReader->bSaveQuicklook->value(0);
  specificModuleReader->bOk->do_callback();
  Fl::check();
  
  // wait for the end of quicklook generation
  Fl::run();

  // Load Uncompress Jpeg2000 module
  otb::UncompressJpeg2000Module::Pointer specificModule = otb::UncompressJpeg2000Module::New();
  otb::Module::Pointer           module = specificModule.GetPointer();

  std::cout << "Module: " << module << std::endl;

  otb::DataObjectWrapper wrapperIn = specificModuleReader->GetOutputByKey("Input");
  std::cout << "Input wrapper: " << wrapperIn << std::endl;

  module->AddInputByKey("InputImage", wrapperIn);
  module->Start();
  specificModule->vStartX->value(atoi(argv[3]));
  specificModule->vStartY->value(atoi(argv[4]));
  specificModule->vSizeX->value(atoi(argv[5]));
  specificModule->vSizeY->value(atoi(argv[6]));
  specificModule->vSizeY->do_callback();
  Fl::check();

  // Simulate Ok button callback
  specificModule->SetCheckFileExistance(false);
  specificModule->SetFilename( argv[2] );
  specificModule->bOk->do_callback();
  Fl::check();

  // Wait for the writer to complete
  Fl::run();

  return EXIT_SUCCESS;
}
