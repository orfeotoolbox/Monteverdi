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
#include "otbCommandLineArgumentParser.h"

#include "itkExceptionObject.h"
#include "otbVectorImage.h"
#include "otbImageFileWriter.h"
#include "otbImageFileReader.h"

//
#include "otbOrthorectification.h"


#include "otbI18n.h"

int main(int argc, char* argv[])
{
  otbI18nMacro();

  // Parse command line parameters
  typedef otb::CommandLineArgumentParser           ParserType;
  typedef otb::CommandLineArgumentParseResult      ParserResultType;

  ParserType::Pointer        parser      = ParserType::New();
  ParserResultType::Pointer  parseResult = ParserResultType::New();
  parser->AddInputImage();
  parser->AddOption("--Output", "Image Name where Output Image will be written.","-out",true);
    
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
  typedef otb::Orthorectification                       OrthoType;

  ReaderType::Pointer   reader = ReaderType::New();
  OrthoType::Pointer    rectif = OrthoType::New();
    
  //Read The input Image
  reader->SetFileName(parseResult->GetInputImage());
  reader->Update();
    
  // Edit The input Image
  rectif->SetInputImage(reader->GetOutput());
  rectif->Show();
  Fl::run();

  //Write the image
  WriterType::Pointer writer = WriterType::New();
  writer->SetInput(rectif->GetOutput());
  writer->SetFileName(parseResult->GetParameterString("--Output"));
  writer->Update();
    
    
  return EXIT_SUCCESS;
}
