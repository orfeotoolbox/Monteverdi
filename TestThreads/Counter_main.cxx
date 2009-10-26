#include <FL/Fl.H>

#include "Counter.H"

#include <stdlib.h>
#include <math.h>

#include <iostream>
#define DEBUG

#ifdef DEBUG
       #include <iostream>
       using namespace std;
#endif

bool stop;
Trigger running;

void Counter::command_cb()
{
       if(!running.up()) {
              command->label("STOP");
              command->labelcolor(FL_RED);
              command->redraw();
              stop=false;
              create();
       } else {       
              // Please try eliminate usage of Thread::kill, it's dangerous!
              command->label("START");
              command->labelcolor(FL_BLUE);
              command->redraw();
              stop=true;
       }
}

void Counter::quit_cb()
{
       stop=true;
       main_w->hide();
       Fl::unlock(); // IMPORTANT!!!!!, child has called Fl::lock(), 
       running.wait(); // so let him have it, then it'll check "stop" and quit.

       Fl::lock(); // return things to their place .....
}

void Counter::thread()
{
       running.toggle();

#ifdef DEBUG
       cout << "running.up(): " << (running.up()?"true":"false") << endl;
#endif

       for(int i=0, delta=5; ; ) {
              i+=delta;
              if(i >= 1000 || i < 1) {
                     delta=-delta;
              }
              Fl::lock();
                     if(stop) {
                            Fl::unlock();
                            break;
                     }
                     progress->value(i/10.0);
              Fl::unlock();
              Fl::awake(); // this will make fltk to do updates, if you just
              // want READ variables Fl::awake() is not needed.

              for(int j=0; j<10000000; sin(j++))
                  {
                  std::cout<<"."; /* CPU-delay */
                  }
              std::cout  << std::endl;
              std::cout  << "--------------------------" << std::endl;
              std::cout  << "--------------------------" << std::endl;
              std::cout  << "--------------------------" << std::endl;
              std::cout  << "--------------------------" << std::endl;
              std::cout  << "--------------------------" << std::endl;
              relax(0.005*rand()/RAND_MAX);
              
#ifdef DEBUG
              cout << "stop: " << (stop?"true":"false") << endl;
#endif
       }

       Fl::lock();
              command->label("START");
              command->labelcolor(FL_BLUE);
              command->redraw();
       Fl::unlock();
       Fl::awake();

#ifdef DEBUG
       cout << "running.up(): " << (!running.up()?"true":"false") << endl;
#endif //                      ^^^^^ because we print BEFORE toggle() call.
       running.toggle();
}

int main(int argc, char* argv[])
{
       Fl::lock(); // initialization!!!!!
       Fl::args(argc, argv);
       
       running.toggle();

       Counter counter;
       counter.main_w->show(argc, argv);

       counter.command->do_callback();
       Fl::run();

       return 0;
}
