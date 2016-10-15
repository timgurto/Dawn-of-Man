// (C) 2010 Tim Gurto

#ifndef UI_BAR_FUNCTIONS_H
#define UI_BAR_FUNCTIONS_H

//contains the functions used by the UIBar class to fetch icons
//surfaces, and execute actions when clicked.

#include "globals.h"

struct SDL_Surface;
struct GameData;

//surfaceFun_

//BuildingType icons
SDL_Surface *getBuildingTypeIcons(typeNum_t i,
                                  const GameData &game);


//clickFun_

//Selects a building to construct
void selectBuilding(typeNum_t index, GameData &game);


#endif