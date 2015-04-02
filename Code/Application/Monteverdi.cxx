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
#include <string>
#include <ctime>
#include <iostream>

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_PNG_Image.H>

#include "itksys/SystemTools.hxx"

#include "ConfigureMonteverdi.h"

#include "otbMonteverdiModel.h"
#include "otbMonteverdiViewGUI.h"
#include "otbMonteverdiController.h"
#include "otbSplashScreen.h"


// There are function prototype conflits under cygwin between standard w32 API
// and standard C ones
#ifndef CALLBACK
#if defined(_WINDOWS) || defined(__CYGWIN__)
#define CALLBACK __stdcall
#else
#define CALLBACK
#endif
#endif

#include "otbMsgReporter.h"

#include "otbReaderModule.h"
#include "otbSpeckleFilteringModule.h"
#include "otbMeanShiftModule.h"
#include "otbWriterModule.h"
#include "otbWriterMVCModule.h"
#if OTB_USE_LIBSVM
#include "otbSupervisedClassificationModule.h"
#include "otbSupervisedClassificationModule2.h"
#include "otbChangeDetectionModule.h"
#include "otbObjectCountingModule.h"
#include "otbObjectLabelingModule.h"
#endif
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
#include "otbKMeansModule.h"
#include "otbGCPToSensorModelModule.h"
#include "otbThresholdModule.h"
#include "otbOpticalCalibrationModule.h"
#include "otbSarCalibrationModule.h"
#include "otbCommandLineArgumentParser.h"
#include "otbTileExportModule.h"
#include "otbFineCorrelationModule.h"
#include "otbVectorizationModule.h"
#include "otbSpectrumModule.h"
#include "otbBandMathModule.h"
#include "otbPolarimetricSynthesisModule.h"
#include "otbDEMToImageGeneratorModule.h"
#include "otbColorMappingModule.h"
#include "otbImageStatisticsModule.h"
#include "otbBayesianFusionModule.h"
#include "otbSARPolarimetrySinclairModule.h"
#include "otbSARPolarimetryMuellerModule.h"
#include "otbSARPolarimetryReciprocalModule.h"
#include "otbSARPolarimetryAnalysisModule.h"
#include "otbConnectedComponentSegmentationModule.h"
#include "otbVectorDataTransformModule.h"
#include "otbUncompressJpeg2000Module.h"
#include "otbResampleModule.h"
#include "otbCompareImagesModule.h"
#include "otbRadiometricIndicesExtractionModule.h"
#include "otbTextureExtractionModule.h"
#include "otbMorphologyExtractionModule.h"
#include "otbSmoothingModule.h"
#include "otbEdgeExtractionModule.h"
#include "otbMosaicImagesModule.h"

#include "otbExtendedFilenameToReaderOptions.h"

#ifdef OTB_USE_MAPNIK
#include "otbRasterizationModule.h"
#endif

#ifdef OTB_USE_CURL
#include "otbTileMapImportModule.h"
#endif

#include "otbCurlHelperInterface.h"

int main(int argc, char* argv[])
{
  //Internationalization


  // Parse command line parameters
  typedef otb::CommandLineArgumentParser ParserType;
  ParserType::Pointer parser = ParserType::New();
  parser->AddOption("--InputImage", "input image file name to be visualized in the monteverdi viewer", "-in", 1, false);
  parser->AddOptionNParams("--InputList", "inputs can be images and vectorDatas (are not visualized in the viewer)", "-il", false);
  parser->SetProgramDescription("Monteverdi launcher");
  //   parser->AddOption("--NoSplashScreen", "Deactivate the splash screen","-NoSplash", 0, false);

  typedef otb::CommandLineArgumentParseResult ParserResultType;
  ParserResultType::Pointer parseResult = ParserResultType::New();

  try
    {
    parser->ParseCommandLine(argc, argv, parseResult);
    }
  catch (itk::ExceptionObject& err)
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
  typedef otb::MonteverdiModel      ModelType;
  typedef otb::MonteverdiController ControllerType;
  typedef otb::MonteverdiViewGUI    ViewType;

  // Create the MVC
  ModelType::Pointer      model = otb::MonteverdiModel::GetInstance();
  ViewType::Pointer       view = ViewType::New();
  ControllerType::Pointer controller = ControllerType::New();
  controller->SetView(view);
  view->SetMonteverdiController(controller);

  // Register modules
  /***********  File menu *******************/
  model->RegisterModule<otb::ReaderModule>("Reader", "File/Open dataset");
  model->RegisterModule<otb::WriterModule> ("Writer", "File/Save dataset");
  model->RegisterModule<otb::WriterMVCModule> ("Specific writer for X image",
                                               "File/Save dataset (advanced)");
  model->RegisterModule<otb::CachingModule>("Caching", "File/Cache dataset");
  model->RegisterModule<otb::ExtractROIModule>("ExtractROI", "File/Extract ROI from dataset");
  model->RegisterModule<otb::UncompressJpeg2000Module>("UncompressJpeg2000Module", "File/Uncompress Jpeg2000 image");
  model->RegisterModule<otb::ConcatenateModule>("Concatenate", "File/Concatenate images");
  model->RegisterModule<otb::MosaicImagesModule>("Mosaic", "File/Mosaic images");
#ifdef OTB_USE_MAPNIK
  model->RegisterModule<otb::RasterizationModule>("Rasterize", "File/Rasterize vector data");
#endif

  model->RegisterModule<otb::TileExportModule>("Export To Kmz", "File/Export To Kmz");

#ifdef OTB_USE_CURL
    model->RegisterModule<otb::TileMapImportModule>("Tile Map Import", "File/Tile Map Import");
#endif

    model->RegisterModule<otb::ImageStatisticsModule>("Image Statistics", "File/Statistics/Image Statistics");
    model->RegisterModule<otb::CompareImagesModule>("Compare Images", "File/Statistics/Compare images");

  /***********  Visu menu *******************/
  model->RegisterModule<otb::ViewerModule>("Viewer", "Visualization/Viewer");
  model->RegisterModule<otb::SpectrumModule>("SpectralViewer", "Visualization/Spectral Viewer");
  model->RegisterModule<otb::ColorMappingModule>("ColorMapping", "Visualization/Color Mapping");

  /***********  Calibration menu *******************/
  model->RegisterModule<otb::OpticalCalibrationModule>("OpticalCalibration",
                                                       "Calibration/Optical Calibration");
  model->RegisterModule<otb::SarCalibrationModule>("SarCalibration", "Calibration/SAR Calibration");

  /***********  Filtering menu *******************/
  model->RegisterModule<otb::BandMathModule>("BandMath", "Filtering/BandMath");
  model->RegisterModule<otb::ThresholdModule>("Threshold", "Filtering/Threshold");
  model->RegisterModule<otb::PanSharpeningModule> ("PanSharpening", "Filtering/Pansharpening/Simple RCS pan-sharpening");
  model->RegisterModule<otb::BayesianFusionModule>("BayesianFusion", "Filtering/Pansharpening/Bayesian fusion");
  model->RegisterModule<otb::MeanShiftModule> ("MeanShift", "Filtering/Mean shift clustering");
//   model->RegisterModule<otb::FeatureExtractionModule>("FeatureExtraction",
//                                                       "Filtering/Feature extraction");
  model->RegisterModule<otb::RadiometricIndicesExtractionModule>("RadiometricIndicesExtraction",
                                                      "Filtering/Feature extraction/Radiometric indices extraction");
  model->RegisterModule<otb::TextureExtractionModule>("TextureExtraction",
                                                      "Filtering/Feature extraction/Texture extraction");
  model->RegisterModule<otb::MorphologyExtractionModule>("MorphologyExtraction",
                                                      "Filtering/Feature extraction/Morphology");
  model->RegisterModule<otb::SmoothingModule>("Smoothing",
                                                      "Filtering/Feature extraction/Smoothing");
  model->RegisterModule<otb::EdgeExtractionModule>("EdgeExtraction",
                                                      "Filtering/Feature extraction/Edge extraction");
#if OTB_USE_LIBSVM
  model->RegisterModule<otb::ChangeDetectionModule>("ChangeDetection", "Filtering/Change detection");
#endif
  model->RegisterModule<otb::FineCorrelationModule>("FineCorrelation", "Filtering/Fine Correlation");
  model->RegisterModule<otb::VectorizationModule>("Vectorization", "Filtering/Vectorization");
  model->RegisterModule<otb::ConnectedComponentSegmentationModule>("ConnectedComponentSegmentation", "Filtering/Connected Component segmentation");
  model->RegisterModule<otb::ResampleModule>("Resampling", "Filtering/Resampling");

  /***********  SAR menu *******************/
  model->RegisterModule<otb::SpeckleFilteringModule>("Speckle", "SAR/Despeckle");
  model->RegisterModule<otb::SarIntensityModule>("SarIntensity",
                                                 "SAR/Compute intensity and log-intensity");
  model->RegisterModule<otb::PolarimetricSynthesisModule>("PolarimetricSynthesis",
                                                 "SAR/Polarimetry/Synthesis");
  model->RegisterModule<otb::SARPolarimetrySinclairModule>("PolarimetrySinclair",
                                                 "SAR/Polarimetry/Conversion/From Sinclair matrix to");
   model->RegisterModule<otb::SARPolarimetryMuellerModule>("PolarimetryMueller",
                                                 "SAR/Polarimetry/Conversion/From Mueller matrix to");
  model->RegisterModule<otb::SARPolarimetryReciprocalModule>("PolarimetryReciprocal",
                                                 "SAR/Polarimetry/Conversion/From reciprocal matrix to");
  model->RegisterModule<otb::SARPolarimetryAnalysisModule>("PolarimetryAnalysis",
                                                 "SAR/Polarimetry/Analysis");

  /***********  Learning menu *******************/
#if OTB_USE_LIBSVM
  model->RegisterModule<otb::SupervisedClassificationModule>("SupervisedClassification",
                                                             "Learning/SVM classification");
  model->RegisterModule<otb::SupervisedClassificationModule2>("SupervisedClassification2",
                                                                     "Learning/SVM classification (EXPERIMENTAL)");
#endif
  model->RegisterModule<otb::KMeansModule>("KMeans", "Learning/KMeans clustering");

  /***********  Geometry menu *******************/
  model->RegisterModule<otb::ProjectionModule>("Projection", "Geometry/Reproject image");
  model->RegisterModule<otb::SuperimpositionModule>("Superimposition",
                                                    "Geometry/Superimpose two images");
  model->RegisterModule<otb::HomologousPointExtractionModule>("HomologousPoints",
                                                              "Geometry/Homologous points extraction");
#if OTB_USE_LIBSVM
  model->RegisterModule<otb::ObjectLabelingModule>("Object Labeling (Experimental)", "Learning/Object Labeling");
#endif
  model->RegisterModule<otb::GCPToSensorModelModule>("GCPToSensorModel",
                                                     "Geometry/GCP to sensor model");
  model->RegisterModule<otb::DEMToImageGeneratorModule>("DEM To Image Generator",
                                                     "Geometry/DEM To Image Generator");

  model->RegisterModule<otb::VectorDataTransformModule>("VectorData Transform",
                                                        "Geometry/VectorData Transform");

  // Launch Monteverdi
  view->InitWidgets();
  view->Show();
  Fl::lock();

  //Test if there is an input image (optional)
  if (parseResult->IsOptionPresent("--InputImage"))
    {
    otb::ExtendedFilenameToReaderOptions::Pointer extendedFilenameHelper = otb::ExtendedFilenameToReaderOptions::New();
    extendedFilenameHelper->SetExtendedFileName(parseResult->GetInputImage().c_str());
    if (!itksys::SystemTools::FileExists(extendedFilenameHelper->GetSimpleFileName()))
      {
        std::ostringstream oss;
        oss << "The file "<< parseResult->GetInputImage().c_str() <<" does not exist.";
        otb::MsgReporter::GetInstance()->SendError( oss.str().c_str() );
      }
    else
      {
      Fl::check();

      std::vector<std::string> moduleVector;

      // Create an instance of module reader and get the ID
      std::string readerId = model->CreateModuleByKey("Reader");

      // Get the module itself
      otb::Module::Pointer module = model->GetModuleByInstanceId(readerId);

      // Simulate file chooser and ok callback
      // Cyrille cast effect !
      otb::ReaderModule::Pointer
          readerModule = static_cast<otb::ReaderModule::Pointer> (dynamic_cast<otb::ReaderModule *> (module.GetPointer()));
      readerModule->vFilePath->value(parseResult->GetInputImage().c_str());
      readerModule->Analyse();
      readerModule->bOk->do_callback();
      Fl::check();

      // Need to wait the end of the reader thread in case of JPEG2000 reading
      readerModule->SynchronizeThreads();

      // Create an instance of module viewer and get the ID
      std::string viewerId = model->CreateModuleByKey("Viewer");

      // Get the module itself
      otb::Module::Pointer module2 = model->GetModuleByInstanceId(viewerId);

      // Open the viewer and simulate a connection
      otb::ViewerModule::Pointer
          viewerModule = static_cast<otb::ViewerModule::Pointer> (dynamic_cast<otb::ViewerModule *> (module2.GetPointer()));

      typedef otb::Module::InputDataDescriptorMapType InputDataDescriptorMapType;
      InputDataDescriptorMapType lInputDataMap = model->GetModuleInputsByInstanceId(viewerId);
      InputDataDescriptorMapType::const_iterator it_in;
      it_in = lInputDataMap.begin();

      std::string viewerInputKey = it_in->first;

      typedef otb::InputViewGUI::InputViewComponentMapType InputViewComponentMapType;
      InputViewComponentMapType inputComponentMap;
      inputComponentMap = view->GetInputViewGUI()->GetInputViewComponentMap();

      inputComponentMap[viewerInputKey]->SelectNthChoice(1);

      Fl::check();

      view->GetInputViewGUI()->bOk->do_callback();
      Fl::check();
      }
    }

  if (parseResult->IsOptionPresent("--InputList"))
    {
      int numberOfImage = parseResult->GetNumberOfParameters("--InputList");
      for (int i = 0; i < numberOfImage; i++)
        {
        Fl::check();
        std::vector<std::string> moduleVector;

        std::string filename = parseResult->GetParameterString("--InputList", i);
        if( itksys::SystemTools::FileExists(filename.c_str()) )
          {
            // Get the ModuleInstanceId
            std::string readerId = model->CreateModuleByKey("Reader");

            // Get the module itself
            otb::Module::Pointer module = model->GetModuleByInstanceId(readerId);

            // Simulate file chooser and ok callback
            otb::ReaderModule::Pointer readerModule = static_cast<otb::ReaderModule::Pointer>(dynamic_cast<otb::ReaderModule *>(module.GetPointer()));
            readerModule->vFilePath->value(parseResult->GetParameterString("--InputList", i).c_str());
            readerModule->Analyse();
            readerModule->bOk->do_callback();
            Fl::check();
          }
        else
          {
            std::ostringstream oss;
            oss << "The file "<< filename <<" does not exist.";
            otb::MsgReporter::GetInstance()->SendError( oss.str().c_str() );
          }
        }
    }

  return Fl::run();
}
