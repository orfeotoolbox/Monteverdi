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


// void MonteverdiViewGUI::cb_mQuit_i(Fl_Menu_*, void*) {
//   this->QuitCallback();
// }


void MonteverdiViewGUI::cb_mQuit(Fl_Menu_* o, void* v) {
  MonteverdiViewGUI *lThis = (MonteverdiViewGUI *)v;
  lThis->Quit();
}

void MonteverdiViewGUI::cb_wHelp(Fl_Menu_* o, void* v) 
{
  MonteverdiViewGUI *lThis = (MonteverdiViewGUI *)v;
  lThis->HelpCallback();
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

  for(mcIt = lModuleDescriptorMap.begin();mcIt != lModuleDescriptorMap.end();mcIt++)
  {

    /** CallbackParameterType is needed to pass two parameters to our callback method. 
      * Indeed, to call "CreateModuleByKey" and create instances of a module, we will both need the controller and the key of the module.
      * Futhermore,  m_vector_param is need to save the adresses of these parameters to be able to delete them in the end ! 
      */
    CallbackParameterType *param = new CallbackParameterType(m_MonteverdiController,mcIt->second.m_Key);
    m_vector_param.push_back( param );
    mMenuBar->add(mcIt->second.m_MenuPath.c_str(), 0, (Fl_Callback *)MonteverdiViewGUI::CreateModuleByKey_Callback,(void *)(param));
  }

 
  // In the end
  mMenuBar->add("File/Quit", 0, (Fl_Callback *)MonteverdiViewGUI::cb_mQuit, (void*)(this));
  mMenuBar->add("?/Help",0, (Fl_Callback *)MonteverdiViewGUI::cb_wHelp, (void*)(this));
}



/** Static method/callback : CreateModuleByKey_Callback
  *
  * Because this method is called from a button into the Fl_Menu_Bar (cf. BuildMenus), 
  * "CreateModuleByKey_Callback" must be static. Problem : in this method must use 
  * "m_MonteverdiController" which is not static ! 
  */
void
MonteverdiViewGUI
::CreateModuleByKey_Callback(Fl_Menu_* w, void* v)
{
  CallbackParameterType *param = static_cast<CallbackParameterType *>(v);

  std::string moduleKey = param->second;

  MonteverdiControllerInterface *lController = param->first;

  lController->CreateModuleByKey(moduleKey.c_str());

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


void
MonteverdiViewGUI
::BuildTree()
{

  wMainWindow->begin();

  m_Tree->box( FL_DOWN_BOX );
  m_Tree->auto_branches( true );
  m_Tree->label( "Tree Browser" );

  gTreeGroup->resizable( NULL );

  ModuleDescriptorMapType lModuleDescriptorMap = m_MonteverdiModel->GetRegisteredModuleDescriptors();
  ModuleDescriptorMapType::const_iterator mcIt;

  // for each modulus
  for(mcIt = lModuleDescriptorMap.begin();mcIt != lModuleDescriptorMap.end();mcIt++)
  {
    m_Tree->add_branch(mcIt->second.m_Key.c_str());
    AddChild("Input 1");
  }

  wMainWindow->resizable(m_Tree);
  gTreeGroup->end();

  // FileGroup and tree
  gTreeGroup->add(m_Tree);
  gTreeGroup->show();
}

void
MonteverdiViewGUI
::Notify()
{
  this->InitWidgets();
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
::HelpCallback()
{
  wHelpWindow->show();
}

void
MonteverdiViewGUI
::CreateModuleByKey(const char * modulekey)
{
  m_MonteverdiController->CreateModuleByKey(modulekey);
}


} // end namespace otb


