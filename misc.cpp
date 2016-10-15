// (C) 2009-2010 Tim Gurto

#include <cassert>
#include <string>
#include <sstream>

#include "SDL.h"
#include "SDL_image.h"

#include "Debug.h"

#include "globals.h"
#include "misc.h"
#include "Entity.h"
#include "Point.h"
#include "GameData.h"

extern Debug debug;
extern int surfacesLoaded;


//========SDL=========


SDL_Surface *loadImage(const char* fileName){
   debug("Loading surface: ", fileName);
   std::string strFile(fileName);
   SDL_Surface *load, *opt;
   load = IMG_Load(fileName);
   checkP(load);
   opt = SDL_DisplayFormat(load);
   checkP(opt);
   SDL_FreeSurface(load);
   ++surfacesLoaded;

   return opt;
}

SDL_Surface *loadImage(const char* fileName, const SDL_Color &background){
   SDL_Surface *img = loadImage(fileName);
   SDL_SetColorKey(img, SDL_SRCCOLORKEY, SDL_MapRGB( img->format, background.r, background.g, background.b));
   return img;
}

SDL_Surface *loadImage(const std::string fileName){
   return loadImage(fileName.c_str());
}

SDL_Surface *loadImage(const std::string fileName, const SDL_Color &background){
   return loadImage(fileName.c_str(), background);
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

SDL_Surface *setScreen(){
   SDL_Surface *screen = SDL_SetVideoMode(SCREEN_WIDTH,
                                          SCREEN_HEIGHT,
                                          SCREEN_BPP,
                                          SDL_HWSURFACE |
                                          (DEBUG ?
                                             0 :
                                             SDL_FULLSCREEN));
   SDL_WM_SetCaption( "Dawn of Man", NULL );
   return screen;
}

void freeSurface(SDL_Surface *&p){
   if (p != 0){
      debug("Unloading surface");
      SDL_FreeSurface(p);
      p = 0;
      --surfacesLoaded;
   }
}

SDL_Surface *copySurface(SDL_Surface* src){
   assert(src != 0);
   ++surfacesLoaded;
   return SDL_ConvertSurface(src, src->format, SDL_SWSURFACE);
}

SDL_Rect dimRect(const SDL_Rect &original){
   return makeRect(0, 0, original.w, original.h);
}


//========misc=========


std::string makePath(EntityTypeID type, typeNum_t imageNumber,
                     ImageModifier modifier){
   std::ostringstream path;
   switch (type){
   case BUILDING:
      path << BUILDINGS_IMAGE_PATH;
      break;
   case DECORATION:
      path << DECORATIONS_IMAGE_PATH;
      break;
   }

   switch (modifier){
   case ICON:
      path << 'p'; // p
      break;
   case MASK:
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
   assert (p1 != 0);
   assert (p2 != 0);
   return *p1 < *p2;
}

bool pointCollision(const Point &point, const SDL_Rect rect){
   if (point.x < rect.x) return false;
   if (point.y < rect.y) return false;
   if (point.x > rect.x + rect.w) return false;
   if (point.y > rect.y + rect.h) return false;
   return true;
}

bool noCollision(const GameData &game, const EntityType &type,
               const Point &mousePos){
   SDL_Rect rect = mousePos + type.getBaseRect();

   //check that it's inside map
   if (!inside(rect, game.map))
      return false;
   
   rect = rect - game.map;

   //check against entities
   for (entities_t::const_iterator it = game.entities.begin();
        it != game.entities.end(); ++it){
      if ((*it)->collides() && collision(rect, (*it)->getBaseRect()))
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

Uint32 getEntityColor(const GameData &game, int color){
   //if a player color
   if (color < MAX_PLAYERS)
      return game.players[color].color;
   
   //otherwise
   //switch(color){
   //default:
      return ENTITY_DEFAULT_COLOR;

   //}
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