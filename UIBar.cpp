// (C) 2009 Tim Gurto

#include "SDL.h"
#include "globals.h"
#include "misc.h"
#include "GameData.h"
#include "Point.h"
#include "UIBar.h"

const GameData *UIBar::game_ = 0;
SDL_Surface *hBarSurface_ = 0;
SDL_Surface *vBarSurface_ = 0;

UIBar::UIBar(Corner corner, Orientation orientation,
             SDL_Surface *(*surfaceFun)(typeNum_t index,
                                        const GameData &game),
             typeNum_t initialIconCount,
             ControlMode requiredMode):
corner_(corner),
orientation_(orientation),
surfaceFun_(surfaceFun),
iconCount(initialIconCount),
requiredMode_(requiredMode){}

//TODO one-by-one
//TODO Make it an attribute instead of a function
SDL_Rect UIBar::rect() const{
   SDL_Rect barRect;

   //x, y
   if (corner_ == TOP_LEFT || corner_ == BOTTOM_LEFT)
      barRect.x = 0;
   else
      if (orientation_ == VERTICAL)
         barRect.x = SCREEN_WIDTH - ICON_SIZE;
      else
         barRect.x = SCREEN_WIDTH - ICON_SIZE * iconCount;
   if (corner_ == TOP_LEFT || corner_ == TOP_RIGHT)
      barRect.y = 0;
   else
      if (orientation_ == HORIZONTAL)
         barRect.y = SCREEN_HEIGHT - ICON_SIZE;
      else
         barRect.y = SCREEN_HEIGHT - ICON_SIZE * iconCount;

   //w, h
   if (orientation_ == VERTICAL){
      barRect.w = ICON_SIZE;
      barRect.h = ICON_SIZE * iconCount;
   }else{
      barRect.w = ICON_SIZE * iconCount;
      barRect.h = ICON_SIZE;
   }
   return barRect;
}

void UIBar::draw(SDL_Surface *screen) const{
   if (game_->mode == requiredMode_){

      //blit background
      SDL_Surface *src = (orientation_ == HORIZONTAL ?
                          hBarSurface_ :
                          vBarSurface_);

      SDL_BlitSurface(src, &makeRect(0, 0, rect().w, rect().h),
                      screen, &rect());
      
      //blit icons
      pixels_t x = rect().x, y = rect().y;
      for (typeNum_t i = 0; i != iconCount; ++i){
         //draw
         SDL_BlitSurface(surfaceFun_(i, *game_), 0, screen, &makeRect(x, y));

         //iterate
         if (orientation_ == HORIZONTAL)
            x += ICON_SIZE;
         else
            y += ICON_SIZE;
      }
   }
}

typeNum_t UIBar::mouseIndex(const Point &point) const{
   //if collision with bar
   SDL_Rect barRect = rect();
   if (pointCollision(point, barRect))
      switch (orientation_){
      case HORIZONTAL:
         return (point.x - barRect.x) / ICON_SIZE;
         break;
      case VERTICAL:
         return (point.y - barRect.y) / ICON_SIZE;
         break;
      }
   return NO_TYPE;
}

void UIBar::init(const GameData *game,
                SDL_Surface *vBarSurface,
                SDL_Surface *hBarSurface){
   game_ = game;
   vBarSurface_ = vBarSurface;
   hBarSurface_ = hBarSurface;
}