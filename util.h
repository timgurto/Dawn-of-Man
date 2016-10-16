// (C) 2010 Tim Gurto

#ifndef UTIL_H
#define UTIL_H

#include <string>
#include "SDL.h"
#include "types.h"

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

//SDL_Color */ double
SDL_Color operator*(const SDL_Color &lhs, double rhs);
SDL_Color operator/(const SDL_Color &lhs, double rhs);

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

//Checks for a collision between two SDL_Rects
bool collision(const SDL_Rect &r1, const SDL_Rect &r2);

//Whether a Point lies in an SDL_Rect
bool collision(const Point &point, const SDL_Rect &rect);
bool collision(const SDL_Rect &rect, const Point &point);

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

//logical XOR
bool lXor(bool a, bool b);

//removes the last character of a string
void removeLast(std::string &str);

//transform a string to a double
double atod(std::string s);

#endif