#include <stdio.h>
#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Input.H>

#include <FLU/Flu_Chat_Buffer.h>

static void sendMsg( Fl_Widget *w, void *arg )
{
  ((Flu_Chat_Buffer*)arg)->addLocalMessage( "<Local User> ", ((Fl_Input*)w)->value() );
  ((Fl_Input*)w)->value("");
}

int main( int argc, char **argv )
{
  Fl_Double_Window *win = new Fl_Double_Window( 100, 100, 400, 300, "Flu_Chat_Buffer_Try" );
  Flu_Chat_Buffer *b = new Flu_Chat_Buffer( 0, 0, 400, 200 );
  Fl_Input *in = new Fl_Input( 80, 210, 100, 30, "Send:" );
  in->when( FL_WHEN_ENTER_KEY_ALWAYS );
  in->callback( sendMsg, b );
  win->end();
  win->resizable( win );

  b->addSystemMessage( "System message" );
  b->addRemoteMessage( "<Remote User> ", "Remote user's message" );
  b->addLocalMessage( "<Local User> ", "Local user's message" );

  win->show();

  return( Fl::run() );
}
