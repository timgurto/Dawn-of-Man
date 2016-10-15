#include <sstream>
#include <string>

#include "SDL.h"
#include "misc.h"
#include "globals.h"
#include "BuildingType.h"
#include "Debug.h"

extern Debug debug;

BuildingType::BuildingType(std::string name,
                           typeNum_t imageNumber,
                           SDL_Rect drawRect,
                           SDL_Rect baseRect,
                           SDL_Color background):
name_(name),
imageNumber_(imageNumber),
drawRect_(drawRect),
baseRect_(baseRect),
background_(background){
   surface_ = loadImage(makePath(Building, imageNumber_), background_);
   iconSurface_ = loadImage(makePath(Building, imageNumber_, true), background_);
}

BuildingType::BuildingType(const BuildingType &original):
name_(original.name_),
imageNumber_(original.imageNumber_),
drawRect_(original.drawRect_),
baseRect_(original.baseRect_),
background_(original.background_){
   surface_ = loadImage(makePath(Building, imageNumber_), background_);
   iconSurface_ = loadImage(makePath(Building, imageNumber_, true), background_);
}

BuildingType &BuildingType::operator=(const BuildingType &rhs){
   name_ = rhs.name_;
   imageNumber_ = rhs.imageNumber_;
   drawRect_ = rhs.drawRect_;
   baseRect_ = rhs.baseRect_;
   background_ = rhs.background_;
   surface_ = loadImage(makePath(Building, imageNumber_), background_);
   iconSurface_ = loadImage(makePath(Building, imageNumber_, true), background_);
   return *this;
}

BuildingType::~BuildingType(){
   debug("Unloading BuildingType surface");
   freeSurface(surface_);
   debug("Unloading BuildingType icon");
   freeSurface(iconSurface_);
   debug("Done unloading BuildingType");
}