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

#include "otbRadiometricIndicesExtractionViewGUI.h"
#include "otbRadiometricIndicesExtractionModel.h"
#include "otbRadiometricIndicesExtractionController.h"

#include "otbTextureExtractionViewGUI.h"
#include "otbTextureExtractionModel.h"
#include "otbTextureExtractionController.h"

#include "otbMorphologyExtractionViewGUI.h"
#include "otbMorphologyExtractionModel.h"
#include "otbMorphologyExtractionController.h"

#include "otbEdgeExtractionViewGUI.h"
#include "otbEdgeExtractionModel.h"
#include "otbEdgeExtractionController.h"

#include "otbSmoothingViewGUI.h"
#include "otbSmoothingModel.h"
#include "otbSmoothingController.h"

#include "otbVectorImage.h"
#include "otbImageFileReader.h"
#include "otbImageFileWriter.h"

int main(int argc, char* argv[])
{
  typedef otb::FeatureExtractionBaseViewGUI           BaseViewType;
  typedef otb::FeatureExtractionBaseController        BaseControllerType;

  /* --------- Choose the MVC to be tested --------- */
//   typedef otb::RadiometricIndicesExtractionViewGUI    ViewType;
//   typedef otb::RadiometricIndicesExtractionController ControllerType;
//   typedef otb::RadiometricIndicesExtractionModel      ModelType;
  
  typedef otb::TextureExtractionViewGUI    ViewType;
  typedef otb::TextureExtractionController ControllerType;
  typedef otb::TextureExtractionModel      ModelType;

//   typedef otb::MorphologyExtractionViewGUI    ViewType;
//   typedef otb::MorphologyExtractionController ControllerType;
//   typedef otb::MorphologyExtractionModel      ModelType;

//   typedef otb::SmoothingViewGUI    ViewType;
//   typedef otb::SmoothingController ControllerType;
//   typedef otb::SmoothingModel      ModelType;

//   typedef otb::EdgeExtractionViewGUI    ViewType;
//   typedef otb::EdgeExtractionController ControllerType;
//   typedef otb::EdgeExtractionModel      ModelType;
  /* --------------------------------------------------- */

  // Instanciation of pointer
  ViewType::Pointer       view       = ViewType::New();
  ControllerType::Pointer controller = ControllerType::New();
  ModelType::Pointer      model      = ModelType::New();
  
  BaseViewType*       baseView        = view;
  BaseControllerType* baseController  = controller;

  baseView->InitVisu();
  baseView->SetFeatureExtractionBaseController(baseController);
  
  controller->SetModel(model);
  controller->SetView(view);

  // Put in the tests
  const char * infname = argv[1];
  typedef otb::FeatureExtractionBaseModel::InputImageType ImageType;
  typedef otb::ImageFileReader<ImageType>             ReaderType;
  typedef otb::ImageFileWriter<ImageType>             WriterType;

  //reader
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName(infname);
  reader->GenerateOutputInformation();

  model->SetInputImage(reader->GetOutput());

  // Open the GUI
  view->Show();
  Fl::check();

  // ------------ Chose Haralick texture feature (entropy)
//   view->UpdateParameterArea(2);
//   view->SetFeatureType(otb::FeatureInfoTex::TEXT_HAR_ENTROPY);
//   Fl::check();

//   view->m_SpecificView->guiHaralick->show();
//   Fl::check();
//   view->m_SpecificView->guiHarRadiusX->value(3);
//   view->m_SpecificView->guiHarRadiusY->value(1);
//   view->m_SpecificView->guiHarOffsetX->value(2);
//   view->m_SpecificView->guiHarOffsetY->value(1);
//   view->guiParameter->redraw();
//   Fl::check();

//   // Uncheck channels
//   view->guiChannelSelection->checked(1, false);
//   view->guiChannelSelection->checked(2, false);
//   view->guiChannelSelection->checked(3, false);
//   view->guiChannelSelection->checked(4, false);

//   // Update Channels
//   view->UpdateChannels();

//   //Add the feature
//   view->guiAdd->do_callback();
//   view->guiFeatureListAction->redraw();
//   Fl::check();

//   view->guiOK->do_callback();
//   Fl::check();

  //Write the image
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName(argv[2]);
  writer->SetInput(model->GetOutputImage());
  writer->Update();

  return 0;
}
