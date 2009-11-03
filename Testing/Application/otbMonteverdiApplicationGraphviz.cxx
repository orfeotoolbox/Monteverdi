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
#include "otbInputViewGUI.h"



// #include <string>

int otbMonteverdiApplicationGraphviz(int argc, char* argv[])
{

  // Parameters of the tests
  const char * infname = argv[1];
  unsigned int  run = atoi(argv[2]);
  const std::string outgraphvizfname = argv[3];
  
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
  model->RegisterModule<otb::SpeckleFilteringModule>("Speckle","Filtering/Despeckle");
  model->RegisterModule<otb::FeatureExtractionModule>("FeatureExtraction", "Filtering/Feature Extraction");
  model->RegisterModule<otb::SupervisedClassificationModule>("SupervisedClassification", "Learning/SVM Classification");
  model->RegisterModule<otb::OrthorectificationModule>("Orthorectification", "Geometry/Ortho Rectification");
  model->RegisterModule<otb::WriterModule> ("Writer","File/Export dataset");
  model->RegisterModule<otb::ViewerModule> ("Viewer","Visualization/View image");

  // Launch Monteverdi
  view->InitWidgets();
  view->Show();
  Fl::check();

  std::vector<std::string> moduleVector;

  // Create an instance of module reader
  model->CreateModuleByKey("Reader");
  moduleVector = model->GetAvailableModuleInstanceIds();

  // Get the ModuleInstanceId
  std::string readerId = moduleVector[0];

  // Get the module itself
  otb::Module::Pointer module = model->GetModuleByInstanceId(readerId);

  // Simulate file chooser and ok callback
  // Cyrille cast effect !
  otb::ReaderModule::Pointer readerModule = static_cast<otb::ReaderModule::Pointer>(dynamic_cast<otb::ReaderModule *>(module.GetPointer()));
  readerModule->vFilePath->value(infname);
  readerModule->bOk->do_callback();
  Fl::check();



  // Create an instance of module reader
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
  
  if(run)
    Fl::run();
  else
    Fl::check();

  //viewerModule->bQuit->do_callback();
  view->Quit();
  
  //Generate Graphviz modules graph

  // writeoutgraphvizfname
  model->GetGraphvizDotFile ( outgraphvizfname );

  return 0;
}
