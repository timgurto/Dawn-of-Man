// (C) 2010 Tim Gurto

#include <string>
#include <cassert>

#include "SDL.h"
#include "SDL_image.h"
#include "Surface.h"
#include "Screen.h"
#include "globals.h"
#include "util.h"

int Surface::surfacesLoaded_ = 0;
int Surface::screensSet_ = 0;

//constructor - from file
Surface::Surface(const std::string fileName, bool alpha):
isScreen_(false),
surface_(0){
   loadImage(fileName, alpha);
   ++surfacesLoaded_;
}

//constructor - from file, with transparent background
Surface::Surface(const std::string fileName, const SDL_Color &background, bool alpha):
isScreen_(false),
surface_(0){
   loadImage(fileName, alpha);
   setColorKey(background);
   ++surfacesLoaded_;
}

//constructor - special (blank, screen, or default uninitialized)
Surface::Surface(SpecialSurface special, int width, int height,
                 SDL_Color background):
isScreen_(false),
surface_(0){
   switch(special){

   case SUR_SCREEN:
      if (screensSet_ != 0) //don't make duplicate screen buffers
         break;

      surface_ = SDL_SetVideoMode(Screen::getScreenRes().x,
                                  Screen::getScreenRes().y,
                                  SCREEN_BPP,
                                  SDL_HWSURFACE | (Screen::getWindowedMode() ?
                                  0 :
                                  SDL_FULLSCREEN));
      SDL_WM_SetCaption("Game", 0);
      isScreen_ = true;
      assert (screensSet_ == 0);
      screensSet_ = 1;
      break;

   case SUR_BLANK:
      if (width == -1)
         width = Screen::getScreenRes().x;
      if (height == -1)
         height = Screen::getScreenRes().y;
      surface_ = SDL_CreateRGBSurface(SDL_HWSURFACE,
                                   width, height, SCREEN_BPP,
                                   0, 0, 0, 0);
      setColorKey(background);
      break;
   }
   if (surface_ && !isScreen_)
      ++surfacesLoaded_;
}

//constructor - text
Surface::Surface(TTF_Font *font, std::string message, SDL_Color color):
surface_(TTF_RenderText_Solid(font, message.c_str(), color)),
isScreen_(false){
   if (surface_)
      ++surfacesLoaded_;
}

Surface::Surface(const Surface &original):
surface_(0),
isScreen_(false){
   if (!original)
      return;

   else
      if (original.isScreen_){
         surface_ = original.surface_;
         isScreen_ = true;
         ++screensSet_;
      }else{
         surface_ = SDL_ConvertSurface(original.surface_,
                                       original.surface_->format,
                                       SDL_HWSURFACE);
         ++surfacesLoaded_;
      }
}

Surface::~Surface(){
   if (surface_){
      if (isScreen_)
         --screensSet_;
      else{
         SDL_FreeSurface(surface_);
         --surfacesLoaded_;
      }
   }
}

Surface &Surface::operator=(const Surface &rhs){
   if (this == &rhs)
      return *this;

   if (surface_){
      SDL_FreeSurface(surface_);
      --surfacesLoaded_;
   }
   
   if (rhs.isScreen_){
      surface_ = rhs.surface_;
      isScreen_ = true;
      ++screensSet_;
   }else{
      surface_ = SDL_ConvertSurface(rhs.surface_,
                                    rhs.surface_->format,
                                    SDL_HWSURFACE);
      ++surfacesLoaded_;
   }
   return *this;
}

Surface::operator bool() const{
   //return surface_;
   return surface_ != 0;
}

SDL_Surface *Surface::operator->(){
   return surface_;
}

const SDL_Surface *Surface::operator->() const{
   return surface_;
}

//needs to be called once, in main or wherever
void Surface::init(){}

//needs to be called once, in main or wherever
void Surface::quit(){
   assert(surfacesLoaded_ == 0);
}

//load a surface from an image file
void Surface::loadImage(const std::string &fileName, bool alpha){
   SDL_Surface *load = IMG_Load(fileName.c_str());
   //assert(load);
   surface_ = alpha ?
      SDL_DisplayFormatAlpha(load) :
      SDL_DisplayFormat(load);
   assert(surface_);
   SDL_FreeSurface(load);
}

//set the surface's transparent color
void Surface::setColorKey(const SDL_Color &color){
   if (color != NO_COLOR)
      SDL_SetColorKey(surface_,
                      SDL_SRCCOLORKEY,
                      SDL_MapRGB(surface_->format,
                                 color.r,
                                 color.g,
                                 color.b));
}

//fills a part of the surface with color
void Surface::fill(const SDL_Color color, SDL_Rect *rect){
   fill(colorToUInt(color), rect);
}

//fills a part of the surface with color
void Surface::fill(Uint32 color, SDL_Rect *rect){
   SDL_FillRect(surface_, rect, color);
}

//draw onto another surface
void Surface::draw(Surface &dst,
                   SDL_Rect *dstRect, SDL_Rect *srcRect) const{
   SDL_BlitSurface(surface_, srcRect, dst.surface_, dstRect);
}

void Surface::draw(Surface &dst, Point dstPoint, SDL_Rect *srcRect) const{
   SDL_Rect rect = makeRect(dstPoint);
   draw(dst, &rect, srcRect);
}

//dst << src: draw
Surface &operator<<(Surface &dst, const Surface &src){
   src.draw(dst);
   return dst;
}

//sets the surface's alpha value (0-ff)
void Surface::setAlpha(Uint8 alpha){
   SDL_SetAlpha(surface_, SDL_SRCALPHA, alpha);
}

//flips the screen buffer
void Surface::flip(){
   assert(isScreen_);
   bool test;
   test = SDL_Flip(surface_) == 0;
   assert(test);
}

//saves the surface as an image file
void Surface::saveToBitmap(std::string &fileName) const{
   SDL_SaveBMP(surface_, fileName.c_str());
}