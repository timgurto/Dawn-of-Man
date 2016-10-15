// (C) 2009 Tim Gurto

#include <cstdlib> //rand
#include <cassert>

#include "Entity.h"
#include "EntityType.h"
#include "SDL.h"
#include "Point.h"
#include "misc.h"

const GameData *Entity::game_ = 0;

Entity::Entity(typeNum_t type, const Point &loc):
type_(type),
loc_(loc),
direction(rand() % 4){}

bool Entity::operator<(const Entity &rhs) const{
   return (loc_.y < rhs.loc_.y);
}

void Entity::draw(SDL_Surface *screen) const{
   const EntityType &thisType = type();
   SDL_Rect drawLoc = loc_ + thisType.drawRect_;

if (MASK_BEFORE_CLIP && ENTITY_MASKS)
   SDL_BlitSurface(thisType.mask, 0, screen, &drawLoc);

   SDL_Rect srcLoc;;
   pixels_t
      partialW = pixels_t(getDrawPercent() *
                          thisType.drawRect_.w),
      partialH = pixels_t(getDrawPercent() *
                          thisType.drawRect_.h);

   //clip, based on randomized direction
   switch(direction){
   case 0:
      srcLoc = makeRect(0,
                        0,
                        partialW,
                        thisType.drawRect_.h);
      break;
   case 1:
      srcLoc = makeRect(0,
                        0,
                        thisType.drawRect_.w,
                        partialH);
      break;
   case 2:
      srcLoc = makeRect(thisType.drawRect_.w - partialW,
                        0,
                        partialW,
                        thisType.drawRect_.h);
      drawLoc.x += thisType.drawRect_.w - partialW;
      break;
   case 3:
      srcLoc = makeRect(0,
                        thisType.drawRect_.h - partialH,
                        thisType.drawRect_.w,
                        partialH);
      drawLoc.y += thisType.drawRect_.h - partialH;
      break;
   default:
      assert(false);
   }
   
   if (!MASK_BEFORE_CLIP && ENTITY_MASKS)
      SDL_BlitSurface(thisType.mask, &srcLoc, screen, &drawLoc);

   SDL_BlitSurface(thisType.surface, &srcLoc, screen, &drawLoc);
}

SDL_Rect Entity::baseRect(){
   return loc_ + type().baseRect_;   
}

void Entity::tick(){} //default: do nothing

void Entity::setGame(GameData *game){
   game_ = game;
}

float Entity::getDrawPercent() const{
   return FULL; //default: full
}