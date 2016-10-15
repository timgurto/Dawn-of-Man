// (C) 2009-2010 Tim Gurto

//TODO copy comments at declarations to definitions

#include <cassert>
#include "SDL.h"
#include "SDL_ttf.h"
#include "Debug.h"
#include "game.h"

//Default screen resolutions, 16:10 and 4:3
const pixels_t DEFAULT_W_SCREEN_WIDTH = 1280;
const pixels_t DEFAULT_W_SCREEN_HEIGHT = 800;
const pixels_t DEFAULT_SCREEN_WIDTH = 1024;
const pixels_t DEFAULT_SCREEN_HEIGHT = 768;

pixels_t
   SCREEN_WIDTH = DEFAULT_SCREEN_WIDTH,
   SCREEN_HEIGHT = DEFAULT_SCREEN_HEIGHT;
bool WINDOWED_MODE = DEBUG;

//shows framerate and delta values
Debug deltaLog(CYAN, 465, 0, 2);

//general debug messages
Debug debug(YELLOW, 0, 0, 59);

//the number of surfaces loaded
// - increases with loadImage()
// - increases with createSurface()
// - decreases with freeSurface()
int surfacesLoaded = 0;

//Set screen resolution, based on command line arguments
void setScreenResolution(int argc, char **argv);

int main(int argc, char **argv){

   //SDL initialization
   int sdlInit(SDL_Init(SDL_INIT_VIDEO));
   assert (sdlInit == 0);
   int ttfInit(TTF_Init());
   assert (ttfInit >= 0);

   debug.initFont("Dina.fon", 0);
   deltaLog.initFont("Dina.fon", 0);

   setScreenResolution(argc, argv);


   //New game
   gameMode();


   //Quit
   //TTF_Quit() happens at debug dtor
   SDL_Quit();
   assert (surfacesLoaded == 0);
   return 0;
}

void setScreenResolution(int argc, char **argv){
   const SDL_VideoInfo *current = SDL_GetVideoInfo();
   debug("Current resolution: ", current->current_w, "x",
         current->current_h);
   SDL_Rect** resolutions = SDL_ListModes(0, SDL_FULLSCREEN |
                                             SDL_HWSURFACE |
                                             SDL_DOUBLEBUF);
   assert (resolutions != 0);
   debug("Available resolutions:");
   //whether the default screen size is available
   bool defaultResOkay = false;
   bool defaultWResOkay = false; //as above, for widescreen
   while (*resolutions){
      debug((*resolutions)->w, "x", (*resolutions)->h);
      if ((*resolutions)->w == DEFAULT_SCREEN_WIDTH &&
          (*resolutions)->h == DEFAULT_SCREEN_HEIGHT)
         defaultResOkay = true;
      if ((*resolutions)->w == DEFAULT_W_SCREEN_WIDTH &&
          (*resolutions)->h == DEFAULT_W_SCREEN_HEIGHT)
         defaultWResOkay = true;
      ++resolutions;
   }
   //Windowed
   WINDOWED_MODE = DEBUG || isArg("-win", argc, argv);
   if (isArg("-width", argc, argv) &&
       isArg("-height", argc, argv)){
      SCREEN_WIDTH = whatIsArg("-width", argc, argv);
      SCREEN_HEIGHT = whatIsArg("-height", argc, argv);
   //Current resolution
   }else if (isArg("-retain", argc, argv) ||
             !defaultWResOkay && !defaultResOkay){
      SCREEN_WIDTH = current->current_w;
      SCREEN_HEIGHT = current->current_h;
   //Default
   }else if (defaultWResOkay){
      SCREEN_WIDTH = DEFAULT_W_SCREEN_WIDTH;
      SCREEN_HEIGHT = DEFAULT_W_SCREEN_HEIGHT;
   }else{
      SCREEN_WIDTH = DEFAULT_SCREEN_WIDTH;
      SCREEN_HEIGHT = DEFAULT_SCREEN_HEIGHT;
   }

}