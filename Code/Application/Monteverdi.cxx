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

int main(int argc, char* argv[])
{
  //Internationalization
  otbI18nMacro();

  // Splash Screen
  typedef otb::SplashScreen::Pointer SplashScreenPointerType;

 // SplashScreenPointerType splash = otb::SplashScreen::New();
 // splash->Build();
 // splash->Show();


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
  model->RegisterModule<otb::ReaderModule>("Reader","File/Import dataset");
  model->RegisterModule<otb::WriterModule> ("Writer","File/Export dataset");
  model->RegisterModule<otb::ExtractROIModule>("ExtractROI","File/Extract ROI from dataset");
  model->RegisterModule<otb::WriterMVCModule> ("Specific writer for X image","File/Export X image");
  model->RegisterModule<otb::SpeckleFilteringModule>("Speckle","SAR/Despeckle image");
  model->RegisterModule<otb::SarIntensityModule>("SarIntensity","SAR/Compute intensity and log-intensity");

  model->RegisterModule<otb::FeatureExtractionModule>("FeatureExtraction", "Filtering/Feature extraction");
  model->RegisterModule<otb::SupervisedClassificationModule>("SupervisedClassification", "Learning/SVM classification");
  model->RegisterModule<otb::OrthorectificationModule>("Orthorectification", "Geometry/Orthorectification");
  model->RegisterModule<otb::HomologousPointExtractionModule>("HPEModule", "Geometry/Homologous points extraction");

  model->RegisterModule<otb::MeanShiftModule> ("MeanShift","Filtering/Mean shift clustering");
  model->RegisterModule<otb::PanSharpeningModule> ("PanSharpening","Filtering/Pan-sharpen an image");
  model->RegisterModule<otb::ViewerModule>("Viewer","Visualization/Viewer");
  model->RegisterModule<otb::CachingModule>("zCaching","File/Cache dataset");
  model->RegisterModule<otb::ConcatenateModule>("Concatenate","File/Concatenate images");
  
  // Launch Monteverdi
  view->InitWidgets();
  view->Show();

  Fl::lock();
  return Fl::run();
}
