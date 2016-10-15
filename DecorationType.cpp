// (C) 2009 Tim Gurto

#include "EntityType.h"
#include "DecorationType.h"
#include "types.h"

DecorationType::DecorationType(typeNum_t index,
                               const std::string &name,
                               const SDL_Rect &drawRect,
                               const SDL_Rect &baseRect,
                               const Point &selectionCenter):
EntityType(index, DECORATION, name, drawRect, baseRect,
           selectionCenter){}