// (C) 2010 Tim Gurto

#include <cassert>
#include "Screen.h"
#include "Surface.h"

//TODO comments

Surface *Screen::background_ = 0;
Surface *Screen::screen_ = 0;
Surface *Screen::cursor_ = 0;


void Screen::go(){
   while(true){
      handleEvents();
      draw();
   }

}

void Screen::handleEvents(){
   
}

void Screen::draw(){
   assert(screen_);
   screen_->fill();
   *screen_ << *background_;
   screen_->flip();
}

void Screen::init(Surface *background,
                  Surface *screen,
                  Surface *cursor){
   background_ = background;
   screen_ = screen;
   cursor_ = cursor;
}