#ifndef DEBUG_H
#define DEBUG_H

#include <queue>

#include "SDL.h"
#include "SDL_ttf.h"

#include "misc.h"
#include "globals.h"

class Debug{
public:
   Debug(SDL_Color color, screenSize_t x, screenSize_t y,
         unsigned short count);
   ~Debug();
   void setScreen(SDL_Surface *screen);
   void setFont(std::string name, int size);
   void add(std::string message);
   void operator()(std::string message);
   void display();

private:
   SDL_Color color_;
   std::string name_;
   TTF_Font *font;
   screenSize_t x_, y_;
   unsigned short count_;
   std::queue<std::string> messages;
   SDL_Surface *screen_;
   int height;
};

#endif