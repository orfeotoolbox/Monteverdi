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
#include "ConfigureMonteverdi.h"

// For internationalisation
#include "otbI18n.h"

#include "otbOpticCalibrationModule.h"
#include "otbImageFileReader.h"
#include "otbImageFileWriter.h"

int otbOpticCalibrationModuleTest(int argc, char* argv[])
{
  bool runIt = false;
  if(atoi(argv[2]) == 1)
    runIt = true;
  //Internationalization
  otbI18nMacro();
  
  // Convenient typedefs
  typedef otb::VectorImage<double,2>             InputImageType;
  typedef InputImageType::IndexType              IndexType;
  typedef otb::ImageFileReader<InputImageType>   ReaderType;
  typedef otb::ImageFileWriter<InputImageType>   WriterType;

  otb::OpticCalibrationModule::Pointer pointModule = otb::OpticCalibrationModule::New();
  otb::Module::Pointer module = pointModule.GetPointer();
  
  // Put in the tests
  typedef otb::VectorImage<double,2>      ImageType;
  typedef otb::ImageFileReader<ImageType> ReaderType;
  typedef otb::ImageFileWriter<ImageType> WriterType;


  //reader
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName(argv[1]);
  reader->GenerateOutputInformation();

  otb::DataObjectWrapper wrapperIn = otb::DataObjectWrapper::Create(reader->GetOutput());
 
  module->AddInputByKey("InputImage",wrapperIn);

  module->Start();

  Fl::check();

  if(runIt && pointModule->GetController()->GetIsValidImage() )
    {  
      pointModule->GetView()->m_CoefSetupWindow->show();
      Fl::check();
      pointModule->GetView()->guiAerosolModel->value(2);
      Fl::check();
      pointModule->GetView()->guiCoefSetupOk->do_callback();
      Fl::check();
      pointModule->GetView()->bLuminance->do_callback();
      pointModule->GetView()->bReflectanceTOA->do_callback();
      pointModule->GetView()->bReflectanceTOC->do_callback();
      pointModule->GetView()->bDiffRefl->do_callback();
      pointModule->GetView()->bReflectanceScale->value(1);
      pointModule->GetView()->bReflectanceScale->do_callback();
      Fl::check();
      pointModule->GetView()->bSaveQuit->do_callback();

      WriterType::Pointer writer = WriterType::New();
      writer->SetInput( pointModule->GetModel()->GetDifferenceImage());
      writer->SetFileName(argv[3]);
      writer->Update();
    }

  Fl::check();

  return EXIT_SUCCESS;
}
