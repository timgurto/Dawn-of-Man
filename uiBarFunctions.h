// (C) 2010 Tim Gurto

#ifndef UI_BAR_FUNCTIONS_H
#define UI_BAR_FUNCTIONS_H

//contains the functions used by the UIBar class to fetch icons
//surfaces, and execute actions when clicked.

#include "globals.h"

struct SDL_Surface;
struct GameData;

//surfaceFun_
typedef SDL_Surface *surfaceFunPtr(typeNum_t i, const GameData &game);
surfaceFunPtr
   getBuildingTypeIcons;//BuildingType icons


//clickFun_
typedef void clickFunPtr(typeNum_t index, GameData &game);
clickFunPtr
   selectBuilding; //Selects a building to construct

#endif