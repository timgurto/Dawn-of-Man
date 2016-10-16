// (C) 2010 Tim Gurto

#include <cassert>
#include "MessageBox.h"
#include "globals.h"
#include "misc.h"
#include "Surface.h"

MessageBox::MessageBox(SDL_Color color,
                       pixels_t x, pixels_t y,
                       pixels_t margin,
                       Surface &background,
                       const std::string &fontName,
                       int fontSize,
                       bool accountForHeight,
                       bool visible):
color_(color),
x_(x),
y_(y),
margin_(margin),
background_(background),
visible_(visible),
font_(TTF_OpenFont(fontName.c_str(), fontSize)){
   assert (font_);
   //font height
   int fontHeight = TTF_FontHeight(font_);
   height_ = fontHeight + 2 * margin_;
   if (accountForHeight)
      y_ -= fontHeight;
}

void MessageBox::display(Surface &screen) const{
   if (visible_)
      if (message_ != ""){
         assert (screen);

         //message surface
         Surface text(font_, message_, color_);
         pixels_t width = text->clip_rect.w + margin_;

         //background
         if (background_){
            screen.fill(ENGRAVE_DARK,
                        &makeRect(x_ - margin_, y_ - margin_,
                                  width + 1, height_ + 1));
            screen.fill(ENGRAVE_LIGHT_UINT,
                        &makeRect(x_ - margin_ - 1, y_ - margin_ - 1,
                                  width + 1, height_ + 1));
            background_.draw(screen,
                             &makeRect(x_ - margin_, y_ - margin_),
                             &makeRect(0, 0, width, height_));
         }

         //message
         text.draw(screen, &makeRect(x_ + margin_, y_ + margin_));
      }
}


void MessageBox::clear(){
   message_.clear();
}

void MessageBox::toggleVisibility(){
   visible_ = !visible_;
}