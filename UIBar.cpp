// (C) 2009-2010 Tim Gurto

#include "SDL.h"
#include "types.h"
#include "misc.h"
#include "GameData.h"
#include "Point.h"
#include "UIBar.h"

GameData *UIBar::game_ = 0;
SDL_Surface *UIBar::screen_ = 0;
SDL_Surface *UIBar::hBarSurface_ = 0;
SDL_Surface *UIBar::vBarSurface_ = 0;

UIBar::UIBar(Corner corner, Orientation orientation,
             iconCountFunPtr iconCountFun,
             surfaceFunPtr surfaceFun,
             clickFunPtr clickFun,
             ControlMode requiredMode):
corner_(corner),
clickFun_(clickFun),
orientation_(orientation),
surfaceFun_(surfaceFun),
iconCountFun_(iconCountFun),
requiredMode_(requiredMode){
   calculateRect();
}

//TODO fix, so that the edge inside the screen has a border
void UIBar::calculateRect(){
   iconCount_ = iconCountFun_(*game_);
   if (orientation_ == VERTICAL){
      switch (corner_){
      case TOP_LEFT:
         rect_.x = 0;
         rect_.y = 0;
         break;
      case TOP_RIGHT:
         rect_.x = SCREEN_WIDTH - ICON_SIZE;
         rect_.y = 0;
         break;
      case BOTTOM_LEFT:
         rect_.x = 0;
         rect_.y = SCREEN_HEIGHT - ICON_SIZE * iconCount_;
         break;
      case BOTTOM_RIGHT:
         rect_.x = SCREEN_WIDTH - ICON_SIZE;
         rect_.y = SCREEN_HEIGHT - ICON_SIZE * iconCount_;
         break;
      }
      rect_.w = ICON_SIZE;
      rect_.h = ICON_SIZE * iconCount_;
   }else{
      switch (corner_){
      case TOP_LEFT:
         rect_.x = 0;
         rect_.y = 0;
         break;
      case TOP_RIGHT:
         rect_.x = SCREEN_WIDTH - ICON_SIZE * iconCount_;
         rect_.y = 0;
         break;
      case BOTTOM_LEFT:
         rect_.x = 0;
         rect_.y = SCREEN_HEIGHT - ICON_SIZE;
         break;
      case BOTTOM_RIGHT:
         rect_.x = SCREEN_WIDTH - ICON_SIZE * iconCount_;
         rect_.y = SCREEN_HEIGHT - ICON_SIZE;
         break;
      }
      rect_.w = ICON_SIZE * iconCount_;
      rect_.h = ICON_SIZE;
   }
}

void UIBar::draw() const{
   if (game_->mode == requiredMode_){

      //blit background
      SDL_Surface *src = (orientation_ == HORIZONTAL ?
                          hBarSurface_ :
                          vBarSurface_);

      SDL_BlitSurface(src, &makeRect(0, 0, rect_.w, rect_.h),
                      screen_, &SDL_Rect(rect_));
      
      //blit icons
      pixels_t x = rect_.x, y = rect_.y;
      for (typeNum_t i = 0; i != iconCount_; ++i){
         //draw
         SDL_BlitSurface(surfaceFun_(i, iconCount_, *game_), 0,
                         screen_, &makeRect(x, y));

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
   SDL_Rect barRect = rect_;
   if (collision(point, barRect))
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

void UIBar::click(typeNum_t index){
   clickFun_(index, iconCount_, *game_);
}