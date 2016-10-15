// (C) 2009 Tim Gurto

#include "Entity.h"
#include "EntityType.h"
#include "SDL.h"
#include "Point.h"

Entity::Entity(typeNum_t type, const Point &loc):
type_(type),
loc_(loc){}

bool Entity::operator<(const Entity &rhs) const{
   return (loc_.y < rhs.loc_.y);
}

void Entity::draw(SDL_Surface *screen, const GameData &game) const{
   const EntityType &thisType = type(game);
   SDL_Rect drawLoc = loc_ + thisType.drawRect_;
   SDL_BlitSurface(thisType.surface, 0, screen, &drawLoc);
}

SDL_Rect Entity::baseRect(const GameData &game){
   return loc_ + type(game).baseRect_;   
}