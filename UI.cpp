// (C) 2009 Tim Gurto

#include "UI.h"
#include "misc.h"
#include "globals.h"
#include "GameData.h"

UI::UI(const std::string &topBarPath,
       const std::string &bottomBarPath,
       const SDL_Color &background):
topBar(loadImage(topBarPath, background)),
bottomBar(loadImage(bottomBarPath, background)){}

UI::~UI(){
   freeSurface(topBar);
   freeSurface(bottomBar);
}

void UI::draw(SDL_Surface *screen,
              const GameData &game,
              ControlMode controlMode) const{
   switch(controlMode){
   case BUILD_MODE:
      break;
   case NORMAL_MODE:
      //draw interface background
      pixels_t bottomBarHeight = game.buildingTypes.size() * ICON_SIZE + UI_BAR_PADDING;
      SDL_BlitSurface(bottomBar, 0, screen, &bottomBarRect(game, controlMode));
      //draw building icons
      int i = 0;
      for (buildingTypes_t::const_iterator it = game.buildingTypes.begin();
           it != game.buildingTypes.end(); ++it, ++i){
         SDL_BlitSurface(it->portrait, 0, screen,
                         &makeRect(0, SCREEN_HEIGHT - ICON_SIZE * (game.buildingTypes.size() - i), 0, 0));
      }
      break;
   }
}

pixels_t UI::bottomBarHeight(const GameData &game, ControlMode controlMode) const{
   switch(controlMode){
   case NORMAL_MODE:
      return game.buildingTypes.size() * ICON_SIZE + UI_BAR_PADDING;
      break;
   }
        
}

SDL_Rect UI::bottomBarRect(const GameData &game, ControlMode controlMode) const{
   switch(controlMode){
   case NORMAL_MODE:
      return makeRect(0, SCREEN_HEIGHT - bottomBarHeight(game, controlMode),
                      ICON_SIZE + UI_BAR_PADDING,
                      bottomBarHeight(game, controlMode));
      break;
   }
}