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

#include "otbMonteverdiModel.h"
#include "otbMonteverdiViewGUI.h"
#include "otbMonteverdiController.h"

#include "otbReaderModule.h"
#include "otbSpeckleFilteringModule.h"
#include "otbFeatureExtractionModule.h"
#include "otbOrthorectificationModule.h"
#include "otbViewerModule.h"
#include "otbWriterModule.h"
#include "otbSupervisedClassificationModule.h"

int otbMonteverdiApplicationTest(int argc, char* argv[])
{

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
  model->RegisterModule<otb::ReaderModule>("Reader", "File/Import dataset");
  model->RegisterModule<otb::SpeckleFilteringModule>("Speckle", "Filtering/Despeckle");
  model->RegisterModule<otb::FeatureExtractionModule>("FeatureExtraction", "Filtering/Feature Extraction");
  model->RegisterModule<otb::SupervisedClassificationModule>("SupervisedClassification", "Learning/SVM Classification");
  model->RegisterModule<otb::OrthorectificationModule>("Orthorectification", "Geometry/Ortho Rectification");
  model->RegisterModule<otb::WriterModule> ("Writer", "File/Export dataset");
  model->RegisterModule<otb::ViewerModule> ("Viewer", "Visualization/View image");

  // Launch Monteverdi
  view->InitWidgets();
  view->Show();

  Fl::check();

  view->Quit();

  return 0;
}
