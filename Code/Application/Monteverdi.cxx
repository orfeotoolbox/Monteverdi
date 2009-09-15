
#include "ConfigureMonteverdi.h"
#include <FL/Fl.h>
#include <FL/Fl_Timer.h>
#include <FL/Fl_Shared_Image.h>
//#include <FL/Fl_Monitor.h>
#include <FL/Fl_Window.h>
//#include <FL/Fl_run.h>
#include <FL/Fl_PNG_Image.h>
#include <string>
#include <ctime>
#include <iostream>

Fl_Window* splash_screen() {

  unsigned int spl_w = 484;
  unsigned int spl_h = 599;
        Fl_Window* o = new Fl_Window(spl_w, spl_h, "splash screen");
        o->type(241);
        o->begin();
                Fl_Group* g = new Fl_Group(0, 0, spl_w, spl_h);
		std::string splashImage(Monteverdi_SOURCE_DIR);
		splashImage +="/Data/Artwork/Monteverdi.png";

		std::cout << splashImage << std::endl;

                g->image(new Fl_PNG_Image(splashImage.c_str()));
                g->resizable(o);
        o->end();
        o->set_non_modal();
        o->clear_border();
        o->resizable(o);

        o->border(false);
	int posx,posy;
	posx = (Fl::w() / 2) - spl_w/2;
	posy = (Fl::h() / 2) - spl_h/2; 

	o->position(posx,posy);
        
//o->Rectangle::set(Fl_Monitor::find(0,0),o->w(),o->h(),Fl_ALIGN_CENTER);
        o->show();
	//o->flush();

        do {
                 Fl::check();

        } while (!o->visible());

        return o;
}




int main(int argc, char** argv) {

        const bool show_splash = true;

        // open splash screen
        Fl_Window* splash_window = 0;
	std::clock_t start;

	start = std::clock();

                splash_window = splash_screen();

                splash_window->show();


                // hide splash screen after 3 seconds
                while(splash_window->visible() && ( std::clock() - start ) / (double)CLOCKS_PER_SEC < 3.0)
		  {
		Fl::check();
		  }

                splash_window->hide();
                delete splash_window;

		return Fl::run();      
}
