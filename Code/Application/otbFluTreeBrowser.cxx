// $Id: Flu_Tree_Browser.cpp,v 1.126 2004/11/05 17:03:20 jbryan Exp $

/***************************************************************
 *                FLU - FLTK Utility Widgets
 *  Copyright (C) 2002 Ohio Supercomputer Center, Ohio State University
 *
 * This file and its content is protected by a software license.
 * You should have received a copy of this license with this file.
 * If not, please contact the Ohio Supercomputer Center immediately:
 * Attn: Jason Bryan Re: FLU 1224 Kinnear Rd, Columbus, Ohio 43212
 *
 ***************************************************************/

#include "otbFluTreeBrowser.h"
#include <iostream>

namespace otb
{

 FluTreeBrowser
::FluTreeBrowser( int x, int y, int w, int h, const char *label ) : Flu_Tree_Browser(x, y, w, h, label)
 {
   m_ModuleMenu = new MonteverdiMenu(0, 0, 100, 200);
 }

 FluTreeBrowser
 ::~FluTreeBrowser()
 {
    delete m_ModuleMenu;
 }

int
FluTreeBrowser
::handle( int event )
{
#ifdef USE_FLU_DND
  if( dnd_handle( event ) )
    return 1;
#endif

  if( event == FL_NO_EVENT )//|| event == FL_MOVE )
    return 0;

  if( event == FL_FOCUS )//&& rdata.lastHilighted )
    {
      this->redraw();
      return 1;
    }

  if( event == FL_UNFOCUS )
    {
        Fl_Group::handle( event );
      this->redraw();
      return 1;
    }

  if( !rdata.dragging && !( event == FL_MOVE && rdata.selectUnderMouse ) )
    {
      if( ! (event == FL_MOVE || event == FL_ENTER || event == FL_LEAVE ) )
       _box->redraw();

      if( Fl_Group::handle( event ) )
       {
         return 1;
       }
    }

  if( event == FL_RELEASE )
    {
      this->rdata.dragging = false;
      this->rdata.grabbed = 0;
      this->rdata.dragNode = 0;
    }
  
 

  int dx = Fl::box_dx(box()), dy = Fl::box_dy(box());

  // set some initial values for the recursive data structure
  // account for the scrollbar positions
  this->rdata.x = this->x()+dx; this->rdata.y = this->y()+dy;
  if( this->scrollH->visible() )
    this->rdata.x -= this->scrollH->value();
  if( scrollV->visible() )
    this->rdata.y -= this->scrollV->value();

  this->rdata.previous = NULL;
  this->rdata.delta = 0;
  this->rdata.visibilityChanged = false;

  // catch cursor keys for moving the hilighted entry or selecting all entries
  if( event == FL_KEYDOWN )
    {
      // move hilighted entry up
      if( Fl::event_key() == FL_Up )
       {
         this->rdata.delta = -1;
         Fl::focus(this);
         this->redraw();
       }

      // move hilighted entry down
      else if( Fl::event_key() == FL_Down )
       {
         this->rdata.delta = 1;
         Fl::focus(this);
         this->redraw();
       }

      // select all
      else if( Fl::event_state(FL_CTRL) && Fl::event_key() == 'a' )
       {
         select_all();
         Fl::focus(this);
         this->redraw();
         return 1;
       }

      // check for the Home key
      else if( Fl::event_key() == FL_Home )
       {
         // set the hilighted entry to be the first entry
         if( this->rdata.showRoot || ( this->rdata.root->children() == 0 ) )
           this->set_hilighted( rdata.root );
         else if( this->rdata.root->children() > 0 )
           this->set_hilighted( this->rdata.root->child(0) );
         this->redraw();
       }

      // check for the End key
      else if( Fl::event_key() == FL_End )
       {
         // set the hilighted entry to be the last visible entry
         if( this->rdata.showRoot && ( this->rdata.root->children() == 0 ) )
           this->set_hilighted( this->rdata.root );
         else
           {
             // find the last node by repeatedly looking for the last child until there are no more branches
             Node *n = &root;
             while( n->children() && n->open() )
              n = n->child( n->children()-1 );
             set_hilighted( n );
           }
         this->redraw();
       }
    }

   // pass the event down the tree
  int val = this->root.recurse( rdata, NodeType::HANDLE, event );
  if( val )
    {
      //redraw();
      if( this->rdata.visibilityChanged )
       {
         this->root.determineVisibility();
       }
      if( val == 1 )
       {
           if( event == FL_PUSH && Fl::event_button()==FL_RIGHT_MOUSE)
             {
               this->get_hilighted()->do_callback(OTB_FLU_RIGHT_MOUSE_PUSHED);
             }
         return 1;
       }
    }
  // special case: if multi-select or single-select and user clicks on no items, unselect all items
  else if( (this->rdata.selectionMode != FLU_NO_SELECT) && (event == FL_PUSH) && (!Fl::event_state(FL_CTRL)) )
    {
      this->unselect_all();
      this->set_hilighted( NULL );
      this->rdata.forceResize = true;
      this->redraw();

      return 1;
    }

  if( event == FL_SHOW || event == FL_HIDE )
    this->root.determineVisibility();

  return Fl_Group::handle( event );
  //return 0;
}

}// namespace otb

