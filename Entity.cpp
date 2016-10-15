// (C) 2009 Tim Gurto

#include <cstdlib> //rand
#include <cassert>

#include "Entity.h"
#include "EntityType.h"
#include "SDL.h"
#include "Point.h"
#include "misc.h"
#include "globals.h"

GameData *Entity::game_ = 0;
SDL_Surface *Entity::screen_ = 0;

Entity::Entity(typeNum_t type, const Point &loc):
type_(type),
loc_(loc),
direction(Direction(rand() % 4)){}

bool Entity::operator<(const Entity &rhs) const{
   return (loc_.y < rhs.loc_.y);
}

void Entity::draw(SDL_Surface *screen) const{
   const EntityType &thisType = type();
   SDL_Rect drawLoc = loc_ + thisType.drawRect_;
   
   if (!MASK_BEFORE_CLIP && ENTITY_MASKS)
      SDL_BlitSurface(thisType.mask, 0, screen, &drawLoc);

   SDL_BlitSurface(thisType.surface, 0, screen, &drawLoc);
}

void Entity::drawLater() const{};

SDL_Rect Entity::getBaseRect(){
   return loc_ + type().baseRect_;   
}

void Entity::tick(){} //default: do nothing

void Entity::init(GameData *game, SDL_Surface *screen){
   game_ = game;
   screen_ = screen;
}

float Entity::getDrawPercent() const{
   return FULL; //default: full
}