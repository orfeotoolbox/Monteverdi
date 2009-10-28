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
  model->RegisterModule<otb::ReaderModule>(otbGetTextMacro("Reader"),"File/Open dataset");
  model->RegisterModule<otb::WriterModule> (otbGetTextMacro("Writer"),otbGetTextMacro("File/Save dataset"));
  model->RegisterModule<otb::ExtractROIModule>(otbGetTextMacro("ExtractROI"),"File/Extract ROI from dataset");
  model->RegisterModule<otb::WriterMVCModule> (otbGetTextMacro("Specific writer for X image"),"File/Save dataset (advanced)");
  model->RegisterModule<otb::SpeckleFilteringModule>(otbGetTextMacro("Speckle"),"SAR/Despeckle image");
  model->RegisterModule<otb::SarIntensityModule>(otbGetTextMacro("SarIntensity"),"SAR/Compute intensity and log-intensity");

  model->RegisterModule<otb::FeatureExtractionModule>(otbGetTextMacro("FeatureExtraction"), "Filtering/Feature extraction");
  model->RegisterModule<otb::SupervisedClassificationModule>(otbGetTextMacro("SupervisedClassification"), "Learning/SVM classification");
  model->RegisterModule<otb::KMeansModule>(otbGetTextMacro("KMeans"),"Learning/KMeans clustering");
  model->RegisterModule<otb::OrthorectificationModule>(otbGetTextMacro("Orthorectification"), "Geometry/Orthorectification");
  model->RegisterModule<otb::ProjectionModule>(otbGetTextMacro("Projection"),"Geometry/Reproject image");
  model->RegisterModule<otb::SuperimpositionModule>(otbGetTextMacro("Superimposition"),"Geometry/Superimpose two images");
  model->RegisterModule<otb::HomologousPointExtractionModule>(otbGetTextMacro("HomologousPoints"), "Geometry/Homologous points extraction");
  model->RegisterModule<otb::GCPToSensorModelModule>(otbGetTextMacro("GCPToSensorModel"), "Geometry/GCP To Sensor Model");

  model->RegisterModule<otb::MeanShiftModule> (otbGetTextMacro("MeanShift"),"Filtering/Mean shift clustering");
  model->RegisterModule<otb::PanSharpeningModule> (otbGetTextMacro("PanSharpening"),"Filtering/Pan-sharpen an image");
  model->RegisterModule<otb::ViewerModule>(otbGetTextMacro("Viewer"),"Visualization/Viewer");
  model->RegisterModule<otb::CachingModule>(otbGetTextMacro("zCaching"),"File/Cache dataset");
  model->RegisterModule<otb::ConcatenateModule>(otbGetTextMacro("Concatenate"),"File/Concatenate images");

  model->RegisterModule<otb::AlgebraModule>(otbGetTextMacro("Algebra"),"Filtering/Band Math");
  model->RegisterModule<otb::ChangeDetectionModule>(otbGetTextMacro("ChangeDetection"),"Filtering/Change Detection");
  model->RegisterModule<otb::ThresholdModule>(otbGetTextMacro("Threshold"),"Filtering/Threshold");
  
  // Launch Monteverdi
  view->InitWidgets();
  view->Show();

  Fl::lock();
  return Fl::run();
}
