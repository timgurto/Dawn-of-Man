// (C) 2009, 2010 Tim Gurto

#include <cassert>
#include <fstream>
#include <ctime>
#include <cstdlib>
#include <string>

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
#include "misc.h"

//global screen buffer
Surface screenBuf; //uninitialized

//general debug messages
Debug debug(YELLOW, 0, 0, 59);

//can be reused for whichever screens; the useful bit is the name
enum ButtonID{
   //ScreenElement::NO_ID = -1
   BUTTON_QUIT = 0,
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

   Screen::setScreenResolution(argc, argv);
   debug.initFont(FONT_DEBUG, 0);
   SDL_ShowCursor(SDL_DISABLE);

   //initialize screen buffer
   screenBuf = Surface(SUR_SCREEN);
   
   {//new scope for surfaces

      //Other surfaces needed for Screens
      Surface
         background   (MISC_IMAGE_PATH + "dark.PNG"),
         cursor       (MISC_IMAGE_PATH + "cursor.PNG",       GREEN),
         cursorShadow (MISC_IMAGE_PATH + "cursorShadow.PNG", GREEN);
      cursorShadow.setAlpha(SHADOW_ALPHA);

      //init with surfaces
      Screen::init(&background, &cursor);
      Screen
         mainMenu(BUTTON_NEW, BUTTON_QUIT),
         game(&gameMode),
         credits(BUTTON_QUIT, BUTTON_QUIT);
      buildScreens(mainMenu, credits);

      //bool loop = true;
      //do{
      //   switch(mainMenu()){
      //   case BUTTON_QUIT:
      //      loop = false;
      //      break;

      //   case BUTTON_CREDITS:
      //      credits();
      //      break;

      //   case BUTTON_NEW:
      //      {
      //         int outcome = playCampaign(game);
      //         if (outcome == ALT_F4)
      //            loop = false;
      //      }
      //      break;
      //   }
      //}while(loop);
      game(&(std::string)"00.dat");

   }

   //Quit
   //TTF_Quit() happens at debug dtor
   SDL_ShowCursor(SDL_ENABLE);
   Mix_CloseAudio();
   SDL_Quit();
   Surface::quit();
   Sound::quit();
   return 0;
}

void buildScreens(Screen &mainMenu,
                  Screen &credits){
   
   //Main menu
   mainMenu.addElement(ScreenElement
      (ELEM_LABEL, "Dawn of Man", ANCHOR_TOP,    Point(0, 40),  ScreenElement::NO_ID,
       0, 0, 75));
   mainMenu.addElement(ScreenElement
      (ELEM_BUTTON, "Begin Game", ANCHOR_CENTER, Point(0, -60), BUTTON_NEW));
   mainMenu.addElement(ScreenElement
      (ELEM_BUTTON, "Credits",    ANCHOR_CENTER, Point(0, 0),   BUTTON_CREDITS));
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
   addCredit     (credits, yPos, "Drew Banyai");
   addCredit     (credits, yPos, "Evan Pipho");
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

int playCampaign(Screen &game){
   int levels;
   for (levels = 0;
        std::ifstream((DATA_PATH + format2(levels) + ".dat").c_str());
        ++levels);
   //after loop, levels = number of levels in the campaign
   //play each consecutive level
   unsigned outcome = QUIT;
   for (int i = 0; i != levels; ++i){
      //repeat each level if lost
      std::string filename = format2(i) + ".dat";
      while ((outcome = game(&filename)) == LOSS)
         ;
      if (outcome == QUIT || outcome == ALT_F4)
         break;
   }
   return outcome;
}