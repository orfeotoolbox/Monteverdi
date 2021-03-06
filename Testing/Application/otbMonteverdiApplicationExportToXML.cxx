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
#include "otbRadiometricIndicesExtractionModule.h"
#include "otbFeatureRI.h"
#include "otbViewerModule.h"
#include "otbWriterModule.h"
#include "otbSupervisedClassificationModule.h"
#include "otbInputViewGUI.h"

#include "otbMacro.h"

int otbMonteverdiApplicationExportToXML(int argc, char* argv[])
{

  // Parameters of the tests
  const char * infname = argv[1];
  unsigned int run = atoi(argv[2]);
  const char * oufname = argv[3];
  const char * oufname2 = argv[4];

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
  //model->RegisterModule<otb::SpeckleFilteringModule>("Speckle","Filtering/Despeckle");
  model->RegisterModule<otb::RadiometricIndicesExtractionModule>("RadiometricIndicesExtraction", "Filtering/Radiometric Indices Extraction");
  //model->RegisterModule<otb::SupervisedClassificationModule>("SupervisedClassification", "Learning/SVM Classification");
  model->RegisterModule<otb::WriterModule> ("Writer", "File/Export dataset");
  model->RegisterModule<otb::ViewerModule> ("Viewer", "Visualization/View image");

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
  otb::ReaderModule::Pointer readerModule =
    static_cast<otb::ReaderModule::Pointer>(dynamic_cast<otb::ReaderModule *>(module.GetPointer()));
  readerModule->vFilePath->value(infname);
  readerModule->Analyse();
  readerModule->bOk->do_callback();
  Fl::check();

  //RadiometricIndicesExtraction module
  // Create an instance of module viewer
  otbGenericMsgDebugMacro(<< "Create radiometric indices extraction module");
  model->CreateModuleByKey("RadiometricIndicesExtraction");
  moduleVector = model->GetAvailableModuleInstanceIds();

  // Get the ModuleInstanceId
  std::string riExtractionId = moduleVector[1];

  otbGenericMsgDebugMacro(<< "riExtractionId " << riExtractionId);
  // Get the module itself
  otb::Module::Pointer module2 = model->GetModuleByInstanceId(riExtractionId);

  // Open the viewer and simulate a connexion
  otb::RadiometricIndicesExtractionModule::Pointer RIExtractionModule =
    static_cast<otb::RadiometricIndicesExtractionModule::Pointer>(dynamic_cast<otb::RadiometricIndicesExtractionModule *>(module2.GetPointer()));

  otbGenericMsgDebugMacro(<< "Input data management");

  typedef otb::Module::InputDataDescriptorMapType InputDataDescriptorMapType;
  InputDataDescriptorMapType                 lInputDataMap = model->GetModuleInputsByInstanceId(riExtractionId);
  InputDataDescriptorMapType::const_iterator it_in;
  it_in = lInputDataMap.begin();

  std::string riExtractionInputKey = it_in->first;

  otbGenericMsgDebugMacro(<< "riExtractionInputKey " << riExtractionInputKey);

  typedef otb::InputViewGUI::InputViewComponentMapType InputViewComponentMapType;
  InputViewComponentMapType inputComponentMap;
  inputComponentMap = view->GetInputViewGUI()->GetInputViewComponentMap();

  otbGenericMsgDebugMacro(
    << "input component map number choices: " <<  inputComponentMap[riExtractionInputKey]->GetNumberOfChoices());

  for (unsigned int i = 0; i < inputComponentMap[riExtractionInputKey]->GetNumberOfChoices(); i++)
    {
    otbGenericMsgDebugMacro(<< "input component map: " <<  inputComponentMap[riExtractionInputKey]);
    inputComponentMap[riExtractionInputKey]->SelectNthChoice(i);
    }
  Fl::check();

  view->GetInputViewGUI()->bOk->do_callback();
  Fl::check();

  // Set the feature extraction GUI actions
  RIExtractionModule->GetViewRI()->UpdateParameterArea(1);
  RIExtractionModule->GetViewRI()->SetFeatureType(otb::FeatureInfoRI::NDVI);
  Fl::check();

  //Select bands 2 and 3
  RIExtractionModule->GetViewRI()->m_SpecificGUI->guiRAndNIRR->value(2);
  RIExtractionModule->GetViewRI()->m_SpecificGUI->guiRAndNIRR->redraw();
  Fl::check();
  RIExtractionModule->GetViewRI()->m_SpecificGUI->guiRAndNIRR->do_callback();
  RIExtractionModule->GetViewRI()->m_SpecificGUI->guiRAndNIRNIR->value(1);
  RIExtractionModule->GetViewRI()->m_SpecificGUI->guiRAndNIRNIR->redraw();
  Fl::check();
  RIExtractionModule->GetViewRI()->m_SpecificGUI->guiRAndNIRNIR->do_callback();

  //Add the feature
  RIExtractionModule->GetViewRI()->guiAdd->do_callback();
  RIExtractionModule->GetViewRI()->guiFeatureListAction->redraw();
  Fl::check();

  // Create an instance of module viewer
  model->CreateModuleByKey("Viewer");
  moduleVector = model->GetAvailableModuleInstanceIds();

  // Get the ModuleInstanceId
  std::string viewerId = moduleVector[2];

  // Get the module itself
  otb::Module::Pointer module3 = model->GetModuleByInstanceId(viewerId);

  // Open the viewer and simulate a connexion
  otb::ViewerModule::Pointer viewerModule =
    static_cast<otb::ViewerModule::Pointer>(dynamic_cast<otb::ViewerModule *>(module3.GetPointer()));

  lInputDataMap = model->GetModuleInputsByInstanceId(viewerId);

  it_in = lInputDataMap.begin();

  std::string viewerInputKey = it_in->first;

  otbGenericMsgDebugMacro(<< "viewerInputKey " << viewerInputKey);

  inputComponentMap = view->GetInputViewGUI()->GetInputViewComponentMap();

  for (unsigned int i = 0; i < inputComponentMap[viewerInputKey]->GetNumberOfChoices(); i++)
    {
    inputComponentMap[viewerInputKey]->SelectNthChoice(i);
    }
  Fl::check();

  view->GetInputViewGUI()->bOk->do_callback();
  Fl::check();

  if (run) Fl::run();
  else Fl::check();

  // Export the scenario to an XML file
  model->ExportGraphToXML(oufname);

  model->GetGraphvizDotFile (oufname2);

  //viewerModule->bQuit->do_callback();
  view->Quit();

  return 0;
}
