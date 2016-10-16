// (C) 2009-2010 Tim Gurto

#include "EntityType.h"
#include "DecorationType.h"
#include "types.h"

DecorationType::DecorationType(typeNum_t index,
                               const std::string &name,
                               const SDL_Rect &drawRect,
                               const SDL_Rect &baseRect,
                               bool collides):
EntityType(index,
           ENT_DECORATION,
           name,
           drawRect,
           baseRect),
collides_(collides){}