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
#include "Player.h"

typedef std::vector<BuildingType> buildingTypes_t;
typedef std::vector<DecorationType> decorationTypes_t;
typedef std::vector<Player> players_t;
typedef std::list<Entity *> entities_t;
typedef std::list<Particle> particles_t;

//Contains all data necessary to describe the game's
//state.  Makes passing this information to classes
//and functions much simpler.
struct GameData{
   
   //Entity types
   //vectors:
   // - contiguous for index access
   buildingTypes_t buildingTypes;
   decorationTypes_t decorationTypes;
   
   //Each player in the game.
   //[0] is always the human player.
   //vector:
   // - contiguous for index access
   players_t players;

   //All entities in the game, sorted by y co-ordinate.
   //Entity* used, for polymorphism
   //list:
   // - iterated through sequentially
   // - sorted; random swaps and insertions
   // - random deletions
   entities_t entities;

   //All particles in the game
   //list:
   // - iterated through sequentially
   // - random deletions
   particles_t particles;

   //The current game mode
   ControlMode mode;

   //free entities pointed to
   ~GameData(){
      for (entities_t::iterator it = entities.begin();
           it != entities.end(); ++it){
         delete(*it);
      }
   }
};

#endif