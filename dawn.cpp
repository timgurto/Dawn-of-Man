// (C) 2009-2010 Tim Gurto

#include <cassert>
#include <fstream>
#include <ctime>
#include <cstdlib>

#include "SDL.h"
#include "SDL_ttf.h"
#include "SDL_mixer.h"
#include "dawn.h"
#include "game.h"
#include "globals.h"
#include "Debug.h"
#include "Screen.h"
#include "Surface.h"

//TODO: try to replace get()s with const refs, or reaffirm why it can't be done

//TODO replace with ranked priorities, including 16:9
//Default screen resolutions, 16:10 and 4:3
const pixels_t DEFAULT_W_SCREEN_WIDTH = 1280;
const pixels_t DEFAULT_W_SCREEN_HEIGHT = 800;
const pixels_t DEFAULT_SCREEN_WIDTH = 1024;
const pixels_t DEFAULT_SCREEN_HEIGHT = 768;

//global screen buffer
Surface screenBuf; //uninitialized

pixels_t
   SCREEN_WIDTH = DEFAULT_SCREEN_WIDTH,
   SCREEN_HEIGHT = DEFAULT_SCREEN_HEIGHT;
bool WINDOWED_MODE = DEBUG;

//general debug messages
Debug debug(YELLOW, 0, 0, 59);

//the number of sounds loaded
// - increases with loadSound()
// - decreases with freeSound()
int soundsLoaded = 0;

int main(int argc, char **argv){

   //seed random generator
   srand(unsigned(time(0)));

   //SDL initialization
   int sdlInit(SDL_Init(SDL_INIT_VIDEO));
   assert (sdlInit == 0);
   int ttfInit(TTF_Init());
   assert (ttfInit >= 0);
   int mixInit(Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 256));//256));
   assert (mixInit >= 0);

   debug.initFont("Dina.fon", 0);

   setScreenResolution(argc, argv);

   //initialize screen buffer
   screenBuf = Surface(SUR_SCREEN);
   
   {//new scope for surfaces

      //Other surfaces needed for Screens
      Surface
         background       (MISC_IMAGE_PATH + "dark.PNG"),
         cursor        (MISC_IMAGE_PATH + "cursor.PNG",         GREEN),
         cursorShadow  (MISC_IMAGE_PATH + "cursorShadow.PNG",   GREEN);
      cursorShadow.setAlpha(SHADOW_ALPHA);

      //init with surfaces
      Screen::init(&background, &cursor);

      Screen mainMenu;
      //mainMenu();

      Screen game;
      game();

      //campaign: go through each level
      int levels;
      for (levels = 0;
           std::ifstream((DATA_PATH + format2(levels) + ".dat").c_str());
           ++levels);
      //levels = number of levels in the campaign

      for (int i = 0; i != levels; ++i){
         GameOutcome outcome;
         //repeat each level if lost
         while ((outcome = gameMode(game, &(format2(i) + ".dat"))) == LOSS)
            ;
         if (outcome == QUIT)
            break;
      }

   }

   //Quit
   //TTF_Quit() happens at debug dtor
   Mix_CloseAudio();
   SDL_Quit();
   Surface::quit();
   assert (soundsLoaded == 0);
   return 0;
}

void setScreenResolution(int argc, char **argv){
   const SDL_VideoInfo *current = SDL_GetVideoInfo();
   debug("Current resolution: ", current->current_w, "x",
         current->current_h);
   SDL_Rect** resolutions = SDL_ListModes(0, SDL_FULLSCREEN |
                                             SDL_HWSURFACE |
                                             SDL_DOUBLEBUF);
   assert (resolutions);
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