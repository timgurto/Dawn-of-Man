#ifndef BUILDING_TYPE_H
#define BUILDING_TYPE_H

#include <string>

#include "SDL.h"
#include "globals.h"

class BuildingType{

public:
   BuildingType(std::string name, typeNum_t imageNumber,
                SDL_Rect drawRect, SDL_Rect baseRect, RGB background);
   BuildingType(const BuildingType &original);

   /*bool operator==(const CaveBuildingType &rhs);
   bool operator!=(const CaveBuildingType &rhs);*/
   BuildingType &operator=(const BuildingType &rhs);
   ~BuildingType();

//private:
   std::string name_;
   SDL_Surface *surface_;
   SDL_Surface *iconSurface_;
   typeNum_t imageNumber_;

   RGB background_;

   //the x and y values of these are offsets from the building's location
   SDL_Rect drawRect_;
   SDL_Rect baseRect_; //for collisions, placement
};

#endif