// (C) 2010 Tim Gurto

#include "uiBarFunctions.h"
#include "GameData.h"
#include "Debug.h"
#include "BuildingType.h"

extern Debug debug;

//surfaceFun_

SDL_Surface *getBuildingTypeIcons(typeNum_t i,
                                  const GameData &game){
   return game.buildingTypes[i].getIcon();
}


//clickFun_

void selectBuilding(typeNum_t index, GameData &game){
   game.toBuild = index;
   debug("Building to construct: ",
         game.buildingTypes[game.toBuild].getName());
   game.mode = BUILD_MODE;
}