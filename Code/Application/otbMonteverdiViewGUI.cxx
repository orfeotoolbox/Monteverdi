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
  m_Tree = new Flu_Tree_Browser( 0, 0, 300, 460, "Tree test" );

  // Build the GUI
  this->InitWidgets();

}


MonteverdiViewGUI
::~MonteverdiViewGUI()
{
}

void
MonteverdiViewGUI
::InitWidgets()
{
//  assert(m_MonteverdiController.IsNotNull() && "The controller is not created");

  // Generate dynamicaly the menus with the Model informations
  this->BuildMenus();

  this->BuildTree();


}

void
MonteverdiViewGUI
::BuildMenus()
{

}

void
MonteverdiViewGUI
::BuildTree()
{
 //tree = new Flu_Tree_Browser( 200, 0, 300, 460, "Tree test" );
  m_Tree->box( FL_DOWN_BOX );
  m_Tree->auto_branches( true );
  m_Tree->label( "Tree Browser Demo" );

 // gTreeGroup->resizable( NULL );

//   newNode = new Fl_Input( 50, 370, 100, 20, "Name" );
//   newNode->value( "Node" );
// 
//   addChild = new Fl_Button( 10, 400, 100, 20, "Add Child" );
//   addChild->callback( addChildCB, NULL );
// 
//   removeBtn = new Fl_Button( 10, 430, 100, 20, "Remove" );
//   removeBtn->callback( removeCB, NULL );


  //gTreeGroup->end();

  // FileGroup and tree
//   gTreeGroup->add(m_DatasTree);
//   gTreeGroup->show();
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
  wMainWindow->hide();
  //MsgReporter::GetInstance()->Hide();
}


} // end namespace otb


