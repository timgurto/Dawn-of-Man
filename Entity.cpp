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
SDL_Surface *Entity::colorTemp = 0;

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
   SDL_Rect srcLoc = makeRect(0, 0,
                              thisType.drawRect_.w,
                              thisType.drawRect_.h);
   layeredBlit(&srcLoc, &drawLoc, screen);
}

void Entity::layeredBlit(SDL_Rect *srcLoc,
                         SDL_Rect *dstLoc,
                         SDL_Surface *screen) const{
   const EntityType &thisType = type();
   //TODO shadows
   //color sprite
   SDL_FillRect(colorTemp, 0, getColor());
   SDL_BlitSurface(thisType.surface, 0, colorTemp, 0);

   //blit mask, hiding anything that would otherwise
   //show through the gaps in the sprite
   if (!MASK_BEFORE_CLIP && ENTITY_MASKS)
      SDL_BlitSurface(thisType.mask, srcLoc, screen, dstLoc);
   
   //blit the sprite
   SDL_BlitSurface(colorTemp, srcLoc, screen, dstLoc);
}

SDL_Rect Entity::getBaseRect(){
   return loc_ + type().baseRect_;   
}

void Entity::tick(){} //default: do nothing

void Entity::init(GameData *game, SDL_Surface *screen){
   game_ = game;
   screen_ = screen;
   colorTemp =SDL_CreateRGBSurface(SDL_HWSURFACE,
                                   SCREEN_WIDTH,
                                   SCREEN_HEIGHT, SCREEN_BPP,
                                   0, 0, 0, 0);
   SDL_SetColorKey(colorTemp, SDL_SRCCOLORKEY,
                   SDL_MapRGB(colorTemp->format,
                              ENTITY_BACKGROUND.r,
                              ENTITY_BACKGROUND.g,
                              ENTITY_BACKGROUND.b));
}

float Entity::getDrawPercent() const{
   return FULL; //default: full
}

Uint32 Entity::getColor() const{
   return DEFAULT_ENTITY_COLOR; //default
}