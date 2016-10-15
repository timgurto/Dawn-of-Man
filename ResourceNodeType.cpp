// (C) 2010 Tim Gurto

#include "EntityType.h"
#include "ResourceNodeType.h"
#include "Resources.h"
#include "globals.h"

ResourceNodeType::ResourceNodeType
   (typeNum_t index,
   const std::string &name,
   const SDL_Rect &drawRect,
   const SDL_Rect &baseRect,
   const Point &selectionCenter,
   const Resources &maxResources,
   const Resources &yield,
   EntityColor color):
EntityType(index,
           ENT_RESOURCE_NODE,
           name,
           drawRect,
           baseRect,
           selectionCenter),
maxResources_(maxResources),
yield_(yield),
color_(color){}