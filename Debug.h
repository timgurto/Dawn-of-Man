// (C) 2009-2010 Tim Gurto

#ifndef DEBUG_H
#define DEBUG_H

#include <queue>
#include <sstream>

#include "SDL.h"
#include "SDL_ttf.h"

#include "misc.h"
#include "types.h"

//In Release, these objects have zero functionality.
//The methods are macro'd to be empty, and calls are
//hopefully optimized away by the compiler.
#ifndef NDEBUG

//A box of scrolling messages sent from various parts
//of the program
class Debug{
   SDL_Color color_; //font color
   std::string name_; //font name
   TTF_Font *font_; //SDL font object
   pixels_t x_, y_; //co-ordinates of top-left corner
   
   //maximum number of messages to show at once
   unsigned short count_;
   std::queue<std::string> messages; //the messages
   SDL_Surface *screen_; //The surface used to display
   int height_; //the height of each line of text

   //debug objects that exist
   static int debugCount_;

public:
   Debug(SDL_Color color, pixels_t x, pixels_t y,
         unsigned short count);
   ~Debug();

   //Initializes static pointers
   void initScreen(SDL_Surface *screen);

   //Initializes font info
   void initFont(std::string name, int size);

   //Adds message
   void add(std::string message);

   //Draws messages to the screen
   void display() const;

   //Generic wrappers for add(string)
   //A, B, C, D must have << defined
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
   template <typename A, typename B, typename C, typename D>
   void operator()(A a, B b, C c, D d){
      std::ostringstream ss;
      ss << a << b << c << d;
      add(ss.str());
   }
   template<>
   void operator()<std::string>(std::string s){
      add(s);
   }
   template<>
   void operator()<char *>(char *s){
      add(s);
   }
};

//Empty class for Release mode, that keeps method calls
//valid
#else

//empty functions
class Debug{
public:
   Debug(SDL_Color color, pixels_t x, pixels_t y,
      unsigned short count){}
   void initScreen(SDL_Surface *screen){}
   void initFont(std::string name, int size){}
   void display(){}

   template <typename A>
   void operator()(A a){}
   template <typename A, typename B>
   void operator()(A a, B b){}
   template <typename A, typename B, typename C>
   void operator()(A a, B b, C c){}
   template <typename A, typename B, typename C, typename D>
   void operator()(A a, B b, C c, D d){}
};

#endif //class definition

#endif //header guard