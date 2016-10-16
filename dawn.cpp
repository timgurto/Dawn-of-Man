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

   SDL_ShowCursor(SDL_DISABLE);

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
         game(&gameMode),
         credits;
      buildScreens(mainMenu, credits);

      bool loop = true;
      do{
         switch(mainMenu()){
         case BUTTON_QUIT:
            loop = false;
            break;

         case BUTTON_CREDITS:
            credits();
            break;

         case BUTTON_NEW:
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
            break;
         }
      }while(loop);


   }

   //Quit
   //TTF_Quit() happens at debug dtor
   SDL_ShowCursor(SDL_ENABLE);
   Mix_CloseAudio();
   SDL_Quit();
   Surface::quit();
   assert (soundsLoaded == 0);
   return 0;
}

void buildScreens(Screen &mainMenu,
                  Screen &credits){
   
   //Main menu
   mainMenu.addElement(ScreenElement
      (ELEM_LABEL, "Dawn of Man", ANCHOR_TOP,    Point(0, 40),  ScreenElement::NO_ID,
       0, 0, 75));
   mainMenu.addElement(ScreenElement
      (ELEM_BUTTON, "Begin",      ANCHOR_CENTER, Point(0, -60), BUTTON_NEW));
   mainMenu.addElement(ScreenElement
      (ELEM_BUTTON, "Credits",    ANCHOR_CENTER, 0,             BUTTON_CREDITS));
   mainMenu.addElement(ScreenElement
      (ELEM_BUTTON, "Quit",       ANCHOR_CENTER, Point(0, 60),  BUTTON_QUIT));


   //Credits
   credits.addElement(ScreenElement
      (ELEM_BUTTON, "Back", ANCHOR_BOTTOM_RIGHT, Point(-30, -30), BUTTON_QUIT));

   int yPos = CREDITS_OFFSET;

   addCreditTitle(credits, yPos, "Dawn of Man");
   addCredit     (credits, yPos, "(C) 2009-2010 Tim Gurto");
   addCredit     (credits, yPos, "Sydney, Australia");
   addCreditGap(yPos);

   addCreditTitle(credits, yPos, "Created by");
   addCredit     (credits, yPos, "Tim Gurto");
   addCreditGap(yPos);

   addCreditTitle(credits, yPos, "Lead Tester");
   addCredit     (credits, yPos, "Alex Poms");
   addCreditGap(yPos);

   addCreditTitle(credits, yPos, "Other Testers");
   addCredit     (credits, yPos, "Tim Veizer");
   addCredit     (credits, yPos, "Jeremy Gurto");
   addCreditGap(yPos);

   addCreditTitle(credits, yPos, "Special Thanks");
   addCredit     (credits, yPos, "Evan Pipho");
   addCredit     (credits, yPos, "Drew Banyai");

}

void addCreditTitle(Screen &creditsScreen, int &yPos, std::string text){
   creditsScreen.addElement(ScreenElement(ELEM_LABEL, text, ANCHOR_CENTER,
                                          Point(0, yPos), ScreenElement::NO_ID,
                                          0, 0, 0, CREDITS_HEADING_COLOR));
   yPos += CREDITS_INCREMENT;
}

void addCredit(Screen &creditsScreen, int &yPos, std::string text){
   creditsScreen.addElement(ScreenElement(ELEM_LABEL, text, ANCHOR_CENTER,
                                          Point(0, yPos)));
   yPos += CREDITS_INCREMENT;
}

void addCreditGap(int &yPos){
   yPos += CREDITS_GAP;
}
