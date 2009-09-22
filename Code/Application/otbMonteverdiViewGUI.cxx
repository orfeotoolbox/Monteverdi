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
#include <FL/Fl_File_Chooser.H>
#include <FL/fl_ask.H>

#include "base/ossimFilename.h"
#include "base/ossimDirectory.h"
#include "otbMacro.h"
#include "itkExceptionObject.h"
#include <iostream>
//#include "otbMsgReporter.h"




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


void
MonteverdiViewGUI
::AddChild( std::string childname )
{
  Flu_Tree_Browser::Node* n = m_Tree->get_selected( 1 );

  if( !n )
    n = m_Tree->last();
  n->add( childname.c_str() );
  m_Tree->redraw();

}

/** The tree is updated when a notifaction is received with the Event type "Output" */
void
MonteverdiViewGUI
::UpdateTree(const MonteverdiEvent & event)
{

//    event.GetType()
//    event.GetInstanceId()
std::cout<<"event.GetInstanceId() : "<<event.GetInstanceId()<<std::endl;


  // for each instance of module
  std::vector<std::string> moduleInstances = m_MonteverdiModel->GetAvailableModuleInstanceIds();
std::cout<<"size : "<<moduleInstances.size()<<std::endl;
  unsigned int i;
  for(i=0;i<moduleInstances.size();i++)
  {

std::cout<<"moduleInstances[i]: "<<moduleInstances[i]<<std::endl;

//     // we build ONLY the new branch (targeted by the moduleInstanceId )
//     if(event.GetInstanceId() == moduleInstances[i] )
//     {
      Flu_Tree_Browser::Node* root = m_Tree->first();

      // add a new branch for a new instance of module
      root->add_branch(moduleInstances[i].c_str());


      // look after all outputdatas into each instance of module
      OutputDataDescriptorMapType lDataMap = m_MonteverdiModel->GetModuleOutputsByInstanceId(moduleInstances[i]);
      OutputDataDescriptorMapType::const_iterator it;
      for (it = lDataMap.begin();it != lDataMap.end();it++)
      {
          // we look for the good node in the tree to add leaves
          Flu_Tree_Browser::Node* n = m_Tree->find(moduleInstances[i].c_str());
          if( !n )
            n = m_Tree->last();
          // add informations to the targeted module
          n->add(it->second.GetDataKey().c_str());
          n->add(it->second.GetDataType().c_str());
          n->add(it->second.GetDataDescription().c_str());
      } // end datas loop
//     } //endif
  } // end moduleInstances loop
}

/** Initialization of the Tree */
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

void
MonteverdiViewGUI
::Notify(const MonteverdiEvent & event)
{
  std::cout<<"View: Received event "<<event.GetType()<<" from module "<<event.GetInstanceId()<<std::endl;

  this->UpdateTree(event);

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


