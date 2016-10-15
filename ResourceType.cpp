// (C) 2010 Tim Gurto

#include "EntityType.h"
#include "ResourceType.h"

ResourceType::ResourceType(typeNum_t index,
                           const std::string &name,
                           const SDL_Rect &drawRect,
                           const SDL_Rect &baseRect,
                           const Point &selectionCenter):
EntityType(index,
           ENT_RESOURCE,
           name,
           drawRect,
           baseRect,
           selectionCenter){}