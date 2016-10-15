//Standard Library
#include <cassert>
#include <string>

//SDL
#include "SDL.h"
#include "SDL_image.h"

//Classes
#include "RGB.h"
#include "Point.h"
#include "CaveBuildingType.h"

//Functions
#include "SDL_wrappers.h"
#include "cave.h"

int main(int argc, char* args[]){
   assert (SDL_Init(SDL_INIT_EVERYTHING) != -1);

   //new game

   

   caveMode();

   SDL_Quit();
   return 0;
}