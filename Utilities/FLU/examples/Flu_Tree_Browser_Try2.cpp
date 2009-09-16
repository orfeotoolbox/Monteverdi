#include <stdio.h>
#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Counter.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Float_Input.H>
#include <FL/Fl_Button.H>
#include <FL/fl_ask.H>
#include <FL/fl_show_colormap.H>

#include <FLU/Flu_Tree_Browser.h>
#include <FLU/flu_pixmaps.h>

Flu_Tree_Browser *tree;
Fl_Input *newNode;
Fl_Button *addChild, *removeBtn;
int next = 1;

Fl_Pixmap blue_dot( (char*const*)bluedot_xpm ), green_dot( (char*const*)greendot_xpm ), red_dot( (char*const*)reddot_xpm ), teal_dot( (char*const*)tealdot_xpm ), 
  text_doc( (char*const*)textdoc_xpm ), computer( (char*const*)computer_xpm ), book( (char*const*)book_xpm ), cd_drive( (char*const*)cd_drive_xpm ),
  arrow_closed( (char*const*)arrow_closed_xpm ), arrow_open( (char*const*)arrow_open_xpm );

static void removeCB( Fl_Widget*, void* )
{
  Flu_Tree_Browser::Node* n = tree->get_selected( 1 );
  if( !n )
    return;
  tree->remove( n );
  //const char *path = n->find_path();
  //tree->remove( path );
  tree->redraw();
}

static void addChildCB( Fl_Widget*, void* )
{
  Flu_Tree_Browser::Node* n = tree->get_selected( 1 );
  if( !n )
    n = tree->last();
  n->add( newNode->value() );
  tree->redraw();

  char buf[32];
  sprintf( buf, "Node%d", next++ );
  newNode->value( buf );
}

int main( int argc, char **argv )
{
  Fl_Double_Window *win = new Fl_Double_Window( 500, 460, "Flu_Tree_Browser_Try2" );

  tree = new Flu_Tree_Browser( 200, 0, 300, 460 );
  tree->box( FL_DOWN_BOX );
  tree->auto_branches( true );
  tree->label( "Tree Browser Demo" );

  Fl_Group *g = new Fl_Group( 0, 0, 200, 460 );
  g->resizable( NULL );

  newNode = new Fl_Input( 50, 370, 100, 20, "Name" );
  newNode->value( "Node" );

  addChild = new Fl_Button( 10, 400, 100, 20, "Add Child" );
  addChild->callback( addChildCB, NULL );

  removeBtn = new Fl_Button( 10, 430, 100, 20, "Remove" );
  removeBtn->callback( removeCB, NULL );

  g->end();

  win->end();
  win->resizable( tree );
  win->show( argc, argv );

  return( Fl::run() );
}
