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

Entity::Entity(typeNum_t type, const Point &loc):
type_(type),
loc_(loc),
direction(Direction(rand() % 4)),
selected(false),
verticalMovement_(VM_NONE){}

bool Entity::operator<(const Entity &rhs) const{
   return (loc_.y < rhs.loc_.y);
}

void Entity::draw(SDL_Surface *screen) const{
   const EntityType &thisType = type();
   SDL_Rect drawLoc = getDrawRect();
   SDL_Rect srcLoc = makeRect(0, 0,
                              thisType.drawRect_.w,
                              thisType.drawRect_.h);
   colorBlit(getColor(), screen, srcLoc, drawLoc);
}

SDL_Rect Entity::getBaseRect() const{
   return loc_ + type().baseRect_;   
}

SDL_Rect Entity::getDrawRect() const{
   return loc_ + type().drawRect_;   
}

void Entity::tick(double delta){} //default: do nothing

bool Entity::onScreen(){
   return collision(loc_ + type().drawRect_ + locRect(game_->map),
                    screen_->clip_rect);
}

void Entity::init(GameData *game, SDL_Surface *screen){
   game_ = game;
   screen_ = screen;
}

double Entity::getDrawPercent() const{
   return FULL; //default: full
}

int Entity::getColor() const{
   return ENTITY_DEFAULT; //default
}

void Entity::colorBlit(int color, SDL_Surface *screen,
                       SDL_Rect &srcLoc,
                       SDL_Rect &dstLoc) const{
   assert(color < ENTITY_MAX);
   const EntityType &thisType = type();
   SDL_Surface *&index = game_->surfaceIndex
                                  [color]
                                  [type_]
                                  [classID()];

   //make sure colored sprite is indexed; if not,
   if (index == 0){

      //1. create it
      debug("Creating indexed surface");
      SDL_Rect drawRect = type().drawRect_;
      index = createSurface(thisType.surface->w,
                            thisType.surface->h);
      SDL_SetColorKey(index, SDL_SRCCOLORKEY,
                      SDL_MapRGB(index->format,
                                 ENTITY_BACKGROUND.r,
                                 ENTITY_BACKGROUND.g,
                                 ENTITY_BACKGROUND.b));
      
      //2. fill with color
      SDL_FillRect(index, 0,
                   getEntityColor(*game_, color));

      //3. add sprite
      SDL_BlitSurface(thisType.surface, 0, index, 0);
   }

   SDL_Rect dest = dstLoc + Point(game_->map);

   //blit mask, hiding anything that would otherwise
   //show through the gaps in the sprite
   if (ENTITY_MASKS)
      SDL_BlitSurface(thisType.mask, &srcLoc, screen,
                      &SDL_Rect(dest));
   //note: the "SDL_Rect(dest)" above avoids SDL_Blit
   //messing with the draw location

   //blit colored sprite to the screen.
   //The indexed version definitely exists now.
   SDL_BlitSurface(index, &srcLoc, screen, &dest);
}

bool Entity::collides() const{
   return true;
}

bool Entity::selectable() const{
   return false;
}

SDL_Rect Entity::getSelectionDest(SDL_Surface *selection) const{
   const EntityType &t = type();
   SDL_Rect r;
   r.x = loc_.x +
         game_->map.x +
         t.selectionCenter_.x -
         selection->clip_rect.w / 2;
   r.y = loc_.y +
         game_->map.y +
         t.selectionCenter_.y -
         selection->clip_rect.h / 2;
   return r;
}

VerticalMovement Entity::getVerticalMovement() const{
   return verticalMovement_;
}

bool Entity::isLocationOK() const{
   SDL_Rect rect = getBaseRect();
   
   //map
   if (!inside(rect, dimRect(game_->map)))
      return false;

   //entities
   for (entities_t::const_iterator it = game_->entities.begin();
        it != game_->entities.end(); ++it){
      
      //skip this
      if (*it == this)
         continue;

      if ((*it)->collides() &&
          collision(rect, (*it)->getBaseRect()))
         return false;
   }

   return true;
}