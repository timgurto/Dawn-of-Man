// (C) 2009 Tim Gurto

#ifndef DEBUG_H
#define DEBUG_H

#include <queue>

#include "SDL.h"
#include "SDL_ttf.h"

#include "misc.h"
#include "globals.h"

//TODO: make singleton
class Debug{
   SDL_Color color_;
   std::string name_;
   TTF_Font *font;
   pixels_t x_, y_;
   unsigned short count_;
   std::queue<std::string> messages;
   SDL_Surface *screen_;
   int height;

public:
   Debug(SDL_Color color, pixels_t x, pixels_t y,
         unsigned short count);
   ~Debug();
   void setScreen(SDL_Surface *screen);
   void setFont(std::string name, int size);
   void add(std::string message);
   void display();

   //A, B, C must have << defined
   template <typename A>
   void operator()(A a){
      std::ostringstream ss;
      ss << a;
      add(ss.str());
   }
   template <typename A, typename B>
   void operator()(A a, B b){
      std::ostringstream ss;
      ss << a << b;
      add(ss.str());
   }
   template <typename A, typename B, typename C>
   void operator()(A a, B b, C c){
      std::ostringstream ss;
      ss << a << b << c;
      add(ss.str());
   }
};

#endif






