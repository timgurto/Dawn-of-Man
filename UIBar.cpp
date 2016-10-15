// (C) 2009 Tim Gurto

#include "SDL.h"
#include "globals.h"
#include "misc.h"
#include "GameData.h"
#include "Point.h"
#include "UIBar.h"

const GameData *UIBar::game_ = 0;
const ControlMode *UIBar::mode_ = 0;

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
   if (*mode_ == requiredMode_){

      //blit background
      
      //blit icons
      pixels_t x = rect().x, y = rect().y;
      typeNum_t i = 0;
      while (i != iconCount){
         //draw
         SDL_BlitSurface(surfaceFun_(i, *game_), 0, screen, &makeRect(x, y));

         //iterate
         if (orientation_ == HORIZONTAL)
            x += ICON_SIZE;
         else
            y += ICON_SIZE;

         ++i;
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

void UIBar::set(const GameData *game, const ControlMode *mode){
   game_ = game;
   mode_ = mode;
}