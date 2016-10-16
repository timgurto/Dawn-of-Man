// (C) 2009-2010 Tim Gurto

#ifndef ENTITY_TYPE_H
#define ENTITY_TYPE_H

#include <string>
#include "SDL.h"
#include "Point.h"
#include "Surface.h"
#include "globals.h"

//A category of entities, differing by attributes,
//regular actions, and in-game behavior
class EntityType{
   friend class Entity;
   friend class Building;
   friend class Unit;
   friend class ResourceNode;
   friend struct CoreData;

   //for loading graphics, and position in vector
   typeNum_t index_;

   std::string name_; //the entity's name

   SDL_Rect
      drawRect_, //position of sprite graphic
      baseRect_; //the entity's base, for collisions

   //where the selection indicator is centered
   Point selectionCenter_;

   //Which decoration this entity becomes when it dies
   typeNum_t decorationAtDeath_;

   Surface
      surface_, //main image
      icon_, //icon, for UIBars
      mask_; //green and magenta background mask

   //these are for copying, since it's a messier
   //process than for surfaces, and thus easier
   //to just load again
   std::string
      soundFile_,
      deathSoundFile_,
      hitSoundFile_;

   SDL_Sound
      *sound_, //standard entity sound
      *hitSound_, //sound when attacking or gathering
      *deathSound_; //sound made upon death

public:
   //deep-copy, loading and allocating new surfaces
   EntityType(typeNum_t index, EntityTypeID type,
              const std::string &name,
              const SDL_Rect &drawRect,
              const SDL_Rect &baseRect,
              const Point &selectionCenter = Point(),
              typeNum_t decorationAtDeath = NO_TYPE,
              const std::string &soundFile = "",
              const std::string &deathSoundFile = "",
              const std::string &hitSoundFile = "");
   EntityType(const EntityType &original);

   ~EntityType();

   //get
   const SDL_Rect &getBaseRect() const;
   const SDL_Rect &getDrawRect() const;
   const Surface &getIcon() const;
   const std::string &getName() const;
   typeNum_t getIndex() const;
   SDL_Sound *getSound() const;
   SDL_Sound *getHitSound() const;
   typeNum_t getDecorationAtDeath() const;
};

#endif