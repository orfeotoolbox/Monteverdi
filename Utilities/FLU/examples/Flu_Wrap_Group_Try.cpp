#include <stdio.h>
#include <string.h>
#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Button.H>

#include <FLU/Flu_Spinner.h>
#include <FLU/Flu_Wrap_Group.h>

Flu_Spinner *offsetX, *offsetY, *spacingX, *spacingY;
Fl_Choice *type;
Flu_Wrap_Group *wrap;

static void value_changed( Fl_Widget *w, void *arg )
{
  wrap->offset( (int)offsetX->value(), (int)offsetY->value() );
  wrap->spacing( (int)spacingX->value(), (int)spacingY->value() );
  wrap->type( (type->value()==0) ? FL_VERTICAL : FL_HORIZONTAL );
}

int main( int argc, char **argv )
{
  Fl_Double_Window *win = new Fl_Double_Window( 100, 100, 400, 400, "Flu_Wrap_Group_Try" );

  wrap = new Flu_Wrap_Group( 20, 20, 304, 124 );
  wrap->box( FL_DOWN_BOX );
  wrap->color( FL_RED );
  {
    for( int i = 0; i < 20; i++ )
      {
	char buf[16];
	sprintf( buf, "%d", i );
	Fl_Button *l = new Fl_Button( 0, 0, 60, 30, strdup(buf) );
      }
  }
  wrap->end();
  win->resizable( wrap );

  offsetX = new Flu_Spinner( 80, 210, 50, 20, "Offset X" );
  offsetX->range( 0, 10 );
  offsetX->precision( 0 );
  offsetX->callback( value_changed );
  offsetY = new Flu_Spinner( 150, 210, 50, 20, "Y" );
  offsetY->range( 0, 10 );
  offsetY->precision( 0 );
  offsetY->callback( value_changed );
  spacingX = new Flu_Spinner( 80, 240, 50, 20, "Spacing X" );
  spacingX->range( 0, 10 );
  spacingX->precision( 0 );
  spacingX->callback( value_changed );
  spacingY = new Flu_Spinner( 150, 240, 50, 20, "Y" );
  spacingY->range( 0, 10 );
  spacingY->precision( 0 );
  spacingY->callback( value_changed );

  type = new Fl_Choice( 50, 270, 160, 20, "Type" );
  type->add( "FL_VERTICAL" );
  type->add( "FL_HORIZONTAL" );
  type->value( 0 );
  type->callback( value_changed );

  win->end();
  win->show();

  return( Fl::run() );
}
