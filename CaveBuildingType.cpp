#include <sstream>
#include <string>

#include "SDL.h"
#include "SDL_wrappers.h"
#include "globals.h"
#include "CaveBuildingType.h"

CaveBuildingType::CaveBuildingType(std::string name, typeNum_t imageNumber,
   SDL_Rect drawRect, SDL_Rect baseRect):
name_(name),
drawRect_(drawRect),
baseRect_(baseRect){
   std::stringstream pathStream;
   pathStream << IMAGE_PATH << BUILDINGS_IMAGE_PATH << imageNumber << IMAGE_SUFFIX;
   surface_ = loadImage(pathStream.str().c_str());
}
