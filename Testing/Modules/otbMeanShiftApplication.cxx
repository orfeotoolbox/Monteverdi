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

// Include monteverdi files
#include "otbMonteverdiModel.h"
#include "otbMonteverdiViewGUI.h"
#include "otbMonteverdiController.h"

// For splash screen
#include "otbSplashScreen.h"

// For internationalisation
#include "otbI18n.h"

// Include existing modules
#include "otbReaderModule.h"

// Include the example module
#include "otbMeanShiftModule.h"


int main(int argc, char* argv[])
{
  //Internationalization
  otbI18nMacro();

  // Splash Screen
  typedef otb::SplashScreen::Pointer SplashScreenPointerType;
  SplashScreenPointerType splash = otb::SplashScreen::New();
  splash->Build();
  splash->Show();

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

  // If specific types are used in your modules, you can register
  // friendly name for them:
  otb::TypeManager::GetInstance()->RegisterType<otb::MeanShiftModule::LabelImageType>("Unsigned_Long_Image");

  // Register modules. First argument is a key to identify module, the
  // second specify where the module will appear in Monteverdi menubar
  model->RegisterModule<otb::ReaderModule>("Reader","File/Import dataset");
  model->RegisterModule<otb::MeanShiftModule>("MeanShift","MeanShiftMenu/MeanShiftSubMenu/MeanShift");

  // Launch Monteverdi
  view->InitWidgets();
  view->Show();

  // Run
  return Fl::run();
}
