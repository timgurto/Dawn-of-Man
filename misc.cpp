// (C) 2009 Tim Gurto

#include <cassert>
#include <string>
#include <sstream>

#include "SDL.h"
#include "SDL_image.h"

#include "Debug.h"

#include "Globals.h"
#include "misc.h"

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
   r.x=x;
   r.y=y;
   r.w=w;
   r.h=h;
   return r;
}

SDL_Color makeColor(Uint8 r, Uint8 g, Uint8 b){
   SDL_Color c;
   c.r = r;
   c.g = g;
   c.b = b;
   return c;
}

SDL_Surface *setScreen(){
   SDL_Surface *screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE);
   SDL_WM_SetCaption( "Dawn of Man", NULL );
   return screen;
}

void blitCursor (SDL_Surface *cursor, SDL_Surface *screen, SDL_MouseMotionEvent coords){
   SDL_BlitSurface(cursor, 0, screen, &makeRect());
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


//========misc=========


std::string makePath(EntityTypeID type, typeNum_t imageNumber, bool isPortrait){
   std::ostringstream path;
   path << IMAGE_PATH; // Images/
   switch (type){
   case BUILDING:
      path << BUILDINGS_IMAGE_PATH; // Units/
      break;
   }

   if (isPortrait)
      path << 'p'; // p

   if (imageNumber < 100) // 0
      path << '0';
   if (imageNumber < 10) // 0
      path << '0';
   path << imageNumber; // 5
   
   path << IMAGE_SUFFIX; // .png
   return path.str();
}