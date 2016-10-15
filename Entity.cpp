// (C) 2009 Tim Gurto

#include "Entity.h"
#include "EntityType.h"
#include "SDL.h"
#include "Point.h"

const GameData *Entity::game_ = 0;

Entity::Entity(typeNum_t type, const Point &loc):
type_(type),
loc_(loc){}

bool Entity::operator<(const Entity &rhs) const{
   return (loc_.y < rhs.loc_.y);
}

void Entity::draw(SDL_Surface *screen) const{
   const EntityType &thisType = type();
   SDL_Rect drawLoc = loc_ + thisType.drawRect_;
   if (getAlpha() != OPAQUE)
      SDL_SetAlpha(thisType.surface, SDL_SRCALPHA, getAlpha());
   SDL_BlitSurface(thisType.surface, 0, screen, &drawLoc);
   SDL_SetAlpha(thisType.surface, SDL_SRCALPHA, OPAQUE);
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