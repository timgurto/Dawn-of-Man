// (C) 2009-2010 Tim Gurto

#include <cassert>
#include <string>
#include <sstream>
#include <cstdlib>
#include <cmath>
#include <sstream>
#include <queue>

#include "SDL_mixer.h"

#include "globals.h"
#include "misc.h"
#include "Entity.h"
#include "Point.h"
#include "GameData.h"
#include "Debug.h"
#include "EntityType.h"
#include "Screen.h"

extern Debug debug;
extern int
   surfacesLoaded,
   soundsLoaded;
extern bool WINDOWED_MODE;


//========SDL=========


//Loads a sound, and registers it with soundsLoaded
SDL_Sound *loadSound(const std::string &fileName){
   if (fileName == "" || fileName == SOUND_PATH)
      return 0;
   SDL_Sound *p = Mix_LoadWAV(fileName.c_str());
   assert(p);
   ++soundsLoaded;
   return p;
}

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
   return (color.r << 16) |
          (color.g <<  8) |
          (color.b      );
}

//Free a sound, and update soundsLoaded
void freeSound(SDL_Sound *&p){
   if (p){
      Mix_FreeChunk(p);
      p = 0;
      --soundsLoaded;
   }
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

//plays a sound
void playSound(SDL_Sound *p){
   if (!DEBUG)
      if (p)
         Mix_PlayChannel(-1, p, 0);
}

//push a mouse-move event onto the queue, to refresh the
//calculations handled there
void pushMouseMove(const GameData &game){
   SDL_Event fakeMouseMove;
   fakeMouseMove.type = SDL_MOUSEMOTION;
   fakeMouseMove.motion.x = Screen::mousePos.x;
   fakeMouseMove.motion.y = Screen::mousePos.y;
   SDL_PushEvent(&fakeMouseMove);
}


//========misc=========


//Constructs a file path to load graphics for entities
std::string makePath(EntityTypeID type, typeNum_t imageNumber,
                     ImageModifier modifier){
   std::ostringstream path;
   switch (type){
   case ENT_BUILDING:
      path << BUILDINGS_IMAGE_PATH;
      break;
   case ENT_DECORATION:
      path << DECORATIONS_IMAGE_PATH;
      break;
   case ENT_UNIT:
      path << UNITS_IMAGE_PATH;
      break;
   case ENT_RESOURCE_NODE:
      path << RESOURCE_NODES_IMAGE_PATH;
      break;
   case ENT_TECH:
      path << TECHS_IMAGE_PATH;
      break;
   }

   switch (modifier){
   case IMG_ICON:
      path << 'p'; // p
      break;
   case IMG_MASK:
      path << 'm'; // m
      break;
   }

   assert (imageNumber < 1000);
   if (imageNumber < 100) // 0
      path << '0';
   if (imageNumber < 10) // 0
      path << '0';
   path << imageNumber; // 5
   
   path << IMAGE_SUFFIX; // .png
   return path.str();
}

//Dereferences Entity pointers and performs <
//(used in sorting the Entities list)
bool dereferenceLessThan(Entity *p1, Entity *p2){
   assert (p1);
   assert (p2);
   return *p1 < *p2;
}

//Checks for collisions against every entity in the game:
//a potential entity, of given type and location
//ignore: an entity to skip checking, i.e. the current unit
bool noCollision(const GameData &game, const EntityType &type,
                 const Point &loc, const Entity *ignore1,
                 const Entity *ignore2){
   SDL_Rect rect = loc + type.getBaseRect();

   //check that it's inside map
   if (!inside(rect, dimRect(game.map)))
      return false;

   //TODO only check nearby entities
   //check against entities
   for (entities_t::const_iterator it = game.entities.begin();
        it != game.entities.end(); ++it){
      if (*it != ignore1 &&
          *it != ignore2 &&
          (*it)->collides() &&
          collision(rect, (*it)->getBaseRect()))
         return false;
   }
   return true;
}

//Checks for a collision between two SDL_Rects
bool collision(const SDL_Rect &r1, const SDL_Rect &r2){
   if (r1.x > r2.x + r2.w)
      return false;
   if (r2.x > r1.x + r1.w)
      return false;
   if (r1.y > r2.y + r2.h)
      return false;
   if (r2.y > r1.y + r1.h)
      return false;
   return true;
}

//Whether a Point lies in an SDL_Rect
bool collision(const Point &point, const SDL_Rect &rect){
   if (point.x < rect.x) return false;
   if (point.y < rect.y) return false;
   if (point.x > rect.x + rect.w) return false;
   if (point.y > rect.y + rect.h) return false;
   return true;
}

bool collision(const SDL_Rect &rect, const Point &point){
   return collision(point, rect);
}

//looks up the relevant color based on index input
Uint32 getEntityColor(const GameData &game, int color){
   //if a player color
   if (color < MAX_PLAYERS)
      return game.players[color].getColor();
   
   //otherwise
   switch(color){
   case CLR_BLACK:
      return BLACK_UINT;
   case CLR_RESOURCE_WOOD:
      return RESOURCE_WOOD_COLOR;
   case CLR_RESOURCE_FOOD:
      return RESOURCE_FOOD_COLOR;
   case CLR_RESOURCE_STONE:
      return RESOURCE_STONE_COLOR;
   case CLR_RESOURCE_METAL:
      return RESOURCE_METAL_COLOR;
   case CLR_DECORATION:
      return DECORATION_COLOR;
   case CLR_CORPSE:
      return CORPSE_COLOR;
   default:
      return ENTITY_DEFAULT_COLOR;

   }
}

//whether rect A is completely inside rect B
bool inside(const SDL_Rect &a, const SDL_Rect &b){
   if (a.x < b.x)
      return false;
   if (a.y < b.y)
      return false;
   if (a.x + a.w > b.x + b.w)
      return false;
   if (a.y + a.h > b.y + b.h)
      return false;

   return true;
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

//Centers the map on the provided co-ordinates.
//Map edges are taken into account automatically
void centerMap(GameData &game, const Point &center){
   game.map.x = SCREEN_WIDTH / 2 - center.x;
   game.map.y = SCREEN_HEIGHT / 2 - center.y;
}

//Determines whether the specified entity is
//able to move freely in a straight line to
//its target, without hitting anything
bool isPathClear(const Point &start,
                 const Point &end,
                 const GameData &game,
                 const Entity &entity){
   const EntityType &thisType = entity.type();


   //calculate angle
   double angle;
   if (end.x == start.x)
      angle =
         end.y < start.y ?
            1.5 * PI :
            0.5 * PI;
   else{
      double gradient =
         1.0 *
         (end.y - start.y) /
         (end.x - start.x);
      angle = atan(gradient);
      if (end.x < start.x){
         if (end.y > start.y)
            angle += PI;
         else
            angle -= PI;
      }
   }

   double
      xDelta = cos(angle) * PATH_CHECK_DISTANCE,
      yDelta = sin(angle) * PATH_CHECK_DISTANCE;

   double x = start.x, y = start.y;
   bool finished = false;
   while (!finished){
      //debug("Checking ", x, ",", y);
      if (!noCollision(game,
                       thisType,
                       Point(pixels_t(x),
                             pixels_t(y)),
                       &entity))
         return false;

      x += xDelta;
      y += yDelta;
      finished =
         (start.x < end.x ? x >= end.x : x <= end.x) &&
         (start.y < end.y ? y >= end.y : y <= end.y);
   }
   return true;
}

//Adds a leading zero if x < 100 (fps display)
std::string format3(double x){
   std::ostringstream oss;
   if (x < 100)
      oss << ' ';
   oss << x;
   return oss.str();
}

//Adds a leading zero if x < 10 (game file names)
std::string format2(int x){
   std::ostringstream oss;
   if (x < 10)
      oss << '0';
   oss << x;
   debug (oss.str());
   return oss.str();
}

//Parse the next token from a data file
std::string parseToken(std::ifstream &data){
   std::ostringstream os;
   while (data){
      bool skip = false;
      char c;
      data >> c;
      //end if punctuation
      if (c == '{' ||
          c == '}' ||
          c == '=' ||
          c == ';'){
         os << c;
         break;
      }
      //skip rest of line if comment
      else if (c == '#'){
         do
            data.ignore(1);
         while (data.peek() != '\n'); //(c != '#');
         skip = true;
      }
      else if (c == '\"'){
         os << c;
         do{
            //stop >> from ignoring spaces
            if (data.peek() == ' '){
               c = ' ';
               data.ignore(1);
            }else
               data >> c;
            os << c;
         }while(c != '\"');
         data >> c;
         os << c;
         break;
      }
      if (!skip)
         os << c;
   }
   std::string ret = os.str();
   //debug(ret);
   return ret;
}

//removes the last character of a string
void removeLast(std::string &str){
   str = str.substr(0, str.size() - 1);
}

//transform a string to a double
double atod(std::string s){
   std::istringstream is(s);
   if (s.substr(0, 2) == "0x"){
      s = s.substr(2);
      long l;
      is >> std::hex >> l;
      return l;
   }
   double d;
   is >> d;
   return d;
}

//if an index is invalid, set it to NO_TYPE
//so that nothing breaks
void checkTypeIndex(typeNum_t &i, size_t max){
   if (i != NO_TYPE && (i < 0 ||
                        i >= max))
      i = NO_TYPE;
}