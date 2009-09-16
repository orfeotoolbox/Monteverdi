#include <stdio.h>
#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Button.H>
#include <FLU/Flu_Progress.h>
#include <FLU/Flu_Progress_Meter.h>

static void timeout( void *arg )
{
  Flu_Progress_Meter *m = (Flu_Progress_Meter*)arg;
  m->value( m->value() + 0.01 );

  Fl::repeat_timeout( 0.04, timeout, m );
}

static void cancel( void *arg )
{
  ((Flu_Progress_Meter*)arg)->hide();
}

static void go( Fl_Widget *w, void *arg )
{
  Flu_Progress_Meter *meter = new Flu_Progress_Meter( "Progress" );
  meter->label( "Performing operation..." );
  meter->show_completion_time( true );
  meter->set_modal();
  meter->show( true );
  meter->cancel_callback( cancel, meter );

  Fl::add_timeout( 0.04, timeout, meter );

  while( meter->value() < 1.0f && meter->shown() )
    Fl::wait(0);

  Fl::remove_timeout( timeout, meter );

  delete meter;
}

int main( int argc, char **argv )
{
  Fl_Double_Window *win = new Fl_Double_Window( 100, 100, 300, 300, "Flu_Progress_Try" );
  Flu_Progress *p1 = new Flu_Progress( 80, 40, 150, 20, "Progress" );
  p1->value( 0.25 );
  Flu_Progress *p2 = new Flu_Progress( 80, 100, 150, 30, "Progress" );
  p2->value( 0.5 );
  Flu_Progress *p3 = new Flu_Progress( 80, 170, 150, 20, "Progress" );
  p3->value( 0.75 );
  Fl_Button *b = new Fl_Button( 130, 220, 40, 30, "Go" );
  b->callback( go );
  win->end();
  win->resizable( win );

  win->show();

  return( Fl::run() );
}
