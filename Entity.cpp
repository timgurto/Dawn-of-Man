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
   //const EntityType &thisType = type();
   //SDL_Rect drawLoc = loc_ + thisType.drawRect_;
   //if (ENTITY_MASKS)
   //   SDL_BlitSurface(thisType.mask, 0, screen, &drawLoc);
   //if (getAlpha() != OPAQUE)
   //   SDL_SetAlpha(thisType.surface, SDL_SRCALPHA, getAlpha());
   //SDL_BlitSurface(thisType.surface, 0, screen, &drawLoc);
   //SDL_SetAlpha(thisType.surface, SDL_SRCALPHA, OPAQUE);

   const EntityType &thisType = type();
   SDL_Rect drawLoc = loc_ + thisType.drawRect_;
   SDL_Rect srcLoc;
   //short direction = rand() % 4;
   pixels_t
      partialW = thisType.drawRect_.w * getAlpha() / 255,
      partialH = thisType.drawRect_.h * getAlpha() / 255;
   switch(direction){
   case 0:
      srcLoc = makeRect(
                  0,
                  0,
                  partialW,
                  thisType.drawRect_.h);
      break;
   case 1:
      srcLoc = makeRect(
                  0,
                  0,
                  thisType.drawRect_.w,
                  partialH);
      break;
   case 2:
      srcLoc = makeRect(
                  thisType.drawRect_.w - partialW,
                  0,
                  partialW,
                  thisType.drawRect_.h);
      drawLoc.x += thisType.drawRect_.w - partialW;
      break;
   case 3:
      srcLoc = makeRect(
                  0,
                  thisType.drawRect_.h - partialH,
                  thisType.drawRect_.w,
                  partialH);
      drawLoc.y += thisType.drawRect_.h - partialH;
      break;
   default:
      assert(false);
   }
   
   if (ENTITY_MASKS)
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

Uint8 Entity::getAlpha() const{
   return OPAQUE; //default: opaque
}