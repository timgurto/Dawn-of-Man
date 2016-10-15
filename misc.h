// (C) 2009-2010 Tim Gurto

#ifndef MISC_H
#define MISC_H

#include <string>

#include "SDL.h"
#include "globals.h"

class Entity;
class EntityType;
struct Point;
struct GameData;

//===SDL===

//Loads an image file into an SDL_Surface, with optional transparent
//background color.  Increments surfacesLoaded.
SDL_Surface *loadImage(const char* fileName);
SDL_Surface *loadImage(const char* fileName, const SDL_Color &background);
SDL_Surface *loadImage(const std::string fileName);
SDL_Surface *loadImage(const std::string fileName, const SDL_Color &background);

//Fake constructor and copy constructor for SDL_Rect
SDL_Rect makeRect(Sint16 x = 0, Sint16 y = 0, Uint16 w = 0, Uint16 h = 0);
SDL_Rect makeRect(const Point &point);

//Fake constructor for SDL_Color
SDL_Color makeColor(Uint8 r, Uint8 g, Uint8 b);

//Initializes a surface with screen settings
SDL_Surface *setScreen();

//Frees a surface.  Decrements surfacesLoaded.
void freeSurface(SDL_Surface *&p);

//Deep-copies a surface from one pointer to another.
SDL_Surface *copySurface(SDL_Surface* src);

//returns a rectangle with equal dimensions, but co-ordinates
// of zero.
SDL_Rect dimRect(const SDL_Rect &original);


//===misc===

//Constructs a file path to load graphics for entities
std::string makePath(EntityTypeID type, typeNum_t imageNumber,
                     ImageModifier modifier = NONE);

//Checks whether a pointer is NULL
template <typename Type> void checkP(Type *pointer){
   assert (pointer != 0);
}

//Dereferences Entity pointers and performs <
//(used in sorting the Entities list)
bool dereferenceLessThan(Entity *p1, Entity *p2);

//Whether the Point lies on the SDL_Rect
bool pointCollision(const Point &point, const SDL_Rect rect);

//Checks for collisions against every entity in the game.
bool noCollision(const GameData &game, const EntityType &type,
                const Point &mousePos);

//Checks for a collision between two SDL_Rects
bool collision(const SDL_Rect &r1, const SDL_Rect &r2);

//looks up the relevant color based on index input
Uint32 getEntityColor(const GameData &game, int color);

//whether rect A is completely inside rect B
bool inside(const SDL_Rect &a, const SDL_Rect &b);

#endif