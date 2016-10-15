// (C) 2009-2010 Tim Gurto

#include "SDL.h"
#include "globals.h"
#include "misc.h"
#include "GameData.h"
#include "Point.h"
#include "UIBar.h"

GameData *UIBar::game_ = 0;
SDL_Surface *UIBar::screen_ = 0;
SDL_Surface *UIBar::hBarSurface_ = 0;
SDL_Surface *UIBar::vBarSurface_ = 0;

UIBar::UIBar(Corner corner, Orientation orientation,
             SDL_Surface *(*surfaceFun)(typeNum_t index,
                                        const GameData &game),
             void (*clickFunction)(typeNum_t index,
                                   GameData &game),
             typeNum_t initialIconCount,
             ControlMode requiredMode):
corner_(corner),
orientation_(orientation),
surfaceFun_(surfaceFun),
clickFun(clickFunction),
iconCount(initialIconCount),
requiredMode_(requiredMode){
   calculateRect();
}

void UIBar::calculateRect(){
   if (orientation_ == VERTICAL){
      switch (corner_){
      case TOP_LEFT:
         rect.x = 0;
         rect.y = 0;
         break;
      case TOP_RIGHT:
         rect.x = SCREEN_WIDTH - ICON_SIZE;
         rect.y = 0;
         break;
      case BOTTOM_LEFT:
         rect.x = 0;
         rect.y = SCREEN_HEIGHT - ICON_SIZE * iconCount;
         break;
      case BOTTOM_RIGHT:
         rect.x = SCREEN_WIDTH - ICON_SIZE;
         rect.y = SCREEN_HEIGHT - ICON_SIZE * iconCount;
         break;
      }
      rect.w = ICON_SIZE;
      rect.h = ICON_SIZE * iconCount;
   }else{
      switch (corner_){
      case TOP_LEFT:
         rect.x = 0;
         rect.y = 0;
         break;
      case TOP_RIGHT:
         rect.x = SCREEN_WIDTH - ICON_SIZE * iconCount;
         rect.y = 0;
         break;
      case BOTTOM_LEFT:
         rect.x = 0;
         rect.y = SCREEN_HEIGHT - ICON_SIZE;
         break;
      case BOTTOM_RIGHT:
         rect.x = SCREEN_WIDTH - ICON_SIZE * iconCount;
         rect.y = SCREEN_HEIGHT - ICON_SIZE;
         break;
      }
      rect.w = ICON_SIZE * iconCount;
      rect.h = ICON_SIZE;
   }
}

void UIBar::draw() const{
   if (game_->mode == requiredMode_){

      //blit background
      SDL_Surface *src = (orientation_ == HORIZONTAL ?
                          hBarSurface_ :
                          vBarSurface_);

      SDL_BlitSurface(src, &makeRect(0, 0, rect.w, rect.h),
                      screen_, &SDL_Rect(rect));
      
      //blit icons
      pixels_t x = rect.x, y = rect.y;
      for (typeNum_t i = 0; i != iconCount; ++i){
         //draw
         SDL_BlitSurface(surfaceFun_(i, *game_), 0, screen_, &makeRect(x, y));

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
   SDL_Rect barRect = rect;
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

bool UIBar::isActive(ControlMode mode){
   return (mode == requiredMode_);
}

void UIBar::init(GameData *game,
                 SDL_Surface *screen,
                 SDL_Surface *vBarSurface,
                 SDL_Surface *hBarSurface){
   game_ = game;
   screen_ = screen;
   vBarSurface_ = vBarSurface;
   hBarSurface_ = hBarSurface;
}