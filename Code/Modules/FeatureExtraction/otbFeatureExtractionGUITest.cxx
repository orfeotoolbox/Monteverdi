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

#include "itkExceptionObject.h"
#include "otbFeatureExtractionController.h"
#include "otbFeatureExtractionViewGUI.h"
#include "otbImageFileReader.h"
#include "otbCommandLineArgumentParser.h"
#include "otbMsgReporter.h"

#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>

int main(int argc, char* argv[])
{
  // Parse command line parameters
  typedef otb::CommandLineArgumentParser ParserType;
  ParserType::Pointer parser = ParserType::New();

  parser->AddInputImage(false); //Optionnal parameter

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
      std::cout << "WARNING : output file pixels are converted in 'unsigned char'" << std::endl;
      return EXIT_SUCCESS;
    }
    if (descriptionException.find("ParseCommandLine(): Version Parser") != std::string::npos)
    {
      return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
  }

  typedef otb::FeatureExtractionController ControllerType;
  typedef otb::FeatureExtractionViewGUI ViewType;

  ControllerType::Pointer controller = ControllerType::New();
  ViewType::Pointer view = ViewType::New();
  controller->SetView(view);
  view->SetFeatureExtractionController(controller);
  otb::MsgReporter::GetInstance()->SetTitle("Feature Extraction application");

/** NewVisu */
  view->InitWidgets();
  view->Show();

  if ( parseResult->IsOptionInputImagePresent() )
    view->GetFeatureExtractionController()->OpenInputImage(parseResult->GetInputImage().c_str());

  Fl::check();

  otbGenericMsgDebugMacro(<<"Running GUI ...");
  Fl::run();



  return EXIT_SUCCESS;

}
