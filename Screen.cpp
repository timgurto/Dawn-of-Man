// (C) 2010 Tim Gurto

#include <cassert>
#include <sstream>
#include <ctime>
#include "Screen.h"
#include "Surface.h"
#include "Debug.h"
#include "Point.h"
#include "globals.h"

//TODO comments
extern Debug debug;

//Default screen resolutions - 16:10 16:9 and 4:3
const Point Screen::defaultRes[] = {
   Point(1280, 800),
   Point(1280, 720),
   Point(1024, 768),
   Point() //sentinel
};

Surface *Screen::background_ = 0;
Surface *Screen::cursor_ = 0;
Point Screen::screenRes(defaultRes[0]);
Point Screen::mousePos = screenRes / 2;
std::vector<Point> Screen::preferredResolutions;
bool Screen::windowedMode = DEBUG;


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
   //TODO don't deref FPs
   return (*go_)(*this, data);
}

void Screen::setScreenResolution(int argc, char **argv){

   const SDL_VideoInfo *current = SDL_GetVideoInfo();
   debug("Current resolution: ", current->current_w, "x",
         current->current_h);
   SDL_Rect **resolutions = SDL_ListModes(0, SDL_FULLSCREEN |
                                             SDL_HWSURFACE |
                                             SDL_DOUBLEBUF);
   assert (resolutions);
   debug("Available resolutions:");
   //whether the default screen size is available
   bool defaultResFound = false;
   const Point *defaultResCheck;
   for (defaultResCheck = defaultRes; *defaultResCheck != Point();
        ++defaultResCheck){
      for (SDL_Rect **currentResCheck = resolutions; *currentResCheck;
           ++currentResCheck){
         if (**currentResCheck == *defaultResCheck){
            defaultResFound = true;
            break;
         }
      }
      if (defaultResFound)
         break;    
   }

   //Windowed
   windowedMode = DEBUG || isArg("-win", argc, argv);
   if (isArg("-width", argc, argv) &&
       isArg("-height", argc, argv)){
      screenRes.x = whatIsArg("-width", argc, argv);
      screenRes.y = whatIsArg("-height", argc, argv);
   //Current resolution
   }else if (isArg("-retain", argc, argv) || !defaultResFound){
      screenRes.x = current->current_w;
      screenRes.y = current->current_h;
   //Default
   }else{
      if (defaultResFound)
         screenRes = *defaultResCheck;
      else
         //default: retain current resolution
         screenRes.x = current->current_w;
         screenRes.y = current->current_h;
   }
}