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
   argc; argv; //suppress warnings

   //SDL initialization
   int sdlInit(SDL_Init(SDL_INIT_VIDEO));
   assert (sdlInit == 0);
   int ttfInit(TTF_Init());
   assert (ttfInit >= 0);


   //debug objects initialization
   debug.initFont("Dina.fon", 0);
   deltaLog.initFont("Dina.fon", 0);


   //Screen resolution stuff
   const SDL_VideoInfo *current = SDL_GetVideoInfo();
   debug("Current resolution: ", current->current_w, "x", current->current_h);

   SDL_Rect** resolutions = SDL_ListModes(NULL, SDL_FULLSCREEN|SDL_HWSURFACE);
   assert (resolutions != 0);
   debug("Available resolutions:");
   while (*resolutions){
      debug((*resolutions)->w, "x", (*resolutions)->h);
      ++resolutions;
   }


   //New game
   gameMode();


   //Quit
   //TTF_Quit() happens at debug dtor
   SDL_Quit();
   assert (surfacesLoaded == 0);
   return 0;
}