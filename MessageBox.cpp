// (C) 2010 Tim Gurto

#include <cassert>
#include "MessageBox.h"
#include "globals.h"
#include "misc.h"

MessageBox::MessageBox(SDL_Color color,
                       pixels_t x, pixels_t y,
                       pixels_t margin,
                       SDL_Surface *background,
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

void MessageBox::display(SDL_Surface *screen) const{
   if (visible_)
      if (message_ != ""){
         assert (screen);

         //message surface
         SDL_Surface *text = TTF_RenderText_Solid(font_,
                                                  message_.c_str(),
                                                  color_);
         pixels_t width = text->clip_rect.w + margin_;

         //background
         if (background_){
            SDL_FillRect(screen,
                         &makeRect(x_ - margin_, y_ - margin_,
                         width + 1, height_ + 1),
                         ENGRAVE_DARK_UINT);
            SDL_FillRect(screen,
                         &makeRect(x_ - margin_ - 1, y_ - margin_ - 1,
                                   width + 1, height_ + 1),
                         ENGRAVE_LIGHT_UINT);
            SDL_BlitSurface(background_, &makeRect(0, 0, width, height_),
                            screen, &makeRect(x_ - margin_, y_ - margin_));
         }

         //message
         SDL_BlitSurface(text, 0,
                         screen, &makeRect(x_ + margin_, y_ + margin_));
      }
}


void MessageBox::clear(){
   message_.clear();
}

void MessageBox::toggleVisibility(){
   visible_ = !visible_;
}