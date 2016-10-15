// (C) 2009-2010 Tim Gurto

#include <cstdlib> //rand
#include <cassert>

#include "misc.h"
#include "globals.h"
#include "SDL.h"
#include "Debug.h"
#include "Entity.h"
#include "EntityType.h"
#include "Point.h"
#include "GameData.h"

extern Debug debug;

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
   shadowBlit(&srcLoc, &drawLoc, screen);
}

void Entity::shadowBlit(SDL_Rect *srcLoc,
                        SDL_Rect *dstLoc,
                        SDL_Surface *screen) const{
   const EntityType &thisType = type();

   //blit mask, hiding anything that would otherwise
   //show through the gaps in the sprite
   if (ENTITY_MASKS)
      SDL_BlitSurface(thisType.mask, srcLoc, screen,
                      &SDL_Rect(*dstLoc)); 

   //shadow - white and black
   if (SHADOWS){
      colorBlit(ENTITY_SHADOW_LIGHT, thisType.surface, screen,
                *srcLoc, (*dstLoc + Point(1,1)));
      colorBlit(ENTITY_SHADOW_DARK, thisType.surface, screen,
                *srcLoc, (*dstLoc - Point(1,1)));
   }
   
   //colored sprite
   colorBlit(getColor(), thisType.surface, screen,
             *srcLoc, *dstLoc);

}

SDL_Rect Entity::getBaseRect(){
   return loc_ + type().baseRect_;   
}

void Entity::tick(double delta){} //default: do nothing

void Entity::init(GameData *game, SDL_Surface *screen){
   game_ = game;
   screen_ = screen;
   colorTemp = SDL_CreateRGBSurface(SDL_HWSURFACE,
                                    SCREEN_WIDTH,
                                    SCREEN_HEIGHT, SCREEN_BPP,
                                    0, 0, 0, 0);
   SDL_SetColorKey(colorTemp, SDL_SRCCOLORKEY,
                   SDL_MapRGB(colorTemp->format,
                              ENTITY_BACKGROUND.r,
                              ENTITY_BACKGROUND.g,
                              ENTITY_BACKGROUND.b));
}

double Entity::getDrawPercent() const{
   return FULL; //default: full
}

int Entity::getColor() const{
   return ENTITY_DEFAULT; //default
}

void Entity::colorBlit(int color, SDL_Surface *surface,
                       SDL_Surface *screen,
                       SDL_Rect &srcLoc,
                       SDL_Rect &dstLoc) const{

      assert(color < ENTITY_MAX);
      SDL_Surface *&index = game_->surfaceIndex
                                     [color]
                                     [type_]
                                     [classID()];

      //make sure colored sprite is indexed; if not,
      if (index == 0){

         //1. create it
         debug("Creating indexed surface");
         SDL_Rect drawRect = type().drawRect_;
         index = SDL_CreateRGBSurface(SDL_HWSURFACE,
                                      drawRect.w,
                                      drawRect.h,
                                      SCREEN_BPP,
                                      0, 0, 0, 0);
         SDL_SetColorKey(index, SDL_SRCCOLORKEY,
                         SDL_MapRGB(index->format,
                                    ENTITY_BACKGROUND.r,
                                    ENTITY_BACKGROUND.g,
                                    ENTITY_BACKGROUND.b));
         
         //2. fill with color
         SDL_FillRect(index, 0,
                      getEntityColor(*game_, color));

         //3. add sprite
         SDL_BlitSurface(surface, 0, index, 0);
      }

      //blit colored sprite to the screen.
      //The indexed version definitely exists now.
      SDL_BlitSurface(index, &srcLoc, screen,
                      &SDL_Rect(dstLoc));

}

bool Entity::collides() const{
   return true; //default: collides
}