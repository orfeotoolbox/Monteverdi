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


#include "otbSpeckleFilteringView.h"
#include "otbSpeckleFilteringModel.h"
#include "otbSpeckleFilteringController.h"

#include "otbImage.h"
#include "otbImageFileReader.h"
#include "otbImageFileWriter.h"

int main(int argc, char* argv[])
{

  typedef otb::SpeckleFilteringView              ViewType;
  typedef otb::SpeckleFilteringController           ControllerType;
  typedef ControllerType::ModelType                 ModelType;

  // Instanciation of pointer
  ViewType::Pointer          view       = ViewType::New();
  ControllerType::Pointer    controller = ControllerType::New();

  controller->SetView(view);
  view->SetController(controller);

  // Put in the tests
  const char * infname = argv[1];
  typedef otb::Image<double,2>  ImageType;
  typedef otb::ImageFileReader<ImageType>     ReaderType;
  typedef otb::ImageFileWriter<ImageType>     WriterType;


  //reader
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName(infname);
  reader->Update();

  //Set The model input Image
  ModelType * model = ModelType::GetInstance();
  model->SetInputImage(reader->GetOutput());

  // Open the GUI
  view->Show();
  Fl::run();

  //Write the image
  WriterType::Pointer  writer = WriterType::New();
  writer->SetFileName(argv[2]);
  writer->SetInput(model->GetOutput());
  writer->Update();

  return 0;

}

