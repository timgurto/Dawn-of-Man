// (C) 2009-2010 Tim Gurto

#include "types.h"
#include "misc.h"
#include "GameData.h"
#include "Point.h"
#include "UIBar.h"
#include "Surface.h"

GameData *UIBar::game_ = 0;
const CoreData *UIBar::core_ = 0;
Surface *UIBar::screen_ = 0;
Surface *UIBar::barSurface_ = 0;
SDL_Sound *UIBar::click_ = 0;

UIBar::UIBar(Corner corner,
             Orientation orientation,
             iconCountFunPtr iconCountFun,
             surfaceFunPtr surfaceFun,
             clickFunPtr clickFun,
             helpFunPtr helpFun,
             ControlMode requiredMode):
corner_(corner),
orientation_(orientation),
iconCountFun_(iconCountFun),
surfaceFun_(surfaceFun),
clickFun_(clickFun),
helpFun_(helpFun),
requiredMode_(requiredMode){
   calculateRect();
}

void UIBar::calculateRect(){
   iconCount_ = iconCountFun_(*core_, *game_);
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

      Surface
         &screen = *screen_,
         &barSurface = *barSurface_;

      //shadow
      screen.fill(ENGRAVE_LIGHT,
                  &SDL_Rect(rect_ - Point(1, 1)));
      screen.fill(ENGRAVE_DARK,
                  &SDL_Rect(rect_ + Point(1, 1)));
      barSurface.draw(screen,
                      &SDL_Rect(rect_),
                      &makeRect(0, 0, rect_.w, rect_.h));
      
      //blit icons
      pixels_t x = rect_.x, y = rect_.y;
      for (typeNum_t i = 0; i != iconCount_; ++i){
         //draw
         surfaceFun_(i, *core_, *game_)
            .draw(screen, &makeRect(x, y));

         //iterate
         if (orientation_ == HORIZONTAL)
            x += ICON_SIZE;
         else
            y += ICON_SIZE;
      }
   }
}

typeNum_t UIBar::mouseIndex() const{
   //if collision with bar
   const Point &point = game_->mousePos;
   SDL_Rect barRect = rect_;
   if (collision(point, barRect))
      switch (orientation_){
      case HORIZONTAL:
         if (point.x >= barRect.w)
            return NO_TYPE;
         return (point.x - barRect.x) / ICON_SIZE;
         //break;
      case VERTICAL:
         if (point.y >= barRect.h)
            return NO_TYPE;
         return (point.y - barRect.y) / ICON_SIZE;
         //break;
      }
   return NO_TYPE;
}

bool UIBar::isActive(){
   return (game_->mode == requiredMode_);
}

void UIBar::init(const CoreData *core,
                 GameData *game,
                 Surface *screen,
                 Surface *barSurface,
                 SDL_Sound *click){
   core_ = core;
   game_ = game;
   screen_ = screen;
   barSurface_ = barSurface;
   click_ = click;
}

void UIBar::click(typeNum_t index){
   if (index == NO_TYPE)
      index = mouseIndex();
   clickFun_(index, *core_, *game_);
   calculateRect();
}

std::string UIBar::helpText(typeNum_t index){
   return helpFun_(index, *core_, *game_);
}

typeNum_t UIBar::size() const{
   return iconCountFun_(*core_, *game_);
}

void UIBar::clickSound(){
   playSound(click_);
}