// (C) 2009-2010 Tim Gurto

#include <cstdlib> //rand
#include <cassert>

#include "misc.h"
#include "types.h"
#include "update.h"
#include "globals.h"
#include "Debug.h"
#include "Entity.h"
#include "EntityType.h"
#include "Point.h"
#include "GameData.h"
#include "Unit.h"
#include "ResourceNode.h"
#include "CoreData.h"
#include "Decoration.h"
#include "Surface.h"

extern Debug debug;

const progress_t Entity::PROGRESS_PER_CALC = 4;
GameData *Entity::game_ = 0;
const CoreData *Entity::core_ = 0;
Surface *Entity::diagGreen_ = 0;
entities_t Entity::trashCan_;

Entity::Entity(typeNum_t typeIndex, const Point &loc):
typeIndex_(typeIndex),
loc_(loc),
direction_(Direction(rand() % DIR_MAX)),
selected(false),
verticalMovement_(VM_NONE){}

//less than; compares y co-ordinates
bool Entity::operator<(const Entity &rhs) const{
   return (loc_.y < rhs.loc_.y);
}

void Entity::draw(Surface &screen) const{
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
void Entity::tick(double /*delta*/){} //default: do nothing

bool Entity::onScreen() const{
   return collision(loc_ + type().drawRect_ + locRect(game_->map),
                    (screenBuf)->clip_rect);
}

void Entity::init(const CoreData *core, GameData *game,
                  Surface *diagGreen){
   core_ = core;
   game_ = game;
   diagGreen_ = diagGreen;
}

double Entity::getDrawPercent() const{
   return FULL; //default: full
}

int Entity::getColor() const{
   return CLR_DEFAULT; //default
}

void Entity::colorBlit(int color, Surface &screen,
                       SDL_Rect &srcLoc,
                       SDL_Rect &dstLoc,
                       bool partial) const{

   assert(color < CLR_MAX);
   const EntityType &thisType = type();
   EntityTypeID thisClass = classID();
   Surface
      //plain color
      &index         = game_->surfaceIndex
                                       [color]
                                       [typeIndex_]
                                       [thisClass],
      //shadows: dark and light parts
      &indexDark     = game_->surfaceIndex
                                       [CLR_SHADOW_DARK]
                                       [typeIndex_]
                                       [thisClass],
      &indexLight    = game_->surfaceIndex
                                       [CLR_SHADOW_LIGHT]
                                       [typeIndex_]
                                       [thisClass],
      //composite shadows + color
      &indexShadowed = game_->surfaceIndexShadowed
                                       [color]
                                       [typeIndex_]
                                       [thisClass],
      //black, for shading
      &indexBlack    = game_->surfaceIndexShadowed
                                       [CLR_BLACK]
                                       [typeIndex_]
                                       [thisClass];

   //make sure colored sprite is indexed; if not,
   if (!index){

      //1. create it
      //debug("Creating indexed surface");
      index = Surface(SUR_BLANK,
                      thisType.surface_->w,
                      thisType.surface_->h,
                      ENTITY_BACKGROUND);
      
      //2. fill with color
      index.fill(getEntityColor(*game_, color));

      //3. add sprite
      index << thisType.surface_;
   }

   //make sure shadow-colored sprites are indexed,
   //as for colored sprite above.
   if (!indexDark){
      indexDark = Surface(SUR_BLANK,
                          thisType.surface_->w,
                          thisType.surface_->h,
                          ENTITY_BACKGROUND);
      indexDark.fill(ENGRAVE_DARK);
      indexDark << thisType.surface_;
   }
   if (!indexLight){
      indexLight = Surface(SUR_BLANK,
                           thisType.surface_->w,
                           thisType.surface_->h,
                           ENTITY_BACKGROUND);
      indexLight.fill(ENGRAVE_LIGHT);
      indexLight << thisType.surface_;
   }
   if (!indexShadowed){
      //Create colored, shadowed surface, as above
      indexShadowed = Surface(SUR_BLANK,
                              thisType.surface_->w + 2,
                              thisType.surface_->h + 2,
                              ENTITY_BACKGROUND);
      indexShadowed.fill(ENTITY_BACKGROUND);

      //Draw shadows, and sprite
      indexLight.draw(indexShadowed, &makeRect(2, 2));
      indexDark .draw(indexShadowed, &makeRect(0, 0));
      index     .draw(indexShadowed, &makeRect(1, 1));
   }
   if (drawBlack() && !indexBlack){
      indexBlack = Surface(SUR_BLANK,
                           thisType.surface_->w + 2,
                           thisType.surface_->h + 2,
                           ENTITY_BACKGROUND);
      indexBlack.fill(BLACK);
      indexBlack << thisType.surface_;
      if (BLACK_ENTITY_ALPHA < 0xff)
         indexBlack.setAlpha(BLACK_ENTITY_ALPHA);
   }

   SDL_Rect dest = dstLoc + Point(game_->map);

   //blit mask, hiding anything that would otherwise
   //show through the gaps in the sprite
   if (ENTITY_MASKS && classID() != ENT_DECORATION)
      thisType.mask_.draw(screen, &SDL_Rect(dest), &srcLoc);
   //note: the "SDL_Rect(dest)" above avoids SDL_Blit
   //messing with the draw location

   //blit colored sprite to the screen.
   //The indexed version definitely exists now.
   //If incomplete, blit individual pieces for better shadow
   if (partial){
      indexLight.draw(screen, &SDL_Rect(dest + Point(1, 1)), &srcLoc);
      indexDark .draw(screen, &SDL_Rect(dest - Point(1, 1)), &srcLoc);
      index     .draw(screen, &dest, &srcLoc);
   }else
      indexShadowed.draw(screen, &SDL_Rect(dest - Point(1, 1)), &srcLoc);

   if (drawBlack())
      indexBlack.draw(screen, &dest, &srcLoc);
}

void Entity::addParticles(int count) const{
   for (int i = 0; i != count; ++i){
      double drawPercent = getDrawPercent();
      pixels_t x = 0, y = 0;
      SDL_Rect drawRect = type().drawRect_;

      //avoid div/0
      if (drawRect.w == 0)
         drawRect.w = 1;
      if (drawRect.h == 0)
         drawRect.h = 1;

      assert (drawRect.w != 0);
      assert (drawRect.h != 0);

      //calculate initial co-ords
      switch(direction_){
      case DIR_UP:
         x = loc_.x + 
             drawRect.x +
             rand() % drawRect.w;
         y = loc_.y + 
             drawRect.y +
             pixels_t((1.0 - drawPercent) * drawRect.h);
         break;
      case DIR_DOWN:
         x = loc_.x + 
             drawRect.x +
             rand() % drawRect.w;
         y = loc_.y + 
             drawRect.y +
             pixels_t(drawPercent * drawRect.h);
         break;
      case DIR_LEFT:
         x = loc_.x + 
             drawRect.x +
             pixels_t((1.0 - drawPercent) * drawRect.w);
         y = loc_.y + 
             drawRect.y +
             rand() % drawRect.h;
         break;
      case DIR_RIGHT:
         x = loc_.x + 
             drawRect.x +
             pixels_t(drawPercent * drawRect.w);
         y = loc_.y + 
             drawRect.y +
             rand() % drawRect.h;
      }

      //add
      game_->particles.push_back(Particle(x, y));
   }
}

SDL_Rect Entity::getSrcClip(pixels_t wPartial,
                            pixels_t hPartial,
                            int xMultiplier) const{
   SDL_Rect srcLoc;
   const EntityType &thisType = type();
   switch(direction_){
   case DIR_RIGHT:
      srcLoc = makeRect(xMultiplier * thisType.drawRect_.w,
                        0,
                        wPartial,
                        thisType.drawRect_.h);
      break;
   case DIR_DOWN:
      srcLoc = makeRect(xMultiplier * thisType.drawRect_.w,
                        0,
                        thisType.drawRect_.w,
                        hPartial);
      break;
   case DIR_LEFT:
      srcLoc = makeRect((xMultiplier + 1) *
                        thisType.drawRect_.w -
                        wPartial,
                        0,
                        wPartial,
                        thisType.drawRect_.h);
      //drawLoc.x += thisType.drawRect_.w - wPartial;
      break;
   case DIR_UP:
      srcLoc = makeRect(xMultiplier * thisType.drawRect_.w,
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

SDL_Rect Entity::getSelectionDest(Surface &selection) const{
   assert (selection);
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
   game_->selectionChanged = true;
   const EntityType &thisType = type();
   playSound(thisType.deathSound_);
   //resource this turns into
   typeNum_t resourceType;
   ResourceNode *node = 0;

   //turns into decoration?
   typeNum_t decorationType = type().getDecorationAtDeath();
   if (decorationType != NO_TYPE){
      Decoration *decoration = new Decoration(decorationType, loc_);
      addEntity(*game_, decoration);
   }

   //unit that turns into resource?
   else if (this->classID() == ENT_UNIT){
      resourceType =
         core_->unitTypes[typeIndex_].getDeathResource();
      if (resourceType != NO_TYPE){
         node = new ResourceNode(resourceType, loc_);
         addEntity(*game_, node);
      }
   }

   //fix units that were targetting this
   for (entities_t::iterator it = game_->entities.begin();
        it != game_->entities.end(); ++it)
      if ((*it)->classID() == ENT_UNIT){
         Unit &unit = (Unit &)(**it);
         if (unit.targetEntity_ == this){
            if (node && unit.isGatherer())
               unit.targetEntity_ = node;
            else
               unit.setTarget(0, unit.loc_);
         }
      }

   trashCan_.push_back(this);
   entities_t::iterator itToErase;
   for (entities_t::iterator it = game_->entities.begin();
        it != game_->entities.end(); ++it){
      if (*it == this)
         itToErase = it;
   }
   game_->entities.erase(itToErase);
   checkVictory(*game_);
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

std::string Entity::getHelp() const{
   return ""; //default is blank, for decorations
}