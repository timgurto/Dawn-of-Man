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
const Point Screen::defaultRes_[] = {
   Point(1280, 800),
   Point(1280, 720),
   Point(1024, 768),

   Point() //sentinel
};

Surface *Screen::background_ = 0;
Surface *Screen::cursor_ = 0;
Point Screen::screenRes_(defaultRes_[0]);
Point Screen::mousePos = screenRes_ / 2;
bool Screen::windowedMode_ = DEBUG;
TTF_Font *Screen::titleFont_ = 0;
SDL_Color Screen::defaultTextColor_ = BLACK;


unsigned Screen::goDefault_(Screen &thisScreen, const void *data){
   assert(!data);

   while(thisScreen.loop_){
      thisScreen.handleEventsDefault_();
      thisScreen.drawDefault_();
   }
   return 0;
}

void Screen::handleEventsDefault_(){
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

   //title
   Surface title(titleFont_, title_, defaultTextColor_);
   pixels_t xCoord = (screenRes_.x - title->w) / 2;
   //TODO init this y co-ord
   title.draw(screenBuf, &makeRect(xCoord, 20));

   //flip buffer
   screenBuf.flip();
}

void Screen::init(Surface *background,
                  Surface *cursor,
                  int titleSize,
                  const std::string &titleFontName,
                  SDL_Color textColor){
   background_ = background;
   cursor_ = cursor;
   defaultTextColor_ = textColor;
   titleFont_ = TTF_OpenFont(titleFontName.c_str(), titleSize);
   assert (titleFont_);
}

Screen::Screen(std::string title, GoFun go):
title_(title),
go_(go),
loop_(true){}

int Screen::operator()(const void *data){
   //make sure there are no events on the queue
   //SDL_Event event;
   //TODO assert(!SDL_PollEvent(&event));

   //empty queue?
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