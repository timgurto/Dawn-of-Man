// (C) 2010 Tim Gurto

#ifndef MESSAGE_BOX_H
#define MESSAGE_BOX_H

#include <vector>
#include <string>
#include <sstream>
#include "SDL_ttf.h"
#include "Surface.h"
#include "types.h"

//Text that appears on the screen
class MessageBox{
   SDL_Color color_; //font color
   std::string name_; //font name
   TTF_Font *font_; //SDL font object
   pixels_t x_, y_; //co-ordinates of top-left corner
   pixels_t height_; //height of the box
   pixels_t margin_; //padding around the text
   std::string message_; //the message
   Surface background_; //the background image
   bool visible_; //whether the box is drawn

public:
   MessageBox(SDL_Color color, pixels_t x, pixels_t y,
              pixels_t margin,
              Surface &background,
              const std::string &fontName, int fontSize,
              bool accountForHeight = false,
              bool visible = true);

   //Draws messages to the screen
   void display(Surface &screen) const;

   void clear();

   void toggleVisibility();

   //Generic wrappers for changeMessage(string)
   //A, B, C, D must have << defined
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
   template<>
   void operator()<std::string>(std::string s){
      message_ = s;
   }
   template<>
   void operator()<char *>(char *s){
      message_ = s;
   }
};

//for drawing
typedef std::vector<MessageBox *> messageBoxes_t;

#endif