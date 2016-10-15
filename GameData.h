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
typedef SDL_Surface *surfaceIndex_t
                        [ENTITY_MAX] //number of colors
                        [MAX_TYPES] //number of types
                        //number of entity subclasses
                        [MAX_ENTITY_TYPE];

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

   //An index of already-usedk, colored entity surfaces.
   //[0] - [MAX_PLAYERS-1] = each player's color
   //see enum EntityColor and getEntityColor() for the rest
   surfaceIndex_t surfaceIndex;

   //The current game mode
   ControlMode mode;

   GameData(){
      for (typeNum_t i = 0; i != ENTITY_MAX; ++i)
         for (typeNum_t j = 0; j != MAX_TYPES; ++j)
            for (typeNum_t k = 0; k != MAX_ENTITY_TYPE; ++k)
               surfaceIndex[i][j][k] = 0;
   }

   //free entities and surfaces pointed to
   ~GameData(){

      //entities (delete)
      for (entities_t::iterator it = entities.begin();
           it != entities.end(); ++it){
         delete(*it);
      }

      //surface index (SDL_FreeSurface)
      for (typeNum_t i = 0; i != ENTITY_MAX; ++i)
         for (typeNum_t j = 0; j != MAX_TYPES; ++j)
            for (typeNum_t k = 0; k != MAX_ENTITY_TYPE; ++k)
               SDL_FreeSurface(surfaceIndex[i][j][k]);
   }
};

#endif