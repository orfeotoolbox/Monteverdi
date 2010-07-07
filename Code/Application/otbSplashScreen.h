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
  typedef SplashScreen                  Self;
  typedef itk::Object                   Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** Standards macros */
  itkNewMacro(Self);

  /** Runtime information */
  itkTypeMacro(SplashScreen, itk::Object);

  /** Build the splash screen */
  void Build() {

    unsigned int ui_im_w = 484;     //765;
    unsigned int ui_im_h = 599;     //653;
    unsigned int ui_nb_images = 1;     //11;
    unsigned int ui_pos_width = (Fl::w() / 2) - ui_im_w / 2;
    unsigned int ui_pos_height = (Fl::h() / 2) - ui_im_h / 2;
    unsigned int ui_i;

    m_SplashWindow = new Fl_Window(ui_im_w, ui_im_h, "splash screen");
    Fl_Group* group;

    m_SplashWindow->type(241);
    m_SplashWindow->begin();

    for (ui_i = 1; ui_i <= ui_nb_images; ui_i++)
      {

      std::ostringstream out;
      out << ui_i;

      std::string splashImage(Monteverdi_SOURCE_DIR);
      splashImage += "/Data/Artwork/Monteverdi";
      splashImage += ".png";
      Fl_PNG_Image* image_PNG;
      image_PNG = new Fl_PNG_Image(splashImage.c_str());
      m_ImageVector.push_back(image_PNG);

      group = new Fl_Group(0, 0, ui_im_w, ui_im_h);
      group->image(m_ImageVector[ui_i - 1]);
      group->align(FL_ALIGN_CENTER);
      group->resizable(m_SplashWindow);
      m_GroupVector.push_back(group);
      }

    m_SplashWindow->end();
    m_SplashWindow->set_non_modal();
    m_SplashWindow->clear_border();
    m_SplashWindow->resizable(m_SplashWindow);
    m_SplashWindow->border(false);

    m_SplashWindow->position(ui_pos_width, ui_pos_height);

  }

  /** Show and shut down the splash screen */
  void Show() {

    unsigned int ui_pos_width = (Fl::w() / 2) - 275;
    unsigned int ui_pos_height = (Fl::h() / 2) - 200;
    unsigned int ui_cpt = 0;

    Fl_Timer* timer = new Fl_Timer(FL_HIDDEN_TIMER, ui_pos_width, ui_pos_height, 30, 30, "timer");

    m_SplashWindow->redraw();
    m_SplashWindow->show();

    // simple case
    timer->value(m_DisplayTime);
    m_GroupVector[ui_cpt]->show();
    while (timer->value() && m_SplashWindow->visible())
      {
      Fl::check();
      }

    // shut down the splash
    m_SplashWindow->hide();
    delete m_SplashWindow;
  }

  itkSetMacro(DisplayTime, double);
  itkGetMacro(DisplayTime, double);

protected:
  /** Constructor */
  SplashScreen(){m_DisplayTime = 5.; }
  /** Destructor */
  virtual ~SplashScreen(){}

private:
  SplashScreen(const Self&); //purposely not implemented
  void operator =(const Self&); //purposely not implemented

  Fl_Window*                 m_SplashWindow;
  Fl_Group*                  m_Group;
  std::vector<Fl_Group*>     m_GroupVector;
  std::vector<Fl_PNG_Image*> m_ImageVector;
  double                     m_DisplayTime;

}; // end class
} // end namespace otb
