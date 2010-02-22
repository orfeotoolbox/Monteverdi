#include <stdio.h>
#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FLU/Flu_Spinner.h>

static void update( Fl_Widget *w, void *arg )
{
  printf( "%g\n", ((Flu_Spinner*)w)->value() );
}

int main( int argc, char **argv )
{
  Fl_Double_Window *win = new Fl_Double_Window( 100, 100, 300, 300, "Flu_Spinner_Try" );

  Flu_Spinner *c = new Flu_Spinner( 100, 30, 70, 25, "Float Spinner" );
  c->callback( update );
  //c->deactivate();

  c = new Flu_Spinner( 100, 70, 70, 25, "Int Spinner" );
  //c->editable( false );
  c->callback( update );
  c->step( 1 );
  c->range( 0, 10 );

  c = new Flu_Spinner( 100, 110, 70, 25, "X: " );
  c->callback( update );
  c->align( FL_ALIGN_INSIDE );
  c->value( 0.5f );

  c = new Flu_Spinner( 100, 150, 70, 25, "Wrap Spinner" );
  c->callback( update );
  c->wrap_range( true );
  c->step( 1 );
  c->range( 0, 10 );

  win->end();
  win->resizable( win );

  win->show();

  return( Fl::run() );
}
