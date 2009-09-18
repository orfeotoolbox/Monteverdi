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

//Fl_Menu_Item mProcessing = {"Processing", 0,  0, 0, 0, FL_NORMAL_LABEL, 0, 14, 0};
//mProcessing.SetText("Processing");
//mMenuBar->add("Processing", 0,  0, 0, FL_NORMAL_LABEL);

Fl_Callback *the_cb;
Fl_Callback *strange_cb;


// Fl_Menu_Item menu_menuItem[] = {
//   {"&alpha",   FL_ALT+'a', the_cb, (void*)1},
//   {"&beta",    FL_ALT+'b', the_cb, (void*)2},
//   {"gamma",    FL_ALT+'c', the_cb, (void*)3, FL_MENU_DIVIDER},
//   {"&strange",  0,   strange_cb},
//   {"&charm",    0,   strange_cb},
//   {"&truth",    0,   strange_cb},
//   {"b&eauty",   0,   strange_cb},
//   {"sub&menu",  0,   0, 0, FL_SUBMENU},
//     {"one"},
//     {"two"},
//     {"three"},
//   {0},
//   {"inactive", FL_ALT+'i', 0, 0, FL_MENU_INACTIVE|FL_MENU_DIVIDER},
//   {"invisible",FL_ALT+'i', 0, 0, FL_MENU_INVISIBLE},
//   {"check",    FL_ALT+'i', 0, 0, FL_MENU_TOGGLE|FL_MENU_VALUE},
//   {"box",      FL_ALT+'i', 0, 0, FL_MENU_TOGGLE},
// {0}};
// mMenuBar->copy(menu_menuItem);


// mMenuBar->add("Processing",  0,   0, 0, FL_SUBMENU);
// mMenuBar->add("Processing/&strange",  0,   0);
// mMenuBar->add("&beta",    FL_ALT+'b', 0, (void*)2,FL_MENU_DIVIDER);
// mMenuBar->add("processing/gamma",    FL_ALT+'c', 0, (void*)3);
// mMenuBar->add("sub&menu",  0,   0, 0, FL_SUBMENU);

  for(mcIt = lModuleDescriptorMap.begin();mcIt != lModuleDescriptorMap.end();mcIt++)
  {


    if(mcIt->first =="Reader" )
        mMenuBar->add("File/Open Data Set", 0, 0, (void *)Test_cb);

    if(mcIt->first =="Writer" )
        mMenuBar->add("File/Save Data Set", 0, 0, 0);

    if(mcIt->first =="Speckle" )
        mMenuBar->add("Processing/Speckle", 0, 0, 0);

    if(mcIt->first =="OrthoRectification" )
        mMenuBar->add("Processing/OrthoRectification", 0, 0, 0);

//     std::ostringstream out;
//     out << mcIt->first;
//     std::cout<< out <<std::endl;
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
    m_Tree->add(mcIt->first.c_str());

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


