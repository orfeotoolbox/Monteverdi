#include <stdio.h>
#include <FL/Fl.H>
#include <FL/Fl_Single_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Round_Button.H>
#include <FL/Fl_Input.H>
#include <FLU/Flu_File_Chooser.h>

Fl_Input *path, *pattern;
Fl_Round_Button *files, *directories, *both, *single, *multi;

static void contextHandler( const char *filename, int type, void *arg )
{
  printf( "'%s' handled\n", filename );
}

static int length_sort( const char *f1, const char *f2 )
{
  int l1 = strlen( f1 );
  int l2 = strlen( f2 );
  return( l1 < l2 ? -1 : ( l1 > l2 ? 1 : 0 ) );
}

static void browse( Fl_Widget *, void* )
{
#if 0
  const char *filename = flu_file_chooser( "Flu_File_Chooser", pattern->value(), path->value() );
  //const char *filename = flu_dir_chooser( "Flu_File_Chooser", path->value() );
  printf( "%s\n", filename?filename:"Cancelled" );
#else
  int type = 0;
  if( directories->value() )
    type |= Flu_File_Chooser::DIRECTORY;
  if( both->value() )
    type |= Flu_File_Chooser::STDFILE;
  if( multi->value() )
    type |= Flu_File_Chooser::MULTI;
  Flu_File_Chooser *f = new Flu_File_Chooser( path->value(), pattern->value(), type, "Flu_File_Chooser" );
  //f->set_sort_function( length_sort );
  f->set_modal();
  f->show();
  f->add_type( "exe", "Executable Program File" /*, icon */ );
  f->add_type( "h", "C++ Header File" );
  f->add_type( "cpp cxx c++", "C++ Source File" );
  f->add_type( "txt", "Text File" );
  f->add_type( "png", "PNG Image" );
  f->add_type( "jpg jpe jpeg", "JPEG Image" );
  f->add_type( "gif", "GIF Image" );
  f->add_type( "tif tiff", "TIFF Image" );
  f->add_type( "gz zip z", "Compressed File" );
  f->add_type( "tar", "Unix Archive" );
  f->add_type( "pdf", "Acrobat Document" );
  f->add_context_handler( Flu_File_Chooser::ENTRY_FILE, "cpp", "Custom Handler", contextHandler, NULL );
  f->add_context_handler( Flu_File_Chooser::ENTRY_FILE | Flu_File_Chooser::ENTRY_DIR,
			  NULL, "General Handler", contextHandler, NULL );
  f->add_context_handler( Flu_File_Chooser::ENTRY_NONE, NULL, "Nothing Handler", contextHandler, NULL );

  while( f->shown() )
    Fl::wait();
  int count = f->count() ;
  if( count == 0 )
    printf( "cancelled\n" );
  else if( count == 1 )
    printf( "selected: %s\n", f->value() );
  else
    {
      printf( "selected:\n" );
      for( int i = 1; i <= count; i++ )
	printf( "  %s\n", f->value(i) );
    }
#endif
}

int main( int argc, char **argv )
{
  Fl_Single_Window *win = new Fl_Single_Window( 100, 100, 300, 200, "Flu_File_Chooser Demo" );
  path = new Fl_Input( 60, 10, win->w()-70, 30, "Path" );
  path->value( "~" );
  pattern = new Fl_Input( 60, 50, win->w()-70, 30, "Pattern" );
  pattern->value( "*" );
  Fl_Group *g = new Fl_Group( 60, 90, win->w()-60, 60 );
  {
    files = new Fl_Round_Button( 60, 90, 100, 20, "Files" );
    files->type( FL_RADIO_BUTTON );
    files->value(1);
    directories = new Fl_Round_Button( 60, 110, 100, 20, "Directories" );
    directories->type( FL_RADIO_BUTTON );
    both = new Fl_Round_Button( 60, 130, 160, 20, "Files and Directories" );
    both->type( FL_RADIO_BUTTON );
  }
  g->end();
  g = new Fl_Group( 230, 90, 100, 40 );
  {
    single = new Fl_Round_Button( 230, 90, 60, 20, "Single" );
    single->type( FL_RADIO_BUTTON );
    single->value(1);
    multi = new Fl_Round_Button( 230, 110, 60, 20, "Multi" );
    multi->type( FL_RADIO_BUTTON );
  }
  g->end();
  Fl_Button *b = new Fl_Button( 120, 160, 60, 25, "Browse!" );
  b->callback( browse );

  win->end();

  win->show();

  Fl::run();

  delete win;
}
