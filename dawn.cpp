//Standard Library
#include <cassert>

//SDL
#include "SDL.h"
#include "SDL_ttf.h"

#include "Debug.h"

//Functions
#include "game.h"

//globals
Debug debug(makeColor(0xff, 0xff, 0), 0, 0, 59);
int surfacesLoaded(0);

int main(int argc, char* args[]){
   int sdlInit(SDL_Init(SDL_INIT_EVERYTHING));
   assert (sdlInit != -1);

   int ttfInit(TTF_Init());
   assert (ttfInit >= 0);

   debug.setFont("Dina.fon", 0);

   //new game



   gameMode();

   //TTF_Quit(); Quit now happens with debug dtor
   SDL_Quit();
   assert (surfacesLoaded == 0);
   return 0;
}