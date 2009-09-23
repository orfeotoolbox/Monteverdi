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

#include "otbMonteverdiViewGUI.h"

#include <FL/fl_ask.H>

#include "base/ossimFilename.h"
#include "base/ossimDirectory.h"
#include "otbMacro.h"
#include "itkExceptionObject.h"
#include <iostream>
//#include "otbMsgReporter.h"
#include "otbInputViewGUI.h"



namespace otb
{

MonteverdiViewGUI
::MonteverdiViewGUI()
{
  // Model pointer and listener registration
  m_MonteverdiModel = MonteverdiModel::GetInstance();
  m_MonteverdiModel->RegisterListener(this);

  // Files Tree
  m_Tree = new Flu_Tree_Browser( 5, 30, 390, 565, "Tree " );

  // Build the structure of the GUI (MonteverdiViewGroup)
  this->Build();
}


MonteverdiViewGUI
::~MonteverdiViewGUI()
{
  // delete the param list built into BuildMenus
//   while(m_vector_param.size()!=0)
//     delete m_vector_param.pop_back();
}



void
MonteverdiViewGUI
::InitWidgets()
{
  assert(m_MonteverdiController.IsNotNull() && "The controller is not created");

  // Generate dynamicaly the menus with the Model informations
  this->BuildMenus();

  // Generate dynamicaly the tree
  this->BuildTree();
  gHelpText->value("Quelque chose");
  gHelpText->redraw();
}

/** First step of Init Widgets: creation of the menus */
void
MonteverdiViewGUI
::BuildMenus()
{
  const ModuleDescriptorMapType & lModuleDescriptorMap = m_MonteverdiModel->GetRegisteredModuleDescriptors();
  ModuleDescriptorMapType::const_iterator mcIt;

  mMenuBar->add("File", 0, 0, 0, FL_SUBMENU);

  for(mcIt = lModuleDescriptorMap.begin();mcIt != lModuleDescriptorMap.end();mcIt++)
  {

    /** CallbackParameterType is needed to pass two parameters to our callback method. 
      * Indeed, to call "CreateModuleByKey" and create instances of a module, we will both need the controller and the key of the module.
      * Futhermore,  m_vector_param is need to save the adresses of these parameters to be able to delete them in the end ! 
      */
    CallbackParameterType *param = new CallbackParameterType(this,mcIt->second.m_Key);
    m_vector_param.push_back( param );
    mMenuBar->add(mcIt->second.m_MenuPath.c_str(), 0, (Fl_Callback *)MonteverdiViewGUI::GenericCallback,(void *)(param));
  }

 
  // In the end
  mMenuBar->add("File/Quit", 0, (Fl_Callback *)MonteverdiViewGUI::QuitCallback, (void*)(this));
  mMenuBar->add("?/Help",0, (Fl_Callback *)MonteverdiViewGUI::HelpCallback, (void*)(this));
}

/** Second step of Init Widgets : creation of the tree */
void
MonteverdiViewGUI
::BuildTree()
{

  wMainWindow->begin();

  m_Tree->box( FL_DOWN_BOX );
  m_Tree->auto_branches( true );
  m_Tree->label( "Tree Browser" );

  // animate the tree
  m_Tree->animate( 1 );
  m_Tree->collapse_time( 0.02 );
  m_Tree->frame_rate(500);

  gTreeGroup->resizable(m_Tree);
  wMainWindow->resizable(gTreeGroup);
  gTreeGroup->end();

  // FileGroup and tree
  gTreeGroup->add(m_Tree);
  gTreeGroup->show();

}

/** BuildInputsGUI create an instance of a small GUI where the user will select his inputs
  * The number and the kind of inputs will be choosen considering the expectation of a 
  * concerned module (moduleInstanceId)
  * Note : when no input is required, the GUI must not appear! ( -> skip )
  */
void
MonteverdiViewGUI
::BuildInputsGUI(const std::string & moduleInstanceId)
{

  bool skip  = false;


      // look after all expected or optionnal input datas
      InputDataDescriptorMapType lInputDataMap = m_MonteverdiModel->GetModuleInputsByInstanceId(moduleInstanceId);
      if(lInputDataMap.size() == 0)
      {
        skip =true;
      }

      if(!skip){
        InputViewGUI::Pointer inputViewGUI = InputViewGUI::New();

        inputViewGUI->SetModel(m_MonteverdiModel);
        inputViewGUI->SetController(m_MonteverdiController);
        inputViewGUI->SetModuleInstanceId(moduleInstanceId);

        inputViewGUI->BuildInputInterface();
        inputViewGUI->Show();
      }

}






/** GenericCallback (static)
  *
  * Because this method is called from a button into the Fl_Menu_Bar (cf. BuildMenus), 
  * "CreateModuleByKey_Callback" must be static. Problem : in this method must use 
  * "this" which is not static ! 
  */
void 
MonteverdiViewGUI
::GenericCallback(Fl_Menu_* w, void* v)
{
  CallbackParameterType *param = static_cast<CallbackParameterType *>(v);

  MonteverdiViewGUI *lThis = param->first;
  std::string moduleKey = param->second;

  // each call to this callback create a new instance of a module
  lThis->CreateModuleByKey(moduleKey.c_str());
}

/** QuitCallback (static) */
void MonteverdiViewGUI::QuitCallback(Fl_Menu_* o, void* v) 
{
  MonteverdiViewGUI *lThis = (MonteverdiViewGUI *)v;
  lThis->Quit();
}

/** HelpCallback (static) */
void MonteverdiViewGUI::HelpCallback(Fl_Menu_* o, void* v) 
{
  MonteverdiViewGUI *lThis = (MonteverdiViewGUI *)v;
  lThis->Help();
}



void
MonteverdiViewGUI
::CreateModuleByKey(const char * modulekey)
{
  m_MonteverdiController->CreateModuleByKey(modulekey);
}



/** The tree is updated when a notifaction is received with the Event type "Output" */
void
MonteverdiViewGUI
::UpdateTree(const std::string & instanceId)
{

      Flu_Tree_Browser::Node* root = m_Tree->first();


      // add a new branch for a new instance of module
      root->add_branch(instanceId.c_str());
      /*Flu_Tree_Browser::Node* node =*/ 

      //NodeDescriptor descr = new NodeDescriptor();

      // look after all outputdatas into each instance of module
      OutputDataDescriptorMapType lDataMap = m_MonteverdiModel->GetModuleOutputsByInstanceId(instanceId);
      OutputDataDescriptorMapType::const_iterator it;
      for (it = lDataMap.begin();it != lDataMap.end();it++)
      {
          // we look for the good node in the tree to add leaves
          Flu_Tree_Browser::Node* n = m_Tree->find(instanceId.c_str());
          if( !n )
            n = m_Tree->last();
          // add informations to the targeted module
          n->add(it->second.GetDataKey().c_str());
          n->add(it->second.GetDataType().c_str());
          n->add(it->second.GetDataDescription().c_str());
      } // end datas loop

}




void
MonteverdiViewGUI
::Notify(const MonteverdiEvent & event)
{

  std::cout<<"View: Received event "<<event.GetType()<<" from module "<<event.GetInstanceId()<<std::endl;


  // Event received : new instance of module is created 
  // -> Open a inputs Window
  if(event.GetType() == "InstanceCreated" ){
    this->BuildInputsGUI(event.GetInstanceId());
  }

  // event received : module has changed
  else if(event.GetType() == "OutputsUpdated" ){
    this->UpdateTree(event.GetInstanceId());
  }

  // Event received : UNKNOWN EVENT
  else
  {
   // itkExceptionMacro(<<event.GetType()<<" is an unknown event.");
  }
}

void
MonteverdiViewGUI
::Show()
{
  wMainWindow->show();
}

void
MonteverdiViewGUI
::Quit()
{
  gTreeGroup->hide();
  wHelpWindow->hide();
  wMainWindow->hide();
}

void
MonteverdiViewGUI
::Help()
{
  wHelpWindow->show();
}

} // end namespace otb


