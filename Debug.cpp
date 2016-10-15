// (C) 2009-2010 Tim Gurto

#ifndef NDEBUG

#include <queue>
#include <cassert>

#include "SDL.h"
#include "SDL_ttf.h"

#include "misc.h"
#include "types.h"
#include "Debug.h"

int Debug::debugCount_ = 0;

Debug::Debug(SDL_Color color, pixels_t x, pixels_t y,
             unsigned short count):
color_(color),
x_(x),
y_(y),
count_(count){
   ++debugCount_;
}

Debug::~Debug(){
   --debugCount_;
   if (font_ != 0)
      TTF_CloseFont(font_);
   if (debugCount_ == 0)
      TTF_Quit();
}

void Debug::initScreen(SDL_Surface *screen){
   screen_ = screen;
}

void Debug::initFont(std::string name, int size){
   font_ = TTF_OpenFont(name.c_str(), size);
   assert (font_ != 0);
   height_ = TTF_FontHeight(font_);
}

void Debug::add(std::string message){
   messages.push(message);
   if (messages.size() > count_)
      messages.pop();
}

void Debug::display(){
   if (screen_ != 0){
      std::queue<std::string> copy(messages);
      int lat = 0;
      while (copy.size() != 0){
         std::string message = copy.front();
         copy.pop();
         SDL_Surface *blackSurface = TTF_RenderText_Solid(font_,
                                                          message.c_str(),
                                                          BLACK);
         SDL_BlitSurface(blackSurface, 0, screen_, &makeRect(x_+1, y_+1 + lat));
         
         SDL_Surface *surface = TTF_RenderText_Solid(font_,
                                                     message.c_str(),
                                                     color_);
         SDL_BlitSurface(surface, 0, screen_, &makeRect(x_, y_ + lat));
         
         SDL_FreeSurface(surface);
         SDL_FreeSurface(blackSurface);
         lat += height_;
      }
   }
}

#endif