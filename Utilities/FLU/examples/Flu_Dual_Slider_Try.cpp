#include <FL/Fl_Double_Window.H>
#include <FLU/Flu_Dual_Slider.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

static void cb( Fl_Widget *w, void* )
{
  Flu_Dual_Slider *d = (Flu_Dual_Slider*)w;
  printf( "%g  %g\n", d->low_value(), d->high_value() );
}

int main( int argc, char **argv )
{
  Fl_Double_Window *win = new Fl_Double_Window( 100, 100, 400, 400, "Flu_Dual_Slider_Try" );

  Flu_Dual_Slider *s = new Flu_Dual_Slider( 50, 10, 300, 20, "FL_HOR_NICE_SLIDER" );
  s->type( FL_HOR_NICE_SLIDER );
  //s->box( FL_FLAT_BOX);
  s->callback( cb );
  s = new Flu_Dual_Slider( 50, 50, 300, 20, "FL_HOR_SLIDER" );
  s->type( FL_HOR_SLIDER );
  s->callback( cb );
  s->range( 255, 0 );
  s->precision(0);
  s->high_value( 255 );
  s = new Flu_Dual_Slider( 90, 100, 20, 200, "FL_VERT_NICE_SLIDER" );
  s->type( FL_VERT_NICE_SLIDER );
  s->callback( cb );
  s->range( 1, 0 );
  s = new Flu_Dual_Slider( 310, 100, 20, 200, "FL_VERT_SLIDER" );
  s->range( 0, 255 );
  s->precision(0);
  s->high_value( 255 );
  s->type( FL_VERT_SLIDER );
  s->callback( cb );

  win->end();
  win->resizable( win );

  win->show();

  return( Fl::run() );
}
