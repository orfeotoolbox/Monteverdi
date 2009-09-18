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
#include <stdlib.h>
#include <iostream>
#include "itkExceptionObject.h"
#include "otbCommandLineArgumentParser.h"

#include "otbVectorImage.h"
#include "otbSupervisedClassificationAppli.h"
#include "otbImageFileReader.h"
#include "otbImageFileWriter.h"

#include "otbI18n.h"

int main( int argc, char * argv[] )
{
  otbI18nMacro();

  // Parse command line parameters
  typedef otb::CommandLineArgumentParser ParserType;
  ParserType::Pointer parser = ParserType::New();
  parser->AddOption("--InputImage","Input image file name","-in",1,false);
  //parser->AddOption("--InputModel","Input model file name","-m",1,false);
  //parser->AddOption("--InputROIsImage","ROIs image file name","-r",1,false);
  //parser->AddOption("--NumberOfClasses","Number of classes","-nc",1,false);


  typedef otb::CommandLineArgumentParseResult ParserResultType;
  ParserResultType::Pointer  parseResult = ParserResultType::New();
  try
  {
    parser->ParseCommandLine(argc,argv,parseResult);
  }
  catch ( itk::ExceptionObject & err )
  {
    std::string descriptionException = err.GetDescription();
    if (descriptionException.find("ParseCommandLine(): Help Parser") != std::string::npos)
    {
      return EXIT_SUCCESS;
    }
    if (descriptionException.find("ParseCommandLine(): Version Parser") != std::string::npos)
    {
      return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
  }

  typedef double                                      PixelType;
  typedef otb::VectorImage<PixelType,2>               ImageType;
  typedef otb::SupervisedClassificationAppli          AppliType;
  typedef otb::ImageFileReader<ImageType>             ReaderType;

  // instantiation
  AppliType::Pointer app = AppliType::New();

  //Reader
  ReaderType::Pointer  reader = ReaderType::New();
  reader->SetFileName(parseResult->GetParameterString("--InputImage"));
  reader->GenerateOutputInformation();
 
  // Add input image
  app->SetInputImage(reader->GetOutput());
  
  // Build the application 
  app->Build();
  app->LoadImage();
    
  // show the applicatin 
  app->Show();
  Fl::run();

  typedef otb::ImageFileWriter<AppliType::OverlayImageType>      WriterType;
  WriterType::Pointer    writer = WriterType::New();
  writer->SetFileName("/home2/otmane/outputClassif.tif");
  writer->SetInput( app->GetOutput());
  writer->Update();
  
  // show the app
  //   
  //   // build the app
  //   app->Build();
  //   // show the app
  //   app->Show();
  
  //   if (parseResult->IsOptionPresent("--InputImage"))
  //   {
  //     app->SetImageFileName(parseResult->GetParameterString("--InputImage").c_str());
  //     app->LoadImage();
  //   }
  //   if (parseResult->IsOptionPresent("--InputModel"))
  //   {
  //     app->SetModelFileName(parseResult->GetParameterString("--InputModel").c_str());
  //     app->LoadSVMModel();
  //   }
  //   if (parseResult->IsOptionPresent("--NumberOfClasses"))
  //   {
  //     for (unsigned int i = 0;i<parseResult->GetParameterUInt("--NumberOfClasses");i++)
  //     {
  //       app->AddClass();
  //     }
  //   }
  //   if (parseResult->IsOptionPresent("--InputROIsImage"))
  //   {
  //     app->SetROIsImageFileName(parseResult->GetParameterString("--InputROIsImage").c_str());
  //     app->LoadROIsImage();
  //   }
  
  //   // if not testing the app
  //   if (!parseResult->IsOptionOTBTestingPresent())
  //   {
  //     Fl::run();
  //   }

  return EXIT_SUCCESS;
}


