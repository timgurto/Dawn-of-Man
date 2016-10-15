// (C) 2010 Tim Gurto

#ifndef UI_BAR_FUNCTIONS_H
#define UI_BAR_FUNCTIONS_H

//contains the functions used by the UIBar class to fetch icons
//surfaces, and execute actions when clicked.

#include "types.h"

struct SDL_Surface;
struct GameData;


//iconCountFun_
typedef typeNum_t iconCountFun(const GameData &game);
typedef iconCountFun *iconCountFunPtr;
   
   //Buildings bar: number of BuildingType icons
   iconCountFun getNumBuildingIcons;

   //Units bar: 
   iconCountFun getNumUnitIcons;


//surfaceFun_
typedef SDL_Surface *surfaceFun(typeNum_t i, const GameData &game);
typedef surfaceFun *surfaceFunPtr;
   
   //Buildings bar: BuildingType icons
   surfaceFun getBuildingTypeIcons;

   //Units bar: UnitType icons, based on the selected building
   surfaceFun getUnitTypeIcons;


//clickFun_
typedef void clickFun(typeNum_t index, GameData &game);
typedef clickFun *clickFunPtr;

   //Buildings bar: Selects a building to construct
   clickFun selectBuilding;

   //Units bar: Trains the chosen unit, at the selected building
   clickFun trainUnit;

#endif