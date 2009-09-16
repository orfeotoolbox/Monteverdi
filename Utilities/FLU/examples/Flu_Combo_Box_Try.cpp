#include <stdio.h>
#include <string.h>
#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Choice.H>

#include <FLU/Flu_Combo_List.h>
#include <FLU/Flu_Combo_Tree.h>

static void list_input_callback( Fl_Widget *w, void *arg )
{
  for( int i = 1; i < ((Flu_Combo_List*)w)->list.size(); i++ )
    if( strcmp( ((Flu_Combo_List*)w)->value(), ((Flu_Combo_List*)w)->list.text(i) ) == 0 )
      return;
  ((Flu_Combo_List*)w)->list.add( ((Flu_Combo_List*)w)->value() );
  printf( "added/selected: %s\n", ((Flu_Combo_List*)w)->value() );
}

static void list_callback( Fl_Widget *w, void *arg )
{
  printf( "selected: %s\n", ((Flu_Combo_List*)w)->value() );
}

static void tree_callback( Fl_Widget *w, void *arg )
{
  printf( "selected: %s\n", ((Flu_Combo_Tree*)w)->value() );
}

int main( int argc, char **argv )
{
  Fl_Double_Window *win = new Fl_Double_Window( 100, 100, 300, 100, "Flu_Combo_Box_Try" );

  Flu_Combo_List *c = new Flu_Combo_List( 100, 10, 160, 25, "Combo List" );
  c->list.add( "One" );
  c->list.add( "Two" );
  c->list.add( "Three" );
  c->list.add( "Four" );
  c->list.add( "Five" );
  c->list.add( "Six" );
  c->list.add( "Seven" );
  c->list.add( "Eight" );
  c->value( "One" );
  c->callback( list_callback );
  c->input_callback( list_input_callback );

  Flu_Combo_Tree *t = new Flu_Combo_Tree( 100, 60, 160, 25, "Combo Tree" );
  //t->box( FL_FLAT_BOX );
  t->editable( false );
  t->tree.all_branches_always_open( true );
  t->tree.show_root( false );
  t->tree.show_connectors( false );
  t->tree.horizontal_gap( -10 );
  t->tree.show_leaves( false );
  t->tree.add( "/data/" );
  t->tree.add( "/home/user/cache/" );
  t->tree.add( "/home/foo/" );
  t->tree.add( "/tmp/" );
  t->tree.add( "/tmp/bar/" );
  t->value( "/data/" );
  t->callback( tree_callback );

  win->end();
  win->resizable( win );

  win->show();

  return( Fl::run() );
}
