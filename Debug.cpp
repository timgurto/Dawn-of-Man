// (C) 2009-2010 Tim Gurto

#ifndef NDEBUG

#include <queue>
#include <cassert>

#include "SDL.h"
#include "SDL_ttf.h"

#include "misc.h"
#include "globals.h"
#include "Debug.h"

int Debug::debugCount = 0;

Debug::Debug(SDL_Color color, pixels_t x, pixels_t y,
             unsigned short count):
color_(color),
x_(x),
y_(y),
count_(count){
   ++debugCount;
}

Debug::~Debug(){
   --debugCount;
   if (font != 0)
      TTF_CloseFont(font);
   if (debugCount == 0)
      TTF_Quit();
}

void Debug::initScreen(SDL_Surface *screen){
   screen_ = screen;
}

void Debug::initFont(std::string name, int size){
   font = TTF_OpenFont(name.c_str(), size);
   assert (font != 0);
   height = TTF_FontHeight(font);
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
         SDL_Surface *blackSurface = TTF_RenderText_Solid(font,
                                                          message.c_str(),
                                                          BLACK);
         SDL_BlitSurface(blackSurface, 0, screen_, &makeRect(x_+1, y_+1 + lat));
         
         SDL_Surface *surface = TTF_RenderText_Solid(font,
                                                     message.c_str(),
                                                     color_);
         SDL_BlitSurface(surface, 0, screen_, &makeRect(x_, y_ + lat));
         
         SDL_FreeSurface(surface);
         SDL_FreeSurface(blackSurface);
         lat += height;
      }
   }
}

#endif