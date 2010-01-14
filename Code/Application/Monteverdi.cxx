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

#include "otbMonteverdiModel.h"
#include "otbMonteverdiViewGUI.h"
#include "otbMonteverdiController.h"
#include "otbSplashScreen.h"
#include "otbI18n.h"

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_PNG_Image.H>

#include <string>
#include <ctime>
#include <iostream>




// There are function prototype conflits under cygwin between standard w32 API
// and standard C ones
#ifndef CALLBACK
#if defined(_WINDOWS) || defined(__CYGWIN__)
#define CALLBACK __stdcall
#else
#define CALLBACK
#endif
#endif

#include "otbReaderModule.h"
#include "otbSpeckleFilteringModule.h"
#include "otbFeatureExtractionModule.h"
#include "otbOrthorectificationModule.h"
#include "otbMeanShiftModule.h"
#include "otbWriterModule.h"
#include "otbWriterMVCModule.h"
#include "otbSupervisedClassificationModule.h"
#include "otbMeanShiftModule.h"
#include "otbPanSharpeningModule.h"
#include "otbViewerModule.h"
#include "otbCachingModule.h"
#include "otbSarIntensityModule.h"
#include "otbHomologousPointExtractionModule.h"
#include "otbExtractROIModule.h"
#include "otbConcatenateModule.h"
#include "otbProjectionModule.h"
#include "otbSuperimpositionModule.h"
#include "otbAlgebraModule.h"
#include "otbKMeansModule.h"
#include "otbChangeDetectionModule.h"
#include "otbGCPToSensorModelModule.h"
#include "otbThresholdModule.h"
#include "otbOpticCalibrationModule.h"
#include "otbSarCalibrationModule.h"
#include "otbCommandLineArgumentParser.h"


int main(int argc, char* argv[])
{
  //Internationalization
  otbI18nMacro();

  // Parse command line parameters
  typedef otb::CommandLineArgumentParser ParserType;
  ParserType::Pointer parser = ParserType::New();
  
  parser->AddInputImage(false); //Optionnal parameter
  parser->SetProgramDescription("Monteverdi launcher");
  //   parser->AddOption("--NoSplashScreen", "Deactivate the splach screen","-NoSplash", 0, false);

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
    }
     
    // Splash Screen (deactivated for now)
//  if ( !parseResult->IsOptionPresent("--NoSplashScreen") )
//    {
//      typedef otb::SplashScreen::Pointer SplashScreenPointerType;
//      
//      SplashScreenPointerType splash = otb::SplashScreen::New();
//      splash->SetDisplayTime(4.);
//      splash->Build();
//      splash->Show();
//    }


  // Application
  typedef otb::MonteverdiModel       ModelType;
  typedef otb::MonteverdiController  ControllerType;
  typedef otb::MonteverdiViewGUI     ViewType;

  // Create the MVC
  ModelType::Pointer model = otb::MonteverdiModel::GetInstance();
  ViewType::Pointer view = ViewType::New();
  ControllerType::Pointer controller = ControllerType::New();
  controller->SetView(view);
  view->SetMonteverdiController(controller);

  // Register modules
  model->RegisterModule<otb::ReaderModule>("0Reader", otbGetTextMacro("File/Open dataset"));
  model->RegisterModule<otb::WriterModule> ("1Writer", otbGetTextMacro("File/Save dataset"));
  model->RegisterModule<otb::WriterMVCModule> ("2Specific writer for X image", otbGetTextMacro("File/Save dataset (advanced)"));
  model->RegisterModule<otb::CachingModule>("3Caching", otbGetTextMacro("File/Cache dataset"));
  model->RegisterModule<otb::ExtractROIModule>("4ExtractROI", otbGetTextMacro("File/Extract ROI from dataset"));
  model->RegisterModule<otb::ConcatenateModule>("5Concatenate", otbGetTextMacro("File/Concatenate images"));

  model->RegisterModule<otb::ViewerModule>("Viewer", otbGetTextMacro("Visualization/Viewer"));

  model->RegisterModule<otb::OpticCalibrationModule>("0OpticCalibration", otbGetTextMacro("Calibration/Optic Calibration"));
  model->RegisterModule<otb::SarCalibrationModule>("1SarCalibration", otbGetTextMacro("Calibration/SAR Calibration"));

  model->RegisterModule<otb::AlgebraModule>("0Algebra", otbGetTextMacro("Filtering/Band math"));
  model->RegisterModule<otb::ThresholdModule>("1Threshold", otbGetTextMacro("Filtering/Threshold"));
  model->RegisterModule<otb::PanSharpeningModule> ("2PanSharpening", otbGetTextMacro("Filtering/Pansharpening"));
  model->RegisterModule<otb::MeanShiftModule> ("3MeanShift", otbGetTextMacro("Filtering/Mean shift clustering"));
  model->RegisterModule<otb::FeatureExtractionModule>("4FeatureExtraction", otbGetTextMacro("Filtering/Feature extraction"));
  model->RegisterModule<otb::ChangeDetectionModule>("5ChangeDetection", otbGetTextMacro("Filtering/Change detection"));

  model->RegisterModule<otb::SpeckleFilteringModule>("Speckle", otbGetTextMacro("SAR/Despeckle image"));
  model->RegisterModule<otb::SarIntensityModule>("SarIntensity", otbGetTextMacro("SAR/Compute intensity and log-intensity"));

  model->RegisterModule<otb::SupervisedClassificationModule>("SupervisedClassification", otbGetTextMacro("Learning/SVM classification"));
  model->RegisterModule<otb::KMeansModule>("KMeans", otbGetTextMacro("Learning/KMeans clustering"));

  model->RegisterModule<otb::OrthorectificationModule>("Orthorectification", otbGetTextMacro("Geometry/Orthorectification"));
  model->RegisterModule<otb::ProjectionModule>("Projection", otbGetTextMacro("Geometry/Reproject image"));
  model->RegisterModule<otb::SuperimpositionModule>("Superimposition", otbGetTextMacro("Geometry/Superimpose two images"));
  model->RegisterModule<otb::HomologousPointExtractionModule>("HomologousPoints", otbGetTextMacro("Geometry/Homologous points extraction"));
  model->RegisterModule<otb::GCPToSensorModelModule>("GCPToSensorModel", otbGetTextMacro("Geometry/GCP to sensor model"));
  
  
  // Launch Monteverdi
  view->InitWidgets();
  view->Show();
  Fl::lock();
  

  //Test if there is an input image (optional)
  if ( parseResult->IsOptionInputImagePresent() )
    {
          Fl::check();
          std::vector<std::string> moduleVector;

          // Create an instance of module reader
          model->CreateModuleByKey("0Reader");
          moduleVector = model->GetAvailableModuleInstanceIds();

          // Get the ModuleInstanceId
          std::string readerId = moduleVector[0];

          // Get the module itself
          otb::Module::Pointer module = model->GetModuleByInstanceId(readerId);

          // Simulate file chooser and ok callback
          // Cyrille cast effect !
          otb::ReaderModule::Pointer readerModule = static_cast<otb::ReaderModule::Pointer>(dynamic_cast<otb::ReaderModule *>(module.GetPointer()));
          readerModule->vFilePath->value(parseResult->GetInputImage().c_str());
          readerModule->Analyse();
          readerModule->bOk->do_callback();
          Fl::check();

          // Create an instance of module viewer
          model->CreateModuleByKey("Viewer");
          moduleVector = model->GetAvailableModuleInstanceIds();

          // Get the ModuleInstanceId
          std::string viewerId = moduleVector[1];

          // Get the module itself
          otb::Module::Pointer module2 = model->GetModuleByInstanceId(viewerId);

          // Open the viewer and simulate a connexion
          otb::ViewerModule::Pointer viewerModule = static_cast<otb::ViewerModule::Pointer>(dynamic_cast<otb::ViewerModule *>(module2.GetPointer()));

          typedef otb::Module::InputDataDescriptorMapType              InputDataDescriptorMapType;
          InputDataDescriptorMapType lInputDataMap = model->GetModuleInputsByInstanceId(viewerId);
          InputDataDescriptorMapType::const_iterator it_in;
          it_in = lInputDataMap.begin();

          std::string viewerInputKey = it_in->first;

          typedef otb::InputViewGUI::InputViewComponentMapType InputViewComponentMapType;
          InputViewComponentMapType inputComponentMap;
          inputComponentMap = view->GetInputViewGUI()->GetInputViewComponentMap();

          for(unsigned int i =0;i<inputComponentMap[viewerInputKey]->GetNumberOfChoices();i++)
            {
            inputComponentMap[viewerInputKey]->SelectNthChoice(i);
            }
          Fl::check();
              
          view->GetInputViewGUI()->bOk->do_callback();
          Fl::check();
    }

  
  return Fl::run();
}
