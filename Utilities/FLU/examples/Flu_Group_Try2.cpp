#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Pack.H>
#include <FL/Fl_Scroll.H>

#include <FLU/Flu_Simple_Group.h>
#include <FLU/Flu_Collapsable_Group.h>
#include <FLU/Flu_Toggle_Group.h>
#include <FLU/Flu_Choice_Group.h>

/*
void grab( void * )
{
  static int frameno = 0;
  static char buf[512];

  sprintf( buf, "/usr/sbin/scrsave image_%03d.rgb 92 437 92 439", frameno++ );
  system( buf );

  Fl::add_timeout( 1.0f, grab, NULL );
}
*/

int main( int argc, char **argv )
{
  Fl_Double_Window *win = new Fl_Double_Window( 100, Fl::h()-100-310, 330, 310, "Flu_Group_*_Try" );
  Fl_Scroll *s = new Fl_Scroll( 5, 5, 320, 300 );
  s->box( FL_FLAT_BOX );
  Fl_Pack *p = new Fl_Pack( 10, 10, 290, 280 );
  p->type( Fl_Pack::VERTICAL );
  p->spacing( 5 );
  {
    Flu_Toggle_Group *g = new Flu_Toggle_Group( 20, 20, 260, 100, "Toggle Group" );
    Fl_Button *b = new Fl_Button( 50, 50, 60, 30, "Button 1" );
    b = new Fl_Button( 50, 80, 60, 30, "Button 2" );
    g->end();
  }
  {
    Flu_Collapsable_Group *g = new Flu_Collapsable_Group( 20, 20, 260, 100, "Collapsable Group 1" );
    g->collapse_time( 0.5f );
    g->frame_rate( 30.0f );
    g->fit( true );
    Fl_Button *b = new Fl_Button( 50, 50, 60, 30, "Button 1" );
    b = new Fl_Button( 50, 80, 60, 30, "Button 2" );
    g->end();
  }
  {
    Flu_Choice_Group *g = new Flu_Choice_Group( 20, 20, 260, 100 );
    {
      Fl_Group *gr = new Fl_Group( 20, 30, 260, 90, "Choice Group Tab 1" );
      {
	Fl_Button *b = new Fl_Button( 50, 50, 60, 30, "Button 1" );
	b = new Fl_Button( 50, 80, 60, 30, "Button 2" );
      }
      gr->end();
    }
    {
      Fl_Group *gr = new Fl_Group( 20, 30, 260, 90, "Choice Group Tab 2" );
      {
	Fl_Button *b = new Fl_Button( 100, 50, 60, 30, "Button 3" );
	b = new Fl_Button( 100, 80, 60, 30, "Button 4" );
      }
      gr->end();
    }
    g->end();
  }
  {
    Flu_Collapsable_Group *g = new Flu_Collapsable_Group( 20, 20, 260, 100, "Collapsable Group 2" );
    Fl_Button *b = new Fl_Button( 50, 50, 60, 30, "Button 1" );
    b = new Fl_Button( 50, 80, 60, 30, "Button 2" );
    g->end();
  }
  {
    Flu_Simple_Group *g = new Flu_Simple_Group( 20, 20, 260, 100, "Simple Group" );
    Fl_Button *b = new Fl_Button( 50, 50, 60, 30, "Button 1" );
    b = new Fl_Button( 50, 80, 60, 30, "Button 2" );
    g->end();
  }
  {
    Flu_Simple_Group *g = new Flu_Simple_Group( 20, 20, 260, 100 );
    Fl_Button *b = new Fl_Button( 50, 50, 60, 30, "Button 1" );
    b = new Fl_Button( 50, 80, 60, 30, "Button 2" );
    g->end();
  }
  p->end();
  s->end();
  win->end();
  win->resizable( s );

  win->show();

  //Fl::add_timeout( 1.0f, grab, NULL );

  return( Fl::run() );
}
