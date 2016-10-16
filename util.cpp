// (C) 2010 Tim Gurto

#include <cassert>
#include <cmath>
#include "util.h"
#include "Debug.h"

//TODO remove once Sound class is made
#include "globals.h"
extern int soundsLoaded;

extern Debug debug;

//Fake constructor
SDL_Rect makeRect(Sint16 x, Sint16 y, Uint16 w, Uint16 h){
   SDL_Rect r;
   r.x = x;
   r.y = y;
   r.w = w;
   r.h = h;
   return r;
}

//Fake copy constructor
SDL_Rect makeRect(const Point &point){
   return makeRect(point.x, point.y, 0, 0);
}

//Fake constructors
SDL_Color makeColor(Uint8 r, Uint8 g, Uint8 b){
   SDL_Color c;
   c.r = r;
   c.g = g;
   c.b = b;
   return c;
}

SDL_Color makeColor(Uint32 c){
   SDL_Color col;
   col.r = c >> 16 & 0xff;
   col.g = c >>  8 & 0xff;
   col.b = c       & 0xff;
   return col;
}

//SDL_Color -> Uint32
//note: .unused is ignored
Uint32 colorToUInt(const SDL_Color &color){
   return 
      (color.r << 16) |
      (color.g <<  8) |
      (color.b      );
}

//equality of colors
bool operator==(const SDL_Color &lhs, const SDL_Color &rhs){
   return
      lhs.r == rhs.r &&
      lhs.g == rhs.g &&
      lhs.b == rhs.b &&
      lhs.unused == rhs.unused;
}

//SDL_Color / int
SDL_Color operator/(const SDL_Color &lhs, int rhs){
   SDL_Color c = lhs;
   if (rhs > 0){
      c.r /= rhs;
      c.g /= rhs;
      c.b /= rhs;
   }
   return c;
}

//returns a rectangle with equal dimensions, but co-ordinates
// of zero.
SDL_Rect dimRect(const SDL_Rect &original){
   return makeRect(0, 0, original.w, original.h);
}

//returns a rectangle with equal co-ordinates, but dimensions
// of zero.
SDL_Rect locRect(const SDL_Rect &original){
   return makeRect(original.x, original.y);
}

//Whether the specified key is currently down
bool isKeyPressed(SDLKey key){
   static Uint8 *keyStates = SDL_GetKeyState(0);
   return keyStates[key] != 0;
}

//SDL_Rect -= SDL_Rect
SDL_Rect &operator-=(SDL_Rect &lhs, const SDL_Rect &rhs){
   lhs.x -= rhs.x;
   lhs.y -= rhs.y;
   lhs.w -= rhs.w;
   lhs.h -= rhs.h;
   return lhs;
}

//SDL_Color != SDL_Color
bool operator!=(const SDL_Color &lhs, const SDL_Color &rhs){
   return lhs.r != rhs.r ||
          lhs.g != rhs.g ||
          lhs.b != rhs.b ||
          lhs.unused != rhs.unused;
}

//Checks for a collision between two SDL_Rects
bool collision(const SDL_Rect &r1, const SDL_Rect &r2){
   return
      r1.x <= r2.x + r2.w &&
      r2.x <= r1.x + r1.w &&
      r1.y <= r2.y + r2.h &&
      r2.y <= r1.y + r1.h;
}

//Whether a Point lies in an SDL_Rect
bool collision(const Point &point, const SDL_Rect &rect){
   return
      point.x >= rect.x &&
      point.y >= rect.y &&
      point.x <= rect.x + rect.w &&
      point.y <= rect.y + rect.h;
}

bool collision(const SDL_Rect &rect, const Point &point){
   return collision(point, rect);
}

//whether rect A is completely inside rect B
bool inside(const SDL_Rect &a, const SDL_Rect &b){
   return
      a.x >= b.x &&
      a.y >= b.y &&
      a.x + a.w <= b.x + b.w &&
      a.y + a.h <= b.y + b.h;
}

//Whether an argument exists
bool isArg(std::string arg, int argc, char* argv[]){
   assert (argv);
   for (int i = 1; i != argc; ++i){
      std::string s(argv[i]);
      size_t len = s.find('=');
      if (s.substr(0, len) == arg){
         debug("Argument exists: ", s.substr(0, len));
         return true;
      }
   }
   return false;
}

//The numeric value associated with an argument
// arg=???
int whatIsArg(std::string arg, int argc, char* argv[]){
   assert (argv);
   assert(isArg(arg, argc, argv));
   for (int i = 1; i != argc; ++i){
      std::string s(argv[i]);
      size_t len = s.find('=');
      if (s.substr(0, len) == arg){
         std::stringstream ss(s.substr(len + 1));
         int result;
         ss >> result;
         debug(arg, " = ", result);
         return result;
      }
   }
   assert(false);
   return -1;
}

//The one-dimensional distance between two points
pixels_t distance(pixels_t a, pixels_t b){
   return abs(a - b);
}

//The two-dimensional distance between two points
pixels_t distance(Point a, Point b){
   double ssqr = square(a.x - b.x) +
                 square(a.y - b.y);
   return pixels_t(sqrt(ssqr));
}

//double % int, preserves fraction
double modulo(double a, int b){
   if (a < b)
      return a;
   int aInt = int(a);
   return aInt % b + a - aInt;
}

