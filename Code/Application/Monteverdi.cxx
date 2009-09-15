
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

        Fl_Window* o = new Fl_Window(484, 599, "splash screen");
        o->type(241);
        o->begin();
                Fl_Group* g = new Fl_Group(0, 0, 550, 400);
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

		Fl::check();
/*                // hide splash screen after 3 seconds
                while(splash_window->visible() && ( std::clock() - start ) / (double)CLOCKS_PER_SEC < 300.0)
*/
  return Fl::run();
                /*splash_window->hide();*/
                delete splash_window;

      
}
