#include <cassert>
#include <string>

#include "SDL.h"
#include "SDL_image.h"

#include "RGB.h"

#include "Globals.h"
#include "SDL_wrappers.h"

SDL_Surface *loadImage(const char* fileName){
   SDL_Surface *load, *opt;
   load = IMG_Load(fileName);
   checkP(load);
   opt = SDL_DisplayFormat(load);
   checkP(opt);
   SDL_FreeSurface(load);
   return opt;
}

SDL_Surface *loadImage(const char* fileName, RGB background){
   SDL_Surface *img = loadImage(fileName);
   SDL_SetColorKey(img, SDL_SRCCOLORKEY, SDL_MapRGB( img->format, background.r, background.g, background.b));
   return img;
}

SDL_Surface *loadImage(const std::string fileName){
   return loadImage(fileName.c_str());
}

SDL_Surface *loadImage(const std::string fileName, RGB background){
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

SDL_Surface *setScreen(){
   SDL_Surface *screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE);
   SDL_WM_SetCaption( "Cave Adventures!", NULL );
   return screen;
}

void checkP(SDL_Surface *p){
   assert (p != 0);
}

void blitCursor (SDL_Surface *cursor, SDL_Surface *screen, SDL_MouseMotionEvent coords){
   SDL_BlitSurface(cursor, 0, screen, &makeRect());//CURSOR_OFFSET_X + coords.x, CURSOR_OFFSET_Y + coords.y));
}