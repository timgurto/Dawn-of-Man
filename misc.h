// (C) 2009, 2010 Tim Gurto

#ifndef MISC_H
#define MISC_H

#include <fstream>
#include <string>
#include <queue>

#include "SDL_mixer.h"
#include "types.h"
#include "globals.h"

class Entity;
class EntityType;
class Unit;
struct Point;
struct GameData;

const Uint8 MOUSE_BUTTON_LEFT        = 1;
const Uint8 MOUSE_BUTTON_MIDDLE      = 2;
const Uint8 MOUSE_BUTTON_RIGHT       = 3;
const Uint8 MOUSE_BUTTON_SCROLL_UP   = 4;
const Uint8 MOUSE_BUTTON_SCROLL_DOWN = 5;

//===SDL===

//Loads a sound, and registers it with soundsLoaded
SDL_Sound *loadSound(const std::string &fileName);

//Fake constructor and copy constructor for SDL_Rect
SDL_Rect makeRect(Sint16 x = 0, Sint16 y = 0, Uint16 w = 0, Uint16 h = 0);
SDL_Rect makeRect(const Point &point);

//Fake constructor for SDL_Color
SDL_Color makeColor(Uint8 r, Uint8 g, Uint8 b);
SDL_Color makeColor(Uint32 c);

//equality of colors
bool operator==(const SDL_Color &lhs, const SDL_Color &rhs);

//SDL_Color -> Uint32
Uint32 colorToUInt(const SDL_Color &color);

//SDL_Color / int
SDL_Color operator/(const SDL_Color &lhs, int rhs);

//Free a sound, and update soundsLoaded
void freeSound(SDL_Sound *&p);

//Deep-copies a sound from one pointer to another.
SDL_Sound *copySound(SDL_Sound* src);

//returns a rectangle with equal dimensions, but blank co-ordinates
SDL_Rect dimRect(const SDL_Rect &original);

//returns a rectangle with equal co-ordinates, but blank dimensions
SDL_Rect locRect(const SDL_Rect &original);

//Whether the specified key is currently down
bool isKeyPressed(SDLKey key);

//SDL_Rect -= SDL_Rect
SDL_Rect &operator-=(SDL_Rect &lhs, const SDL_Rect &rhs);

//SDL_Color != SDL_Color
bool operator!=(const SDL_Color &lhs, const SDL_Color &rhs);

//plays a sound
void playSound(SDL_Sound *p);

//push a mouse-move event onto the queue, to refresh the
//calculations handled there
void pushMouseMove(const GameData &game);

//===misc===

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
                const Entity *ignore2 = 0);

//Checks for a collision between two SDL_Rects
bool collision(const SDL_Rect &r1, const SDL_Rect &r2);

//Whether a Point lies in an SDL_Rect
bool collision(const Point &point, const SDL_Rect &rect);
bool collision(const SDL_Rect &rect, const Point &point);

//looks up the relevant color based on index input
Uint32 getEntityColor(const GameData &game, int color);

//whether rect A is completely inside rect B
bool inside(const SDL_Rect &a, const SDL_Rect &b);

//Whether an argument exists
bool isArg(std::string arg, int argc, char* argv[]);

//The numeric value associated with an argument
// arg=???
int whatIsArg(std::string arg, int argc, char* argv[]);

//The one-dimensional distance between two points
pixels_t distance(pixels_t a, pixels_t b);

//The two-dimensional distance between two points
pixels_t distance(Point a, Point b);

//double % int, preserves fraction
double modulo(double a, int b);

//Centers the map on the provided co-ordinates.
//Map edges are taken into account automatically
void centerMap(GameData &game, const Point &center);

//squares a number
template<typename T>
//param is copied; eval'd once
T square(T x){
   return x * x;
}

//returns the minimum of two values
template<typename T>
T min(T a, T b){
   return a < b ? a : b;
}

//returns the maximum of two values
template<typename T>
T max(T a, T b){
   return a < b ? b : a;
}

//Adds a leading zero if x < 100
std::string format3(double x);

//Adds a leading zero if x < 10
std::string format2(int x);

//Parse the next token from a data file
std::string parseToken(std::ifstream &data);

//removes the last character of a string
void removeLast(std::string &str);

//transform a string to a double
double atod(std::string s);

//if an index is invalid, set it to NO_TYPE
//so that nothing breaks
void checkTypeIndex(typeNum_t &i, size_t max);

//empties a std::queue
template<typename T>
void emptyQueue(std::queue<T> &q){
   while (!q.empty())
      q.pop();
}

//Determines whether the specified entity is
//able to move freely in a straight line to
//its target, without hitting anything
bool isPathClear(const Point &start,
                 const Point &end,
                 const GameData &game,
                 const Entity &entity);

#endif