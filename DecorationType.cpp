// (C) 2009 Tim Gurto

#include "EntityType.h"
#include "DecorationType.h"
#include "globals.h"

DecorationType::DecorationType(typeNum_t index,
                               const std::string &name,
                               const SDL_Rect &drawRect,
                               const SDL_Rect &baseRect):
EntityType(index, DECORATION, name, drawRect, baseRect){}