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
}

void
MonteverdiViewGUI
::InitWidgets()
{
  assert(m_MonteverdiController.IsNotNull() && "The controller is not created");

  // Generate dynamicaly the menus with the Model informations
  this->BuildMenus();

  this->BuildTree();


}


static void Test_cb (Fl_Callback*, void* v)
{
  std::cout<< "first cb"<<std::endl;
  //CreateModuleByKey_Callback();
}


void
MonteverdiViewGUI
::BuildMenus()
{
  ModuleDescriptorMapType lModuleDescriptorMap = m_MonteverdiController->GetRegisteredModuleDescriptors();
  ModuleDescriptorMapType::const_iterator mcIt;

  for(mcIt = lModuleDescriptorMap.begin();mcIt != lModuleDescriptorMap.end();mcIt++)
  {
      mMenuBar->add(mcIt->second.m_MenuPath.c_str(), 0, 0, (void *)Test_cb);
  }

  // In the end
  mMenuBar->add("?/Help",0,0);
}


Fl_Callback *
MonteverdiViewGUI
::CreateModuleByKey_Callback()
{
  std::cout<<"callback"<<std::endl;
  m_MonteverdiController->CreateModuleByKey("Reader");
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

  ModuleDescriptorMapType lModuleDescriptorMap = m_MonteverdiController->GetRegisteredModuleDescriptors();
  ModuleDescriptorMapType::const_iterator mcIt;

  // for each 
  for(mcIt = lModuleDescriptorMap.begin();mcIt != lModuleDescriptorMap.end();mcIt++)
  {

    std::cout<< "ICI " << mcIt->first <<std::endl;


/// MARCHE BIEN mais pas exactement ce qu'il faut    this->AddChild(mcIt->first);
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
//     this->UpdateChannelSelection();
//     this->UpdateInformation();

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
  wMainWindow->hide();
  //MsgReporter::GetInstance()->Hide();
}

void
MonteverdiViewGUI
::CreateModuleByKey(const char * modulekey)
{
  m_MonteverdiController->CreateModuleByKey(modulekey);
}


} // end namespace otb


