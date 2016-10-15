#ifndef CAVE_BUILDING_TYPE_H
#define CAVE_BUILDING_TYPE_H

#include <string>

#include "SDL.h"
#include "globals.h"

class CaveBuildingType{

public:
   CaveBuildingType(
      std::string name,
      typeNum_t imageNumber,
      SDL_Rect drawRect,
      SDL_Rect baseRect);

private:
   const std::string name_;
   const SDL_Surface* surface_;

   //the x and y values of these are offsets from the building's location
   const SDL_Rect drawRect_;
   const SDL_Rect baseRect_; //for collisions, placement
};

#endif