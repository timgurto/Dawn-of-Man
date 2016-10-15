#include <queue>
#include <cassert>

#include "SDL.h"
#include "SDL_ttf.h"

#include "misc.h"
#include "globals.h"
#include "Debug.h"

Debug::Debug(SDL_Color color, screenSize_t x, screenSize_t y,
             unsigned short count):
color_(color),
x_(x),
y_(y),
count_(count){}

Debug::~Debug(){
   if (font != 0)
      TTF_CloseFont(font);
   TTF_Quit();
}

void Debug::setScreen(SDL_Surface *screen){
if (DEBUG){
   screen_ = screen;
}}

void Debug::setFont(std::string name, int size){
if (DEBUG){
   font = TTF_OpenFont(name.c_str(), size);
   assert (font != 0);
   height = TTF_FontHeight(font);
}}

void Debug::operator()(std::string message){
if (DEBUG){
   add(message);
}}

void Debug::add(std::string message){
if (DEBUG){
   messages.push(message);
   if (messages.size() > count_)
      messages.pop();
}}

void Debug::display(){
if (DEBUG){
   if (screen_ != 0){
      std::queue<std::string> copy(messages);
      int lat = 0;
      while (copy.size() != 0){
         std::string message = copy.front();
         copy.pop();
         SDL_Surface *surface = TTF_RenderText_Solid(font,
                                                     message.c_str(),
                                                     color_);
         SDL_BlitSurface(surface, 0, screen_, &makeRect(x_, y_ + lat));
         SDL_FreeSurface(surface);
         lat += height;
      }
   }
}}