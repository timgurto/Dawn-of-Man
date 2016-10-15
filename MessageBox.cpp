// (C) 2010 Tim Gurto

#include <cassert>
#include "MessageBox.h"
#include "globals.h"
#include "misc.h"

MessageBox::MessageBox(SDL_Color color,
                       pixels_t x, pixels_t y,
                       const std::string &fontName,
                       int fontSize):
color_(color),
x_(x),
y_(y),
font_(TTF_OpenFont(fontName.c_str(), fontSize)){
   assert (font_ != 0);
}

void MessageBox::display(SDL_Surface *screen) const{
   assert (screen != 0);

   SDL_Surface *text = TTF_RenderText_Solid(font_,
                                            message_.c_str(),
                                            color_);
   SDL_BlitSurface(text, 0, screen, &makeRect(x_, y_));
}


void MessageBox::clear(){
   message_.clear();
}