// (C) 2009-2010 Tim Gurto

#include <cassert>
#include <string>
#include <sstream>
#include <cstdlib>
#include <cmath>
#include <sstream>
#include <queue>

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_mixer.h"

#include "globals.h"
#include "misc.h"
#include "Entity.h"
#include "Point.h"
#include "GameData.h"
#include "Debug.h"
#include "EntityType.h"

extern Debug debug;
extern int
   surfacesLoaded,
   soundsLoaded;
extern bool WINDOWED_MODE;


//========SDL=========


SDL_Surface *loadImage(const char* fileName,bool alpha){
   debug("loading surface: ", fileName);
   SDL_WM_SetCaption( fileName, NULL );
   SDL_Surface *load, *opt;
   load = IMG_Load(fileName);
   assert(load);
   opt = alpha ?
      SDL_DisplayFormatAlpha(load) :
      SDL_DisplayFormat(load);
   assert(opt);
   SDL_FreeSurface(load);
   ++surfacesLoaded;

   return opt;
}

SDL_Surface *loadImage(const char* fileName,
                       const SDL_Color &background,
                       bool alpha){
   SDL_Surface *img = loadImage(fileName, alpha);
   SDL_SetColorKey(img, SDL_SRCCOLORKEY,
                   SDL_MapRGB(img->format,
                              background.r,
                              background.g,
                              background.b));
   return img;
}

SDL_Surface *loadImage(const std::string fileName,
                       bool alpha){
   return loadImage(fileName.c_str(), alpha);
}

SDL_Surface *loadImage(const std::string fileName,
                       const SDL_Color &background,
                       bool alpha){
   return loadImage(fileName.c_str(), background, alpha);
}

SDL_Surface *createSurface(int width, int height){
   ++surfacesLoaded;
   return SDL_CreateRGBSurface(SDL_HWSURFACE, width, height,
                               SCREEN_BPP, 0, 0, 0, 0);
}

SDL_Sound *loadSound(const std::string &fileName){
   if (fileName == "" || fileName == SOUND_PATH)
      return 0;
   SDL_Sound *p = Mix_LoadWAV(fileName.c_str());
   assert(p);
   ++soundsLoaded;
   return p;
}

SDL_Rect makeRect(Sint16 x, Sint16 y, Uint16 w, Uint16 h){
   SDL_Rect r;
   r.x = x;
   r.y = y;
   r.w = w;
   r.h = h;
   return r;
}

SDL_Rect makeRect(const Point &point){
   return makeRect(point.x, point.y, 0, 0);
}

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

SDL_Surface *setScreen(){
   SDL_Surface *screen = SDL_SetVideoMode(SCREEN_WIDTH,
                                          SCREEN_HEIGHT,
                                          SCREEN_BPP,
                                          SDL_HWSURFACE |
                                          (WINDOWED_MODE ?
                                             0 :
                                             SDL_FULLSCREEN));
   SDL_WM_SetCaption( "Dawn of Man", NULL );
   return screen;
}

void freeSurface(SDL_Surface *&p){
   if (p){
      //debug("Unloading surface");
      SDL_FreeSurface(p);
      p = 0;
      --surfacesLoaded;
   }
}

void freeSound(SDL_Sound *&p){
   if (p){
      Mix_FreeChunk(p);
      p = 0;
      --soundsLoaded;
   }
}

SDL_Surface *copySurface(SDL_Surface* src){
   if (!src)
      return 0;
   ++surfacesLoaded;
   return SDL_ConvertSurface(src,
                             src->format,
                             SDL_SWSURFACE);
}

SDL_Rect dimRect(const SDL_Rect &original){
   return makeRect(0, 0, original.w, original.h);
}

SDL_Rect locRect(const SDL_Rect &original){
   return makeRect(original.x, original.y);
}

bool isKeyPressed(SDLKey key){
   static Uint8 *keyStates = SDL_GetKeyState(0);
   return keyStates[key] != 0;
}

SDL_Rect &operator-=(SDL_Rect &lhs, const SDL_Rect &rhs){
   lhs.x -= rhs.x;
   lhs.y -= rhs.y;
   lhs.w -= rhs.w;
   lhs.h -= rhs.h;
   return lhs;
}

void playSound(SDL_Sound *p){
   if (!DEBUG)
      if (p)
         Mix_PlayChannel(-1, p, 0);
}

void setColorKey(SDL_Surface *surface, const SDL_Color &color){
   assert(surface);
   assert (surface);
   SDL_SetColorKey(surface, SDL_SRCCOLORKEY,
                   SDL_MapRGB(surface->format,
                              color.r,
                              color.g,
                              color.b));
}

void pushMouseMove(const GameData &game){
   SDL_Event fakeMouseMove;
   fakeMouseMove.type = SDL_MOUSEMOTION;
   fakeMouseMove.motion.x = game.mousePos.x;
   fakeMouseMove.motion.y = game.mousePos.y;
   SDL_PushEvent(&fakeMouseMove);
}


//========misc=========


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

bool dereferenceLessThan(Entity *p1, Entity *p2){
   assert (p1);
   assert (p2);
   return *p1 < *p2;
}

bool noCollision(const GameData &game, const EntityType &type,
                 const Point &loc, const Entity *ignore1,
                 const Entity *ignore2){
   SDL_Rect rect = loc + type.getBaseRect();

   //check that it's inside map
   if (!inside(rect, dimRect(game.map)))
      return false;

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
   case CLR_DECORATION:
      return DECORATION_COLOR;
   case CLR_CORPSE:
      return CORPSE_COLOR;
   default:
      return ENTITY_DEFAULT_COLOR;

   }
}

//whether a is inside b
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

pixels_t distance(pixels_t a, pixels_t b){
   return abs(a - b);
}

pixels_t distance(Point a, Point b){
   double ssqr = square(a.x - b.x) +
                 square(a.y - b.y);
   return pixels_t(sqrt(ssqr));
}

double modulo(double a, int b){
   if (a < b)
      return a;
   int aInt = int(a);
   return aInt % b + a - aInt;
}

void centerMap(GameData &game, const Point &center){
   game.map.x = SCREEN_WIDTH / 2 - center.x;
   game.map.y = SCREEN_HEIGHT / 2 - center.y;
}

bool isPathClear(const Point &start,
                 const Point &end,
                 const GameData &game,
                 const Entity &entity,
                 double angle){
   const EntityType &thisType = entity.type();


   //if not already done, calculate angle
   if (angle == DUMMY_ANGLE)
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
         if (end.x < start.x)
            if (end.y > start.y)
               angle += PI;
            else
               angle -= PI;
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

std::string format3(double x){
   std::ostringstream oss;
   if (x < 100)
      oss << ' ';
   oss << x;
   return oss.str();
}

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

void removeLast(std::string &str){
   str = str.substr(0, str.size() - 1);
}

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

void checkTypeIndex(typeNum_t &i, size_t max){
   if (i != NO_TYPE && (i < 0 ||
                        i >= max))
      i = NO_TYPE;
}