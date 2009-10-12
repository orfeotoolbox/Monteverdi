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

#include "otbCommandLineArgumentParser.h"
#include "otbHomologousPointExtractionModule.h"


int main(int argc, char* argv[])
{
  //Internationalization
  otbI18nMacro();

  // Parse command line parameters
  typedef otb::CommandLineArgumentParser           ParserType;
  typedef otb::CommandLineArgumentParseResult      ParserResultType;

  ParserType::Pointer        parser      = ParserType::New();
  ParserResultType::Pointer  parseResult = ParserResultType::New();
  //parser->AddInputImage();
  parser->AddOption("--FirstInputImage", "First Input Image.","-in1",true);
  parser->AddOption("--SecondInputImage", "Second Input Image.","-in2",true);
  //parser->AddOption("--Output", "Image Name where Output Image will be written.","-out",true);
    
  try
    {
      parser->ParseCommandLine(argc,argv,parseResult);
    }
  catch(itk::ExceptionObject & err)
    {
      std::string descriptionException = err.GetDescription();
      if(descriptionException.find("ParseCommandLine(): Help Parser") != std::string::npos)
       {
         return EXIT_SUCCESS;
       }
      if(descriptionException.find("ParseCommandLine(): Version Parser") != std::string::npos)
       {
         return EXIT_SUCCESS;
       }
      return EXIT_FAILURE;
    }


    
  // Convenient typedefs
  typedef otb::VectorImage<double,2>             InputImageType;
  typedef otb::ImageFileReader<InputImageType>   ReaderType;
  typedef otb::ImageFileWriter<InputImageType>   WriterType;

  otb::HomologousPointExtractionModule::Pointer pointModule = otb::HomologousPointExtractionModule::New();
  otb::Module::Pointer module = pointModule.GetPointer();
  
  std::cout<<"Module: "<<pointModule<<std::endl;

  // Put in the tests
  typedef otb::VectorImage<double,2>  ImageType;
  typedef otb::ImageFileReader<ImageType>     ReaderType;
  typedef otb::ImageFileWriter<ImageType>     WriterType;


  //reader
  ReaderType::Pointer reader1 = ReaderType::New();
  ReaderType::Pointer reader2 = ReaderType::New();
  reader1->SetFileName(parseResult->GetParameterString("--FirstInputImage"));
  reader1->GenerateOutputInformation();
  reader2->SetFileName(parseResult->GetParameterString("--SecondInputImage"));
  reader2->GenerateOutputInformation();

  otb::DataObjectWrapper wrapperIn1 = otb::DataObjectWrapper::Create(reader1->GetOutput());
  otb::DataObjectWrapper wrapperIn2 = otb::DataObjectWrapper::Create(reader2->GetOutput());
  std::cout<<"Input wrapper 1: "<<wrapperIn1<<std::endl;
  std::cout<<"Input wrapper 3: "<<wrapperIn2<<std::endl;

  module->AddInputByKey("FirstInputImage",wrapperIn1);
  module->AddInputByKey("SecondInputImage",wrapperIn2);

  module->Start();
 
  Fl::check();
}
