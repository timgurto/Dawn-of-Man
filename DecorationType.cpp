// (C) 2009, 2010 Tim Gurto

#include "EntityType.h"
#include "DecorationType.h"
#include "types.h"
#include "misc.h"

pixels_t DecorationType::DECORATION_DISPLACEMENT = 150;

DecorationType::DecorationType(typeNum_t index,
                               const std::string &name,
                               const SDL_Rect &drawRect,
                               const SDL_Rect &baseRect,
                               bool collides,
                               EntityColor color):
EntityType(index,
           ENT_DECORATION,
           name,
           drawRect + Point(0, DECORATION_DISPLACEMENT),
           baseRect + Point(0, DECORATION_DISPLACEMENT)),
collides_(collides),
color_(color){}