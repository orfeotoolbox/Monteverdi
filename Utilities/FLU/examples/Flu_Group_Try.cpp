#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Button.H>

#include <FLU/Flu_Toggle_Group.h>
#include <FLU/Flu_Choice_Group.h>
#include <FLU/Flu_Simple_Group.h>
#include <FLU/Flu_Collapsable_Group.h>

int main( int argc, char **argv )
{
  Fl_Double_Window *win = new Fl_Double_Window( 0, 100, 1200, 300, "Flu_Group_*_Try" );
  {
    Flu_Simple_Group *g = new Flu_Simple_Group( 20, 20, 250, 260, "Simple Group" );
    Fl_Button *b = new Fl_Button( 100, 100, 60, 30, "Button" );
    g->end();
  }
  {
    Flu_Toggle_Group *g = new Flu_Toggle_Group( 320, 20, 250, 260, "Toggle Group" );
    Fl_Button *b = new Fl_Button( 350, 100, 60, 30, "Button" );
    g->end();
    g->activate();
  }
  {
    Flu_Choice_Group *g = new Flu_Choice_Group( 620, 20, 250, 260 );
    Fl_Group *gr = new Fl_Group( 630, 50, 200, 200, "Choice Group" );
    Fl_Button *b = new Fl_Button( 650, 100, 60, 30, "Button 1" );
    gr->end();
    gr = new Fl_Group( 630, 50, 200, 200, "Group 2" );
    b = new Fl_Button( 670, 100, 60, 30, "Button 2" );
    gr->end();
    gr = new Fl_Group( 630, 50, 200, 200, "Group 3" );
    b = new Fl_Button( 690, 100, 60, 30, "Button 3" );
    gr->end();
    g->end();
  }
  {
    Flu_Collapsable_Group *g = new Flu_Collapsable_Group( 920, 20, 250, 260, "Collapsable Group" );
    Fl_Button *b = new Fl_Button( 950, 100, 60, 30, "Button1" );
    b = new Fl_Button( 950, 130, 60, 30, "Button2" );
    b = new Fl_Button( 950, 160, 60, 30, "Button3" );
    g->end();
  }
  win->end();
  win->resizable( win );

  win->show();

  return( Fl::run() );
}
