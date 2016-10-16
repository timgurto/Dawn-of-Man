// (C) 2010 Tim Gurto

#include <cassert>
#include <sstream>
#include <ctime>
#include "Screen.h"
#include "Surface.h"
#include "Debug.h"
#include "Point.h"

//TODO comments
extern Debug debug;

//Default screen resolutions - 16:10 16:9 and 4:3
//TODO array
const Point Screen::DEFAULT_SCREEN_3(1280, 800);
const Point Screen::DEFAULT_SCREEN_2(1280, 720);
const Point Screen::DEFAULT_SCREEN_1(1024, 768);

Surface *Screen::background_ = 0;
Surface *Screen::cursor_ = 0;
Point Screen::screenRes(DEFAULT_SCREEN_1);
Point Screen::mousePos = screenRes / 2;
std::vector<Point> Screen::preferredResolutions;


unsigned Screen::goDefault(Screen &thisScreen, const void *data){

   while(thisScreen.loop_){
      thisScreen.handleEventsDefault();
      thisScreen.drawDefault();
   }
   return 0;
}

void Screen::handleEventsDefault(){
   SDL_Event event;
   while (SDL_PollEvent(&event)){
      switch (event.type){

      //Window is exited
      case SDL_QUIT:
         loop_ = false;
         break;

      //Mouse is moved
      case SDL_MOUSEMOTION:
         mousePos.x = event.motion.x;
         mousePos.y = event.motion.y;

         //TODO context help

         break;

      //A key is pressed
      case SDL_KEYDOWN:
         {//new scope for key
            SDLKey key = event.key.keysym.sym;
            switch (key){
            case SDLK_PRINT:
               { //new scope for os
                  std::ostringstream os;
                  os << SCREENSHOTS_PATH << "shot" << time(0) << ".bmp";
                  screenBuf.saveToBitmap(os.str());
               }
               break;

            case SDLK_ESCAPE:
               break;

            case SDLK_RETURN:
               break;

            }
         }
         break;



      //A mouse button is pressed
      case SDL_MOUSEBUTTONDOWN:
         debug("Mouse down: ", int(event.button.button));
         switch (event.button.button){
         case MOUSE_BUTTON_LEFT:
            break;
         }
         break;

      //A mouse button is released
      case SDL_MOUSEBUTTONUP:
         switch (event.button.button){
         case MOUSE_BUTTON_LEFT:
            //check buttons/other elements
            break;
         }
         break;

      } //event switch
   } //event while
}

void Screen::drawDefault() const{
   assert(screenBuf);
   screenBuf.fill();
   screenBuf << *background_;
   screenBuf.flip();
}

void Screen::init(Surface *background,
                  Surface *cursor){
   background_ = background;
   cursor_ = cursor;
}

Screen::Screen(GoFun go):
go_(go),
loop_(true){}

int Screen::operator()(const void *data){
   //make sure there are no events on the queue
   //SDL_Event event;
   //TODO assert(!SDL_PollEvent(&event));
   return (*go_)(*this, data);
}

static void setScreenResolution(int argc, char **argv){

}