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
SDL_Surface *Entity::diagGreen_ = 0;
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

//unnamed arg to suppress 'unused' warning
void Entity::tick(double){} //default: do nothing

bool Entity::onScreen() const{
   return collision(loc_ + type().drawRect_ + locRect(game_->map),
                    screen_->clip_rect);
}

void Entity::init(GameData *game, SDL_Surface *screen,
                  SDL_Surface *diagGreen){
   game_ = game;
   screen_ = screen;
   diagGreen_ = diagGreen;
}

double Entity::getDrawPercent() const{
   return FULL; //default: full
}

int Entity::getColor() const{
   return CLR_DEFAULT; //default
}

void Entity::colorBlit(int color, SDL_Surface *screen,
                       SDL_Rect &srcLoc,
                       SDL_Rect &dstLoc,
                       bool partial) const{

   assert(color < CLR_MAX);
   const EntityType &thisType = type();
   EntityTypeID thisClass = classID();
   SDL_Surface
      //plain color
      *&index         = game_->surfaceIndex
                                       [color]
                                       [typeIndex_]
                                       [thisClass],
      //shadows: dark and light parts
      *&indexDark     = game_->surfaceIndex
                                       [CLR_SHADOW_DARK]
                                       [typeIndex_]
                                       [thisClass],
      *&indexLight    = game_->surfaceIndex
                                       [CLR_SHADOW_LIGHT]
                                       [typeIndex_]
                                       [thisClass],
      //composite shadows + color
      *&indexShadowed = game_->surfaceIndexShadowed
                                       [color]
                                       [typeIndex_]
                                       [thisClass],
      //black, for shading
      *&indexBlack    = game_->surfaceIndexShadowed
                                       [CLR_BLACK]
                                       [typeIndex_]
                                       [thisClass];

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

   //make sure shadow-colored sprites are indexed,
   //as for colored sprite above.
   if (indexDark == 0){
      indexDark = createSurface(thisType.surface_->w,
                                thisType.surface_->h);
      SDL_SetColorKey(indexDark, SDL_SRCCOLORKEY,
                      SDL_MapRGB(indexDark->format,
                                 ENTITY_BACKGROUND.r,
                                 ENTITY_BACKGROUND.g,
                                 ENTITY_BACKGROUND.b));
      SDL_FillRect(indexDark, 0, ENGRAVE_DARK_UINT);
      SDL_BlitSurface(thisType.surface_, 0, indexDark, 0);
   }
   if (indexLight == 0){
      indexLight = createSurface(thisType.surface_->w,
                                thisType.surface_->h);
      SDL_SetColorKey(indexLight, SDL_SRCCOLORKEY,
                      SDL_MapRGB(indexLight->format,
                                 ENTITY_BACKGROUND.r,
                                 ENTITY_BACKGROUND.g,
                                 ENTITY_BACKGROUND.b));
      SDL_FillRect(indexLight, 0, ENGRAVE_LIGHT_UINT);
      SDL_BlitSurface(thisType.surface_, 0, indexLight, 0);
   }
   if (indexShadowed == 0){
      //Create colored, shadowed surface, as above
      indexShadowed = createSurface(thisType.surface_->w + 2,
                                    thisType.surface_->h + 2);
      SDL_SetColorKey(indexShadowed, SDL_SRCCOLORKEY,
                      SDL_MapRGB(indexShadowed->format,
                                 ENTITY_BACKGROUND.r,
                                 ENTITY_BACKGROUND.g,
                                 ENTITY_BACKGROUND.b));
      SDL_FillRect(indexShadowed, 0, ENTITY_BACKGROUND_UINT);
      //Draw shadows, and sprite
      SDL_BlitSurface(indexLight, 0, indexShadowed, &makeRect(2, 2));
      SDL_BlitSurface(indexDark, 0, indexShadowed, &makeRect(0, 0));
      SDL_BlitSurface(index, 0, indexShadowed, &makeRect(1, 1));
   }
   if (drawBlack() && indexBlack == 0){
      indexBlack = createSurface(thisType.surface_->w,
                                 thisType.surface_->h);
      SDL_SetColorKey(indexBlack, SDL_SRCCOLORKEY,
                      SDL_MapRGB(indexBlack->format,
                                 ENTITY_BACKGROUND.r,
                                 ENTITY_BACKGROUND.g,
                                 ENTITY_BACKGROUND.b));
      SDL_FillRect(indexBlack, 0, BLACK_UINT);
      SDL_BlitSurface(thisType.surface_, 0, indexBlack, 0);
      if (BLACK_ENTITY_ALPHA < 0xff)
         SDL_SetAlpha(indexBlack, SDL_SRCALPHA, BLACK_ENTITY_ALPHA);
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
   //If incomplete, blit individual pieces for better shadow
   if (partial){
      SDL_Rect baseDest = loc_ +
                          Point(game_->map) +
                          locRect(getBaseRect());
      SDL_BlitSurface(diagGreen_, &dimRect(getBaseRect()),
                      screen, &baseDest);
      SDL_BlitSurface(indexLight, &srcLoc,
                      screen, &SDL_Rect(dest + Point(1, 1)));
      SDL_BlitSurface(indexDark, &srcLoc,
                      screen, &SDL_Rect(dest - Point(1, 1)));
      SDL_BlitSurface(index, &srcLoc,
                      screen, &dest);
      }
   else
      SDL_BlitSurface(indexShadowed, &srcLoc,
                      screen, &SDL_Rect(dest - Point(1, 1)));
   if (drawBlack())
      SDL_BlitSurface(indexBlack, &srcLoc,
                      screen, &dest);
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

bool Entity::drawBlack() const{
   return false;
}