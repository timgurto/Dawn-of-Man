// (C) 2010 Tim Gurto

#ifndef MESSAGE_BOX_H
#define MESSAGE_BOX_H

#include <vector>
#include <string>
#include <sstream>
#include "SDL_ttf.h"
#include "types.h"

//Text that appears on the screen
class MessageBox{
   SDL_Color color_; //font color
   std::string name_; //font name
   TTF_Font *font_; //SDL font object
   pixels_t x_, y_; //co-ordinates of top-left corner
   pixels_t width_; //horizontal width of the box
   pixels_t height_; //height of the box
   pixels_t margin_; //padding around the text
   std::string message_; //the message
   SDL_Surface *background_; //the background image

public:
   MessageBox(SDL_Color color, pixels_t x, pixels_t y,
              pixels_t width, pixels_t margin,
              SDL_Surface *background,
              const std::string &fontName, int fontSize,
              bool accountForHeight = false);

   //Draws messages to the screen
   void display(SDL_Surface *screen) const;

   void clear();

   //Generic wrappers for changeMessage(string)
   //A, B, C, D must have << defined
   //TODO specialists for strings and char*
   template <typename A>
   void operator()(A a){
      std::ostringstream ss;
      ss << a;
      message_ = ss.str();
   }
   template <typename A, typename B>
   void operator()(A a, B b){
      std::ostringstream ss;
      ss << a << b;
      message_ = ss.str();
   }
   template <typename A, typename B, typename C>
   void operator()(A a, B b, C c){
      std::ostringstream ss;
      ss << a << b << c;
      message_ = ss.str();
   }
   template <typename A, typename B, typename C, typename D>
   void operator()(A a, B b, C c, D d){
      std::ostringstream ss;
      ss << a << b << c << d;
      message_ = ss.str();
   }
};

//for drawing
typedef std::vector<MessageBox *> messageBoxes_t;

#endif