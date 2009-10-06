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

#include <iostream>
#include <cassert>

#include <FL/fl_ask.H>

#include "base/ossimFilename.h"
#include "base/ossimDirectory.h"
#include "otbMacro.h"
#include "itkExceptionObject.h"
#include "flu_pixmaps.h"


namespace otb
{
Fl_Pixmap blue_dot( (char*const*)bluedot_xpm ), green_dot( (char*const*)greendot_xpm ), red_dot( (char*const*)reddot_xpm ), teal_dot( (char*const*)tealdot_xpm ), 
  text_doc( (char*const*)textdoc_xpm ), computer( (char*const*)computer_xpm ), book( (char*const*)book_xpm ), cd_drive( (char*const*)cd_drive_xpm ),
  arrow_closed( (char*const*)arrow_closed_xpm ), arrow_open( (char*const*)arrow_open_xpm ), home( (char*const*)home_xpm ), purple_dot( (char*const*)purpledot_xpm );



MonteverdiViewGUI
::MonteverdiViewGUI()
{
  // Files Tree
  m_Tree = new Flu_Tree_Browser( 5, 30, 390, 565, "Tree " );

  // Build the structure of the GUI (MonteverdiViewGroup)
  this->Build();

  m_MonteverdiModel = MonteverdiModel::GetInstance();
  m_MonteverdiModel->RegisterListener(this);
}

MonteverdiViewGUI
::~MonteverdiViewGUI()
{}

void
MonteverdiViewGUI
::InitWidgets()
{
  assert(m_MonteverdiController!=NULL && "The controller is not created");

  // Generate dynamicaly the menus with the Model informations
  this->BuildMenus();

  // Generate dynamicaly the tree
  this->BuildTree();
  gHelpText->value(" Someone please call 911 ");
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
    mMenuBar->add(mcIt->second.m_MenuPath.c_str(), 0, (Fl_Callback *)MonteverdiViewGUI::GenericCallback,(void *)(mcIt->second.m_Key.c_str()));
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

  // allow callback with the tree
  m_Tree->box( FL_DOWN_BOX );
  m_Tree->allow_dnd( true );

  //m_Tree->when( FL_WHEN_RELEASE );
  m_Tree->callback( TreeCallback );

  // animate the tree
  m_Tree->animate( true );
  m_Tree->collapse_time( 0.02 );
  m_Tree->frame_rate(500);

  Flu_Tree_Browser::Node* root = m_Tree->first();
  root->open(true);
  root->label("Data Set");

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
  
  if(!skip)
    {
    m_InputViewGUI = InputViewGUI::New();   
    m_InputViewGUI->SetModel(m_MonteverdiModel);
    m_InputViewGUI->SetController(m_MonteverdiController);
    m_InputViewGUI->SetModuleInstanceId(moduleInstanceId);
    m_InputViewGUI->BuildInputInterface();
    m_InputViewGUI->Show();
    }
  else
    {
    m_MonteverdiController->StartModuleByInstanceId(moduleInstanceId);
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
  MonteverdiViewGUI * pthis = static_cast<MonteverdiViewGUI *>(w->parent()->user_data());
  const char* moduleKey = static_cast<const char *>(v);

  // each call to this callback create a new instance of a module
  pthis->CreateModuleByKey(moduleKey);
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

/** TreeCallback (static) */
void MonteverdiViewGUI::TreeCallback( Fl_Widget* w, void* v )
{
  Flu_Tree_Browser *t = (Flu_Tree_Browser*)w;
  Fl_Menu_Window *coolMenu = new Fl_Menu_Window(300,100,100,300);
  coolMenu->clear_overlay();
  coolMenu->hide();
  
  int reason = t->callback_reason();
  Flu_Tree_Browser::Node *n = t->callback_node();
  
  switch( reason )
  {
    case FLU_HILIGHTED:
      printf( "%s hilighted\n", n->label() );
      break;
  
    case FLU_UNHILIGHTED:
      printf( "%s unhilighted\n", n->label() );
      break;

    case FLU_SELECTED:
      printf( "%s selected\n", n->label() );
      break;

    case FLU_UNSELECTED:
      printf( "%s unselected\n", n->label() );
      break;

    case FLU_OPENED:
      printf( "%s opened\n", n->label() );
      break;

    case FLU_CLOSED:
      printf( "%s closed\n", n->label() );
      break;

    case FLU_DOUBLE_CLICK:
      printf( "%s double-clicked\n", n->label() );
      coolMenu->show();
//       gTree->remove(n);
//       gTree->redraw();
      break;

    case FLU_WIDGET_CALLBACK:
      printf( "%s widget callback\n", n->label() );
      break;

    case FLU_MOVED_NODE:
      printf( "%s moved\n", n->label() );
      break;

    case FLU_NEW_NODE:
      printf( "node '%s' added to the tree\n", n->label() );
      break;
  }
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
  root->label("Data Set");

  // look after all outputdatas into each instance of module
  OutputDataDescriptorMapType lDataMap = m_MonteverdiModel->GetModuleOutputsByInstanceId(instanceId);
  OutputDataDescriptorMapType::const_iterator it;
  for (it = lDataMap.begin();it != lDataMap.end();it++)
    {
    // we look for the good node in the tree to add leaves
    Flu_Tree_Browser::Node* n = m_Tree->find(instanceId.c_str());
    if( !n )
      n = m_Tree->last();

    n->open(true);

    // this node can receive new nodes as a result of draggind-and-dropping
    //TODO
    //n->droppable(true);
    //n->movable(true);

    Flu_Tree_Browser::Node* new_node = n->add_branch(it->second.GetDataKey().c_str());

    // add informations to the targeted module
    new_node->add(it->second.GetDataDescription().c_str());


    new_node->add(it->second.GetDataType().c_str());

    //new_node->open(close);
    n->branch_icons( &book,&book );
    new_node->branch_icons( &blue_dot,&purple_dot );
    } // end datas loop
}

void
MonteverdiViewGUI
::Notify(const MonteverdiEvent & event)
{

  std::cout<<"View: Received event "<<event.GetType()<<" from module "<<event.GetInstanceId()<<std::endl;


  // Event received : new instance of module is created
  // -> Open a inputs Window
  if(event.GetType() == "InstanceCreated" )
    {
    this->BuildInputsGUI(event.GetInstanceId());
    }
  // event received : module has changed
  else if(event.GetType() == "OutputsUpdated" )
    {
    this->UpdateTree(event.GetInstanceId());
    }
  // event received : module instanceID has changed
  else if(event.GetType() == "ChangeInstanceId" )
    {

      std::cout<<"event.GetInstanceId(): "<<event.GetInstanceId()<<std::endl;
    this->UpdateTree(event.GetInstanceId());
    }
  // Event received : UNKNOWN EVENT
  else
    {
    itkExceptionMacro(<<event.GetType()<<" is an unknown event.");
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


