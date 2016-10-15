// (C) 2009-2010 Tim Gurto

#include <cassert>

#include "SDL.h"
#include "SDL_ttf.h"

#include "Debug.h"

#include "game.h"

//globals

//shows framerate and delta values
Debug deltaLog(CYAN, 465, 0, 2);

//general debug messages
Debug debug(YELLOW, 0, 0, 59);

//the number of surfaces loaded
// - increases with loadImage()
// - decreases with freeSurface()
int surfacesLoaded(0);

int main(int argc, char* argv[]){
   int sdlInit(SDL_Init(SDL_INIT_VIDEO));
   assert (sdlInit == 0);

   int ttfInit(TTF_Init());
   assert (ttfInit >= 0);

   debug.initFont("Dina.fon", 0);
   deltaLog.initFont("Dina.fon", 0);

   //new game



   gameMode();

   //TTF_Quit(); Quit happens at debug dtor
   SDL_Quit();
   assert (surfacesLoaded == 0);
   return 0;
}