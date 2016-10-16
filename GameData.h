// (C) 2009-2010 Tim Gurto

#ifndef GAME_DATA_H
#define GAME_DATA_H

#include <vector>
#include <list>

#include "types.h"
#include "BuildingType.h"
#include "DecorationType.h"
#include "UnitType.h"
#include "ResourceNodeType.h"
#include "Entity.h"
#include "Particle.h"
#include "Player.h"
#include "Point.h"
#include "MouseButton.h"
#include "Tech.h"

//move to relevant classes
typedef std::vector<BuildingType> buildingTypes_t;
typedef std::vector<DecorationType> decorationTypes_t;
typedef std::vector<UnitType> unitTypes_t;
typedef std::vector<ResourceNodeType> resourceNodeTypes_t;
typedef std::vector<Tech> techs_t;
typedef std::vector<Player> players_t;
typedef std::list<Particle> particles_t;
typedef SDL_Surface *surfaceIndex_t
                        //number of colors
        //[0] - [MAX_PLAYERS-1] = each player's color
                        [CLR_MAX]

                        //number of types
                        [MAX_TYPES]

                        //number of entity subclasses
                        [ENT_MAX];

//Contains all data necessary to describe the game's
//state.  Makes passing this information to classes
//and functions much simpler.
struct GameData{

   GameData(int mapSizeX, int mapSizeY);

   //free entities and surfaces pointed to
   ~GameData();

   void init(char *filename);

   //Entity types
   //vectors:
   // - contiguous for index access
   buildingTypes_t buildingTypes;
   decorationTypes_t decorationTypes;
   unitTypes_t unitTypes;
   resourceNodeTypes_t resourceNodeTypes;
   techs_t techs;
   
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

   //An index of already-used, colored entity surfaces,
   //with and without shadows.
   surfaceIndex_t surfaceIndex;
   surfaceIndex_t surfaceIndexShadowed;

   //The current game mode
   ControlMode mode;

   //Whether the game loop is continuing
   bool loop;

   //The position of the mouse pointer
   Point mousePos;

   //if in build mode, the selected building type
   typeNum_t toBuild;

   //dimensions of the map, in # of tiles
   int mapX, mapY;

   //the size and displacement of the map, on which
   //everything happens.
   //x, y = scroll displacement
   //w, h = map size
   SDL_Rect map;

   //Information about mouse button positions and
   //dragging
   MouseButton leftMouse, rightMouse;

   //Whether the game is paused
   bool paused;

   //Whether the location being hovered over is viable
   //for construction
   bool buildLocationOK;

   //A pointer to the frontmost building that is
   //selected, or 0 if none is.
   //Used for UIBar display, and unit training
   Building *buildingSelected;
};

#endif