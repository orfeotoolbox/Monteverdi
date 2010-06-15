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


#include "otbFeatureExtractionViewGUI.h"
#include "otbFeatureExtractionModel.h"
#include "otbFeatureExtractionController.h"

#include "otbVectorImage.h"
#include "otbImageFileReader.h"
#include "otbImageFileWriter.h"

int otbFeatureExtractionModuleApplication(int argc, char* argv[])
{
  typedef otb::FeatureExtractionViewGUI              ViewType;
  typedef otb::FeatureExtractionController           ControllerType;
  typedef ControllerType::ModelType                 ModelType;

  // Instanciation of pointer
  ViewType::Pointer       view       = ViewType::New();
  ControllerType::Pointer controller = ControllerType::New();
  ModelType::Pointer      model      = ModelType::New();

  view->SetFeatureExtractionModel(model);
  view->InitVisu();
  view->SetFeatureExtractionController(controller);
  
  controller->SetModel(model);
  controller->SetView(view);
  
  // Put in the tests
  const char * infname = argv[1];
  typedef otb::FeatureExtractionModel::InputImageType  ImageType;
  typedef otb::ImageFileReader<ImageType>              ReaderType;
  typedef otb::ImageFileWriter<ImageType>              WriterType;
  
  //reader
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName(infname);
  reader->GenerateOutputInformation();
  
  model->SetInputImage(reader->GetOutput());

  // Open the GUI
  view->Show();
  Fl::check();
  
  // ------------ Chose Text feature (entropy)
  view->UpdateParameterArea(6);
  view->SetFeatureType(otb::FeatureInfo::TEXT_ENT);
  Fl::check();

  view->guiTextures->show();
  Fl::check();
  view->guiTextRadiusX->value(3);
  view->guiTextRadiusY->value(1);
  view->guiTextOffsetX->value(2);
  view->guiTextOffsetY->value(1);
  view->guiParameter->redraw();
  Fl::check();
  
  // Uncheck channels
  view->guiChannelSelection->checked(1,false);
  view->guiChannelSelection->checked(2,false);
  view->guiChannelSelection->checked(3,false);
  view->guiChannelSelection->checked(4,false);

  // Update Channels
  view->UpdateChannels();

  //Add the feature
  view->guiAdd->do_callback();
  view->guiFeatureListAction->redraw();
  Fl::check();

  view->guiOK->do_callback();
  Fl::check();

  //Write the image
  WriterType::Pointer  writer = WriterType::New();
  writer->SetFileName(argv[2]);
  writer->SetInput(model->GetOutputImage());
  writer->Update();

  return 0;
}

