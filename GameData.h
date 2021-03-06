// (C) 2009, 2010 Tim Gurto

#ifndef GAME_DATA_H
#define GAME_DATA_H

#include <vector>
#include <list>

#include "types.h"
#include "Entity.h"
#include "Particle.h"
#include "Player.h"
#include "Point.h"
#include "MouseButton.h"
#include "Surface.h"

class Building;
struct CoreData;

typedef std::vector<Player> players_t;
typedef std::list<Particle> particles_t;
typedef Surface surfaceIndex_t
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

   //the color the cursor should be painted
   int cursorColor;

   //The current game mode
   ControlMode mode;

   //Whether the game loop is continuing
   bool loop;

   //if in build mode, the selected building type
   typeNum_t toBuild;

   //dimensions of the map, in # of tiles
   int mapX, mapY;

   //the size and displacement of the map, on which
   //everything happens.
   //x, y = scroll displacement
   //w, h = map size
   SDL_Rect map;

   //whether scrolling in each dimension is disabled
   bool scrollLockX, scrollLockY;

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

   //whether a change has occurred that warrants updating
   //a UI bar
   bool recalcBars;

   //whether there has been a change in what is selected,
   //warranting updates in the game mode and UI bar displays
   bool selectionChanged;

   //The outcome/state of the game
   GameOutcome outcome;

private:
   //core data
   const static CoreData *core_;

   //no assignment or copying allowed
   GameData(const GameData &original){}
   GameData &operator=(const GameData &rhs){return *this;}
   
public:

   GameData(std::string filename);

   GameData(int mapSizeX, int mapSizeY);

   //free entities and surfaces pointed to
   ~GameData();

   static void init(const CoreData *core);

   bool trainUnit(typeNum_t index,
                  const Building &sourceBuilding,
                  typeNum_t playerID);

   bool researchTech(typeNum_t index, typeNum_t playerID);

   bool constructBuilding(typeNum_t index, const Point &loc,
                          typeNum_t playerID);

   //whether the given item is available for the player
   bool validBuilding(typeNum_t playerID, typeNum_t i) const;
   bool validUnit(typeNum_t playerID, typeNum_t i, typeNum_t buildingType) const;
   bool validTech(typeNum_t playerID, typeNum_t i, typeNum_t buildingType) const;
   
   //Adds an entity to the list, signing it up for
   //garbage collection upon removal
   //Entity must have been allocated with new
   void addEntity(Entity *entity);
};

#endif