// (C) 2009 Tim Gurto

#ifndef GAME_DATA_H
#define GAME_DATA_H

#include <vector>
#include <list>

#include "globals.h"
#include "BuildingType.h"
#include "DecorationType.h"
#include "Entity.h"
#include "Particle.h"

//type containers need to be contiguous for index access
typedef std::vector<BuildingType> buildingTypes_t;
typedef std::vector<DecorationType> decorationTypes_t;

typedef std::list<Entity *> entities_t;
typedef std::list<Particle> particles_t;

//Contains all data necessary to describe the game's
//state.  Makes passing this information to classes
//and functions much simpler.
struct GameData{
   
   //Entity types
   buildingTypes_t buildingTypes;
   decorationTypes_t decorationTypes;
   
   //All entities in the game, sorted by y co-ordinate.
   //Entity* used, for polymorphism
   entities_t entities;

   //The current game mode
   ControlMode mode;

   //All particles in the game
   particles_t particles;

   ~GameData(){
      for (entities_t::iterator it = entities.begin();
           it != entities.end(); ++it){
         delete(*it);
      }
   }
};

#endif