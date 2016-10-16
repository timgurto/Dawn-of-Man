// (C) 2009, 2010 Tim Gurto

#ifndef MISC_H
#define MISC_H

#include <fstream>
#include <string>
#include <queue>
#include <SDL.h>

#include "types.h"
#include "globals.h"

class Entity;
class EntityType;
class Unit;
struct Point;
struct GameData;

//Constructs a file path to load graphics for entities
std::string makePath(EntityTypeID type, typeNum_t imageNumber,
                     ImageModifier modifier = IMG_NONE);

//Dereferences Entity pointers and performs <
//(used in sorting the Entities list)
bool dereferenceLessThan(Entity *p1, Entity *p2);

//Checks for collisions against every entity in the game:
//a potential entity, of given type and location
//ignore: an entity to skip checking, i.e. the current unit
bool noCollision(const GameData &game, const EntityType &type,
                 const Point &loc, const Entity *ignore1 = 0,
                 const Entity *ignore2 = 0,
                 bool excludeMovingUnits = false);

//checks a specific rectangle, rather than an entity
bool noCollision(const GameData &game, const SDL_Rect &rect,
                 const Entity *ignore1 = 0,
                 const Entity *ignore2 = 0,
                 bool excludeMovingUnits = false);

//looks up the relevant color based on index input
Uint32 getEntityColor(const GameData &game, int color);

//double % int, preserves fraction
double modulo(double a, int b);

//Centers the map on the provided co-ordinates.
//Map edges are taken into account automatically
void centerMap(GameData &game, const Point &center);

//Adds a leading space if x < 100
std::string format3(double x);

//Adds a leading zero if x < 10
std::string format2(int x);

//Parse the next token from a data file
std::string parseToken(std::ifstream &data);

//if an index is invalid, set it to NO_TYPE so that nothing breaks
void checkTypeIndex(typeNum_t &i, size_t max);

//empties a std::queue
template<typename T>
void emptyQueue(std::queue<T> &q){
   while (!q.empty())
      q.pop();
}

//push a mouse-move event onto the queue, to refresh the
//calculations handled there
void pushMouseMove();

//creates a rectangle representing the unit's path between
//the two points specified
SDL_Rect makePathRect(const EntityType &type,
                      const Point &p1, const Point &p2);

#endif