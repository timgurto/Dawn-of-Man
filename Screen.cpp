// (C) 2010 Tim Gurto

#include <cassert>
#include <sstream>
#include <ctime>
#include <vector>
#include "Screen.h"
#include "Surface.h"
#include "Debug.h"
#include "Point.h"
#include "ScreenElement.h"
#include "globals.h"
#include "util.h"

//TODO comments
extern Debug debug;

//Default screen resolutions - 16:10 16:9 and 4:3
const Point Screen::defaultRes_[] = {
   Point(1280, 800),
   Point(1280, 720),
   Point(1024, 768),

   Point() //sentinel
};

Surface *Screen::background_ = 0;
Surface *Screen::cursor_ = 0;
Point Screen::screenRes_ = defaultRes_[0];
Point Screen::mousePos = screenRes_ / 2;
bool Screen::windowedMode_ = DEBUG;


unsigned Screen::goDefault_(Screen &thisScreen, const void *data){
   assert(!data);

   //clear all rollover
   for (elements_t::iterator it = thisScreen.elements_.begin();
        it != thisScreen.elements_.end(); ++it)
      it->rollover_ = false;

   while(thisScreen.loop_){
      int result = thisScreen.handleEventsDefault_();
      //if a button was clicked
      if (result != ScreenElement::NO_ID)
         return result;
      thisScreen.drawDefault_();
   }
   return 0;
}

int Screen::handleEventsDefault_(){
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

         //check whether over any buttons, for rollover color
         { //new scope for found
            bool found = false;
            for (elements_t::iterator it = elements_.begin();
                 it != elements_.end(); ++it){
               it->rollover_ = false;
               if (!found) //only consider one collision
                  if (collision(it->image_->clip_rect + it->loc_,
                                mousePos)){
                     it->rollover_ = true;
                     //debug("rollover: ", it->text_);
                     found = true;
                  }
            }
         }

         //TODO context help
         //TODO rollover color for buttons

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

            //TODO defaults for Esc and Ret
            case SDLK_ESCAPE:
               break;

            case SDLK_RETURN:
               break;

            }
         }
         break;

      //A mouse button is pressed
      case SDL_MOUSEBUTTONDOWN:
         switch (event.button.button){
         case MOUSE_BUTTON_LEFT:
            //check whether a button was clicked
            for (std::vector<ScreenElement>::const_iterator it = elements_.begin();
                 it != elements_.end(); ++it)
               if (it->type_ == ELEM_BUTTON &&
                  collision(it->image_->clip_rect + it->loc_,
                            mousePos))
                     return it->id_;
            break;
         }
         break;

      default:
         debug("Uncaught event: ", int(event.type));

      } //event switch

   } //event while

   //nothing to report
   return ScreenElement::NO_ID;
}

void Screen::drawDefault_() const{
   assert(screenBuf);
   screenBuf.fill();

   //background
   int
      xTiles = screenRes_.x / MAP_TILE_SIZE + 1,
      yTiles = screenRes_.y / MAP_TILE_SIZE + 1;
   for (int x = 0; x != xTiles; ++x)
      for (int y = 0; y != yTiles; ++y)
         background_->draw(screenBuf, &makeRect(x * MAP_TILE_SIZE,
                                                y * MAP_TILE_SIZE));

   //elements
   for (std::vector<ScreenElement>::const_iterator it = elements_.begin();
        it != elements_.end(); ++it)
      it->draw();

   //cursor
   cursor_->draw(screenBuf, &makeRect(mousePos + CURSOR_OFFSET));

   //debug log
   if (DEBUG)
      debug.display();

   //flip buffer
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
   return (*go_)(*this, data);
}

void Screen::addElement(const ScreenElement &element){
   elements_.push_back(element);
}

void Screen::setScreenResolution(int argc, char **argv){

   const SDL_VideoInfo *current = SDL_GetVideoInfo();
   debug("Current resolution: ", current->current_w, "x",
         current->current_h);
   SDL_Rect **resolutions = SDL_ListModes(0, SDL_FULLSCREEN |
                                             SDL_SWSURFACE |
                                             SDL_DOUBLEBUF);
   assert (resolutions);
   debug("Available resolutions:");
   //whether the default screen size is available
   bool defaultResFound = false;
   const Point *defaultResCheck;
   for (defaultResCheck = defaultRes_; *defaultResCheck != Point();
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
   windowedMode_ = DEBUG || isArg("-win", argc, argv);
   if (isArg("-width", argc, argv) &&
       isArg("-height", argc, argv)){
      screenRes_.x = whatIsArg("-width", argc, argv);
      screenRes_.y = whatIsArg("-height", argc, argv);
   //Current resolution
   }else if (isArg("-retain", argc, argv) || !defaultResFound){
      screenRes_.x = current->current_w;
      screenRes_.y = current->current_h;
   //Default
   }else{
      if (windowedMode_ || defaultResFound)
         screenRes_ = *defaultResCheck;
      else
         //default: retain current resolution
         screenRes_.x = current->current_w;
         screenRes_.y = current->current_h;
   }
}

const Point &Screen::getScreenRes(){
   return screenRes_;
}

bool Screen::getWindowedMode(){
   return windowedMode_;
}