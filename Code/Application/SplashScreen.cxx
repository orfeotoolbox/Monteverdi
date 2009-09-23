/*=========================================================================

Program:   ORFEO Toolbox
Language:  C++
Date:      $Date$
Version:   $Revision$


Copyright (c) Centre National d'Etudes Spatiales. All rights reserved.
See OTBCopyright.txt for details.


This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "itkExceptionObject.h"

#include "ConfigureMonteverdi.h"
#include "otbCommandLineArgumentParser.h"
#include <FL/Fl.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_JPEG_Image.H>
#include <FL/Fl_PNG_Image.H>
#include <FL/Fl_Timer.H>
#include <FLU/Flu_Tree_Browser.h>
#include <ctime>
#include <sstream>
#include "itkObject.h"

#include "otbI18n.h"


namespace otb
{
/** \class SplashScreen
 *
 */
class SplashScreen :
  public itk::Object
{
public:
  /** Standard class typedefs */
  typedef SplashScreen                     Self;
  typedef itk::Object                      Superclass;
  typedef itk::SmartPointer<Self>          Pointer;
  typedef itk::SmartPointer<const Self>    ConstPointer;

  /** Standards macros */
  itkNewMacro(Self);

  /** Runtime information */
  itkTypeMacro(SplashScreen,itk::Object);

  /** Build the splash screen */
  void Build() {

        unsigned int ui_im_w = 765;
        unsigned int ui_im_h = 653;
        unsigned int ui_nb_images = 11;
        unsigned int ui_pos_width = (Fl::w()/2) - ui_im_w/2;
        unsigned int ui_pos_height = (Fl::h()/2) - ui_im_h/2;
        unsigned int ui_i;

        m_SplashWindow = new Fl_Window(ui_im_w, ui_im_h, "splash screen");
        Fl_Group* group;

        m_SplashWindow->type(241);
        m_SplashWindow->begin();

        for(ui_i=1;ui_i<=ui_nb_images;ui_i++){

            std::ostringstream out;
            out << ui_i;

            std::string splashImage(Monteverdi_SOURCE_DIR);
            splashImage +="/Data/Artwork/monteverdi";
            splashImage += out.str();
            splashImage +=".png";
            Fl_PNG_Image*   image_PNG;
            image_PNG = new Fl_PNG_Image(splashImage.c_str());
            m_ImageVector.push_back(image_PNG);

            group = new Fl_Group(0, 0, ui_im_w, ui_im_h);
            group->image(m_ImageVector[ui_i-1]);
            group->align(FL_ALIGN_CENTER);;
            group->resizable(m_SplashWindow);
            m_GroupVector.push_back(group);
        }

        m_SplashWindow->end();
        m_SplashWindow->set_non_modal();
        m_SplashWindow->clear_border();
        m_SplashWindow->resizable(m_SplashWindow);
        m_SplashWindow->border(false);

        m_SplashWindow->position(ui_pos_width,ui_pos_height);

  }

  /** Show and shut down the splash screen */
  void Show() {

// Commented out because causing compilation warning
//        unsigned int ui_pos_width = (Fl::w()/2) -275;
//         unsigned int ui_pos_height = (Fl::h()/2) - 200;
        unsigned int ui_cpt =0;

//         Fl_Timer* timer = new Fl_Timer(FL_HIDDEN_TIMER,ui_pos_width,ui_pos_height,30,30,"timer");

        m_SplashWindow->redraw();
        m_SplashWindow->show();

// simple case 
//         timer->value(2.);
//         while( timer->value() && m_SplashWindow->visible() )
//             Fl::check();


// several images case
        for(ui_cpt=0;ui_cpt<m_GroupVector.size();ui_cpt++){

            // wait 0.1 second
//             while( timer->value()>0 && m_SplashWindow->visible() ){
//               Fl::check();
// std::cout<< timer->value()<<std::endl;
//              }
Fl::wait(0.01);
Fl::check();
//             timer->value(0.5);
             m_GroupVector[ui_cpt]->show();

          }
          // shut down the splash
          m_SplashWindow->hide();
          delete m_SplashWindow;

  }

 protected:
  /** Constructor */
  SplashScreen(){};
  /** Destructor */
  ~SplashScreen(){};

 private:
  SplashScreen(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  Fl_Window*      m_SplashWindow;
  Fl_Group*       m_Group;
  std::vector<Fl_Group*>     m_GroupVector;
  std::vector<Fl_PNG_Image*> m_ImageVector;



}; // end class
} // end namespace otb













using namespace otb;

 Fl_Double_Window *win = new Fl_Double_Window( 500, 460, "Flu_Tree_Browser_Try2" );
 Flu_Tree_Browser *tree;
 Fl_Input *newNode;
 Fl_Button *addChild, *removeBtn;
 int next = 1;

static void removeCB( Fl_Widget*, void* )
{
  Flu_Tree_Browser::Node* n = tree->get_selected( 1 );
  if( !n )
    return;
  tree->remove( n );
  const char *path = n->find_path();
  tree->remove( path );
  tree->redraw();
}

static void addChildCB( Fl_Widget*, void* )
{
  Flu_Tree_Browser::Node* n = tree->get_selected( 1 );
  if( !n )
    n = tree->last();
  n->add( newNode->value() );
  tree->redraw();

  char buf[32];
  sprintf( buf, "Node%d", next++ );
  newNode->value( buf );
}


int main(int argc,char* argv[])
{
  otbI18nMacro();

  // Parse command line parameters
  typedef otb::CommandLineArgumentParser ParserType;
  ParserType::Pointer parser = ParserType::New();

  parser->AddInputImage(false); //Optionnal parameter

  typedef otb::CommandLineArgumentParseResult ParserResultType;
  ParserResultType::Pointer  parseResult = ParserResultType::New();

  try
  {
    parser->ParseCommandLine(argc,argv,parseResult);
  }
  catch ( itk::ExceptionObject & err )
  {
    std::string descriptionException = err.GetDescription();
    if (descriptionException.find("ParseCommandLine(): Help Parser") != std::string::npos)
    {
      std::cout << "WARNING : output file pixels are converted in 'unsigned char'" << std::endl;
      return EXIT_SUCCESS;
    }
    if (descriptionException.find("ParseCommandLine(): Version Parser") != std::string::npos)
    {
      return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
  }

  typedef SplashScreen::Pointer SplashScreenPointerType;

  SplashScreenPointerType splash = SplashScreen::New();
  splash->Build();
  splash->Show();



  win->begin();

  tree = new Flu_Tree_Browser( 200, 0, 300, 460, "Tree test" );
  tree->box( FL_DOWN_BOX );
  tree->auto_branches( true );
  tree->label( "Tree Browser Demo" );

  Fl_Group *g = new Fl_Group( 0, 0, 200, 460 );
  g->resizable( NULL );

  newNode = new Fl_Input( 50, 370, 100, 20, "Name" );
  newNode->value( "Node" );

  addChild = new Fl_Button( 10, 400, 100, 20, "Add Child" );
  addChild->callback( addChildCB, NULL );

  removeBtn = new Fl_Button( 10, 430, 100, 20, "Remove" );
  removeBtn->callback( removeCB, NULL );

  win->resizable( tree );
  win->show( argc, argv );

  g->end();
  win->end();




/*
  const bool show_splash = true;
  const char * cfname = fl_file_chooser("Pick an image file", "*.*","");//tooto.c_str()
  Fl::check();*/

    Fl::run();

  return EXIT_SUCCESS;
}

