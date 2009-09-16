#include <stdio.h>
#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Pixmap.H>
#include <FL/fl_ask.H>

#include <FLU/Flu_Button.h>
#include <FLU/Flu_Return_Button.h>
#include <FLU/Flu_Link_Button.h>
#include <FLU/flu_pixmaps.h>

static void cb( Fl_Widget*, void* )
{
  fl_alert( "Hello!" );
}

int main( int argc, char **argv )
{
  Fl_Pixmap computer( (char*const*)computer_xpm );
  Fl_Double_Window *win = new Fl_Double_Window( 100, 100, 300, 300, "Flu_Button_Try" );
  Flu_Button *b = new Flu_Button( 20, 100, 60, 30, "Button" );
  b = new Flu_Button( 100, 100, 60, 30 );
  b->image( computer );
  b->deactivate();
  b = new Flu_Button( 180, 100, 60, 30 );
  b->type( FL_TOGGLE_BUTTON );
  b->image( computer );
  Flu_Return_Button *r = new Flu_Return_Button( 20, 150, 60, 30, "Ok" );
  Flu_Link_Button *l = new Flu_Link_Button( 100, 150, 60, 30, "Click Me" );
  l->callback( cb );
  l = new Flu_Link_Button( 100, 180, 60, 30, "Click Me" );
  l->overlink( true );
  l->callback( cb );
  b = new Flu_Button( 180, 150, 60, 30, "@->" );
  b->box( FL_FLAT_BOX );
  b->enter_box( FL_UP_BOX );

  win->end();
  win->resizable( win );

  win->show();

  return( Fl::run() );
}
