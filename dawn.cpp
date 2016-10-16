// (C) 2009, 2010 Tim Gurto

#include <cassert>
#include <fstream>
#include <ctime>
#include <cstdlib>

#include "SDL.h"
#include "SDL_ttf.h"
#include "SDL_mixer.h"
#include "game.h"
#include "globals.h"
#include "Debug.h"
#include "Screen.h"
#include "Surface.h"
#include "Point.h"
#include "ScreenElement.h"
#include "dawn.h"

//TODO try to replace get()s with const refs, or reaffirm why it can't be done

//global screen buffer
Surface screenBuf; //uninitialized

bool WINDOWED_MODE = DEBUG;

//general debug messages
Debug debug(YELLOW, 0, 0, 59);

//the number of sounds loaded
// - increases with loadSound()
// - decreases with freeSound()
int soundsLoaded = 0;

//can be reused for whichever screens; the useful bit is the name
enum ButtonID{
   //ScreenElement::NO_ID = -1
   BUTTON_QUIT,
   BUTTON_NEW,
   BUTTON_CREDITS
};

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

   //TODO const font names
   debug.initFont("Dina.fon", 0);

   Screen::setScreenResolution(argc, argv);

   //initialize screen buffer
   screenBuf = Surface(SUR_SCREEN);
   
   {//new scope for surfaces

      //Other surfaces needed for Screens
      Surface
         background    (MISC_IMAGE_PATH + "dark.PNG"),
         cursor        (MISC_IMAGE_PATH + "cursor.PNG",         GREEN),
         cursorShadow  (MISC_IMAGE_PATH + "cursorShadow.PNG",   GREEN);
      cursorShadow.setAlpha(SHADOW_ALPHA);

      //init with surfaces
      Screen::init(&background, &cursor);
      Screen
         mainMenu,
         game(&gameMode);
      buildScreens(mainMenu, game);

      mainMenu();

      //campaign: go through each level
      int levels;
      for (levels = 0;
           std::ifstream((DATA_PATH + format2(levels) + ".dat").c_str());
           ++levels);
      //after loop, levels = number of levels in the campaign

      //play each consecutive level
      for (int i = 0; i != levels; ++i){
         unsigned outcome;
         //repeat each level if lost
         std::string filename = format2(i) + ".dat";
         while ((outcome = game(&filename)) == LOSS)
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

void buildScreens(Screen &mainMenu,
                  Screen &game){
   
   //Main menu
   mainMenu.addElement(ScreenElement(ELEM_LABEL,
                                     "Dawn of Man",
                                     ANCHOR_TOP,
                                     Point(0, 40),
                                     ScreenElement::NO_ID,
                                     0, 0,
                                     75));
   mainMenu.addElement(ScreenElement(ELEM_BUTTON,
                                     "Begin",
                                     ANCHOR_CENTER,
                                     Point(0, -60),
                                     BUTTON_NEW));
   mainMenu.addElement(ScreenElement(ELEM_BUTTON,
                                     "Credits",
                                     ANCHOR_CENTER,
                                     0,
                                     BUTTON_CREDITS));
   mainMenu.addElement(ScreenElement(ELEM_BUTTON,
                                     "Quit",
                                     ANCHOR_CENTER,
                                     Point(0, 60),
                                     BUTTON_QUIT));
}