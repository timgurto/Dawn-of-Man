// (C) 2010 Tim Gurto

#include <string>
#include <cassert>

#include "SDL.h"
#include "SDL_image.h"
#include "Surface.h"
#include "globals.h"
#include "misc.h"

extern bool WINDOWED_MODE;

int Surface::surfacesLoaded_ = 0;
int Surface::screensSet_ = 0;

Surface::Surface(const std::string fileName, bool alpha):
isScreen_(false){
   loadImage(fileName, alpha);
   ++surfacesLoaded_;
}

Surface::Surface(const std::string fileName, const SDL_Color &background, bool alpha):
isScreen_(false){
   loadImage(fileName, alpha);
   setColorKey(background);
   ++surfacesLoaded_;
}

Surface::Surface(SpecialSurface special, int width, int height,
                 SDL_Color background):
isScreen_(false),
surface_(0){
   switch(special){

   case SUR_SCREEN:
      if (screensSet_ != 0) //don't make duplicate screen buffers
         break;

      surface_ = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP,
                                  SDL_HWSURFACE | (WINDOWED_MODE ?
                                  0 :
                                  SDL_FULLSCREEN));
      SDL_WM_SetCaption("Dawn of Man", 0);
      isScreen_ = true;
      assert (screensSet_ == 0);
      screensSet_ = 1;
      break;

   case SUR_BLANK:
      surface_ = SDL_CreateRGBSurface(SDL_HWSURFACE,
                                   width, height, SCREEN_BPP,
                                   0, 0, 0, 0);
      setColorKey(background);
      break;
   }
   if (surface_ && !isScreen_)
      ++surfacesLoaded_;
}

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
   //use ternary operator to suppress performance warning, ironically
   return surface_ ?
             true :
             false;
}

SDL_Surface *Surface::operator->(){
   return surface_;
}

const SDL_Surface *Surface::operator->() const{
   return surface_;
}

void Surface::init(){}

void Surface::quit(){
   assert(surfacesLoaded_ == 0);
}

//TODO comments

void Surface::loadImage(const std::string &fileName, bool alpha){
   SDL_Surface *load = IMG_Load(fileName.c_str());
   assert(load);
   surface_ = alpha ?
      SDL_DisplayFormatAlpha(load) :
      SDL_DisplayFormat(load);
   assert(surface_);
   SDL_FreeSurface(load);
}

void Surface::setColorKey(const SDL_Color &color){
   if (color != NO_COLOR)
      SDL_SetColorKey(surface_,
                      SDL_SRCCOLORKEY,
                      SDL_MapRGB(surface_->format,
                                 color.r,
                                 color.g,
                                 color.b));
}

void Surface::fill(const SDL_Color color, SDL_Rect *rect){
   fill(colorToUInt(color), rect);
}

void Surface::fill(Uint32 color, SDL_Rect *rect){
   SDL_FillRect(surface_, rect, color);
}

void Surface::draw(Surface &dst,
                   SDL_Rect *dstRect, SDL_Rect *srcRect) const{
   SDL_BlitSurface(surface_, srcRect, dst.surface_, dstRect);
}

void operator<<(Surface &dst, const Surface &src){
   src.draw(dst);
}

void Surface::setAlpha(Uint8 alpha){
   SDL_SetAlpha(surface_, SDL_SRCALPHA, alpha);
}

void Surface::flip(){
   assert(isScreen_);
   bool test;
   test = SDL_Flip(surface_) == 0;
   assert(test);
}

void Surface::saveToBitmap(std::string &fileName) const{
   SDL_SaveBMP(surface_, fileName.c_str());
}