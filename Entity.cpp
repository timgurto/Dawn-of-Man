// (C) 2009-2010 Tim Gurto

#include <cstdlib> //rand
#include <cassert>

#include "misc.h"
#include "types.h"
#include "globals.h"
#include "SDL.h"
#include "Debug.h"
#include "Entity.h"
#include "EntityType.h"
#include "Point.h"
#include "GameData.h"
#include "Unit.h"

extern Debug debug;

const progress_t Entity::PROGRESS_PER_CALC = 4;
GameData *Entity::game_ = 0;
SDL_Surface *Entity::screen_ = 0;
entities_t Entity::trashCan_;

Entity::Entity(typeNum_t typeIndex, const Point &loc):
typeIndex_(typeIndex),
loc_(loc),
direction_(Direction(rand() % DIR_MAX)),
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

void Entity::tick(double){} //default: do nothing

bool Entity::onScreen() const{
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
   return CLR_DEFAULT; //default
}

void Entity::colorBlit(int color, SDL_Surface *screen,
                       SDL_Rect &srcLoc,
                       SDL_Rect &dstLoc) const{
   assert(color < CLR_MAX);
   const EntityType &thisType = type();
   SDL_Surface *&index = game_->surfaceIndex
                                  [color]
                                  [typeIndex_]
                                  [classID()];

   //make sure colored sprite is indexed; if not,
   if (index == 0){

      //1. create it
      debug("Creating indexed surface");
      index = createSurface(thisType.surface_->w,
                            thisType.surface_->h);
      SDL_SetColorKey(index, SDL_SRCCOLORKEY,
                      SDL_MapRGB(index->format,
                                 ENTITY_BACKGROUND.r,
                                 ENTITY_BACKGROUND.g,
                                 ENTITY_BACKGROUND.b));
      
      //2. fill with color
      SDL_FillRect(index, 0,
                   getEntityColor(*game_, color));

      //3. add sprite
      SDL_BlitSurface(thisType.surface_, 0, index, 0);
   }

   SDL_Rect dest = dstLoc + Point(game_->map);

   //blit mask, hiding anything that would otherwise
   //show through the gaps in the sprite
   if (ENTITY_MASKS)
      SDL_BlitSurface(thisType.mask_, &srcLoc, screen,
                      &SDL_Rect(dest));
   //note: the "SDL_Rect(dest)" above avoids SDL_Blit
   //messing with the draw location

   //blit colored sprite to the screen.
   //The indexed version definitely exists now.
   SDL_BlitSurface(index, &srcLoc, screen, &dest);
}

void Entity::addParticles(int count) const{
   for (int i = 0; i != count; ++i){
      //calculate initial co-ords
      pixels_t x = 0, y = 0;
      double drawPercent = getDrawPercent();
      switch(direction_){
      case DIR_UP:
         x = loc_.x + 
             type().drawRect_.x +
             rand() % type().drawRect_.w;
         y = loc_.y + 
             type().drawRect_.y +
             pixels_t((1.0 - drawPercent) * type().drawRect_.h);
         break;
      case DIR_DOWN:
         x = loc_.x + 
             type().drawRect_.x +
             rand() % type().drawRect_.w;
         y = loc_.y + 
             type().drawRect_.y +
             pixels_t(drawPercent * type().drawRect_.h);
         break;
      case DIR_LEFT:
         x = loc_.x + 
             type().drawRect_.x +
             pixels_t((1.0 - drawPercent) * type().drawRect_.w);
         y = loc_.y + 
             type().drawRect_.y +
             rand() % type().drawRect_.h;
         break;
      case DIR_RIGHT:
         x = loc_.x + 
             type().drawRect_.x +
             pixels_t(drawPercent * type().drawRect_.w);
         y = loc_.y + 
             type().drawRect_.y +
             rand() % type().drawRect_.h;
      }

      //add
      game_->particles.push_back(Particle(x, y));
   }
}

SDL_Rect Entity::getSrcClip(pixels_t wPartial,
                            pixels_t hPartial,
                            int xMutiplier) const{
   SDL_Rect srcLoc;
   const EntityType &thisType = type();
   switch(direction_){
   case DIR_RIGHT:
      srcLoc = makeRect(xMutiplier * thisType.drawRect_.w,
                        0,
                        wPartial,
                        thisType.drawRect_.h);
      break;
   case DIR_DOWN:
      srcLoc = makeRect(xMutiplier * thisType.drawRect_.w,
                        0,
                        thisType.drawRect_.w,
                        hPartial);
      break;
   case DIR_LEFT:
      srcLoc = makeRect((xMutiplier + 1) *
                        thisType.drawRect_.w -
                        wPartial,
                        0,
                        wPartial,
                        thisType.drawRect_.h);
      //drawLoc.x += thisType.drawRect_.w - wPartial;
      break;
   case DIR_UP:
      srcLoc = makeRect(xMutiplier * thisType.drawRect_.w,
                        thisType.drawRect_.h - hPartial,
                        thisType.drawRect_.w,
                        hPartial);
      //drawLoc.y += thisType.drawRect_.h - hPartial;
   }
   return srcLoc;
}

bool Entity::collides() const{
   return true;
}

bool Entity::selectable() const{
   return false;
}

bool Entity::targetable() const{
   return false;
}

typeNum_t Entity::getPlayer() const{
   return NO_TYPE;
}

SDL_Rect Entity::getSelectionDest(SDL_Surface *selection) const{
   assert (selection != 0);
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

void Entity::kill(){
   //TODO make death more spectacular
   trashCan_.push_back(this);
   if ((Entity *)(game_->buildingSelected) == this){
      game_->buildingSelected = 0;
      game_->mode = MODE_NORMAL;
   }

   entities_t::iterator itToErase;
   for (entities_t::iterator it = game_->entities.begin();
        it != game_->entities.end(); ++it){
      if (*it == this)
         itToErase = it;
      else if ((*it)->classID() == ENT_UNIT){
         Unit &unit = (Unit &)(**it);
         if (unit.targetEntity_ == this)
            unit.setTarget(0, (*it)->loc_);
      }
   }
   game_->entities.erase(itToErase);
}

void Entity::emptyTrash(){
   if (!trashCan_.empty()){
      for (entities_t::iterator it = trashCan_.begin();
           it != trashCan_.end(); ++it)
         delete (*it);
      trashCan_.clear();
   }
}

typeNum_t Entity::getTypeIndex() const{
   return typeIndex_;
}

const Point &Entity::getLoc() const{
   return loc_;
}