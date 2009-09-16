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

#include "ConfigureMonteverdi.h"

#include "otbMonteverdiModel.h"

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_PNG_Image.H>

#include <string>
#include <ctime>
#include <iostream>


Fl_Window* splash_screen() {

  unsigned int spl_w = 484;
  unsigned int spl_h = 599;

  int posx = (Fl::w() / 2) - spl_w/2;
  int posy = (Fl::h() / 2) - spl_h/2;
  Fl_Window* o = new Fl_Window(spl_w, spl_h, "splash screen");
  o->type(241);
  o->begin();
  {
  Fl_Group* g = new Fl_Group(0, 0, spl_w, spl_h);
  std::string splashImage(Monteverdi_SOURCE_DIR);
  splashImage +="/Data/Artwork/Monteverdi.png";

  std::cout << splashImage << std::endl;

  Fl_PNG_Image* im = new Fl_PNG_Image(splashImage.c_str());
  g->image(im);
  g->resizable(o);
  g->align(FL_ALIGN_CENTER);
  }
  {
  Fl_Box* o = new Fl_Box(posx+10, posy+450, 45, 15, "Monteverdi");
  o->labeltype(FL_ENGRAVED_LABEL);
  o->labelsize(10);
  o->labelcolor((Fl_Color)1);
  }
  {
  Fl_Box* o = new Fl_Box(posx+10, posy+460, 45, 15, "version x.x");
  o->labeltype(FL_ENGRAVED_LABEL);
  o->labelsize(10);
  o->labelcolor((Fl_Color)1);
  }
  o->end();

  o->set_non_modal();
  o->clear_border();
  o->resizable(o);

  o->border(false);
  o->position(posx,posy);
  o->show();

  Fl::flush();

  do {
  Fl::check();

  } while (!o->shown());

  return o;
}

// There are function prototype conflits under cygwin between standard w32 API
// and standard C ones
#ifndef CALLBACK
#if defined(_WINDOWS) || defined(__CYGWIN__)
#define CALLBACK __stdcall
#else
#define CALLBACK
#endif
#endif

#include "otbReaderModule.h"
#include "otbSpeckleFilteringModule.h"

int main(int argc, char* argv[]) 
{
  // Create the model
  otb::MonteverdiModel::Pointer model = otb::MonteverdiModel::New();

  // Register modules
  model->RegisterModule<otb::ReaderModule>("Reader");
  model->RegisterModule<otb::SpeckleFilteringModule>("Speckle");

  // Create an instance of reader
  otb::Module::Pointer reader = model->CreateModuleByName("Reader");
  std::cout<<"Instance of reader module created: "<<reader<<std::endl;

  otb::Module::Pointer speckle = model->CreateModuleByName("Speckle");
  std::cout<<"Instance of speckle filtering module created: "<<speckle<<std::endl;

  // open splash screen
  Fl_Window* splash_window = 0;
  std::clock_t start;
  start = std::clock();
  splash_window = splash_screen();
  splash_window->show();

  // hide splash screen after 3 seconds
  while(splash_window->visible() && ( std::clock() - start ) / (double)CLOCKS_PER_SEC < 3.0)
    {
    Fl::wait(0);
    }

  splash_window->hide();
  delete splash_window;

  return Fl::run();
}
