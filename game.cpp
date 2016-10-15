#include <cassert>
#include <vector>
#include <sstream>
#include <time.h>

#include "SDL.h"
#include "misc.h"

#include "Debug.h"
#include "BuildingType.h"
#include "globals.h"
#include "game.h"

extern Debug debug;

void gameMode(){
   SDL_Surface *screen = setScreen();
   debug.setScreen(screen);

debug("Beginning game mode");


   SDL_Surface *back = loadImage(IMAGE_PATH + "back.png");
   SDL_Surface *image = loadImage(IMAGE_PATH + "c0.png", MAGENTA);
   SDL_Surface *cursor = loadImage(IMAGE_PATH + "cursor.png", GREEN);

   screenSize_t
      mouseX(SCREEN_WIDTH/2),
      mouseY(SCREEN_HEIGHT/2);

   SDL_ShowCursor(SDL_DISABLE);

   //init
   std::vector<BuildingType> buildingTypes;
   buildingTypes.push_back(BuildingType("Campfire", 0,
                             makeRect(-61, -118, 114, 143),
                             makeRect(-61, -15, 111, 40),
                             GREEN));

   typeNum_t toBuild = 0;
   

   timer_t ticks(SDL_GetTicks());
   timer_t lastCalcTick(ticks), lastDrawTick(ticks);

   bool loop = true;
   while (loop){
      // Handle events
      SDL_Event event;
      while (SDL_PollEvent(&event) ){
         switch (event.type){
         case SDL_MOUSEMOTION:
            mouseX = event.motion.x;
            mouseY = event.motion.y;
            break;
         case SDL_KEYDOWN:
            switch (event.key.keysym.sym){
            case SDLK_PRINT:
               std::ostringstream os;
               os << "shot" << time(0) << ".bmp";
               SDL_SaveBMP(screen, os.str().c_str());
            }
            break;
         case SDL_QUIT: //If the user has Xed out the window
            loop = false;
            break;
         }
      }
      
      ticks = SDL_GetTicks();

      // Update state if necessary
      if (ticks - lastCalcTick >= CALC_MS){
         debug("Tick: updating state");
         updateState();
         lastCalcTick = MIN_WAIT ? ticks : lastCalcTick + CALC_MS;
      }


      // Redraw if necessary
      if (ticks - lastDrawTick >= DRAW_MS){
         debug("Tick: redrawing");
         drawEverything(screen, buildingTypes.front().surface_,
                        back, image, cursor, mouseX, mouseY);
         lastDrawTick = MIN_WAIT ? ticks : lastDrawTick + DRAW_MS;
      }

   }


   SDL_ShowCursor(SDL_ENABLE);

   debug("Unloading Back");
   freeSurface(back);
   debug("Unloading Image");
   freeSurface(image);
   debug("Unloading Cursor");
   freeSurface(cursor);
   debug("Done unloading");

}

void drawEverything(SDL_Surface *screen,
                    SDL_Surface *s, SDL_Surface *back,
                    SDL_Surface *image, SDL_Surface *cursor,
                    screenSize_t mouseX, screenSize_t mouseY){
   SDL_FillRect(screen, 0, 0);
   SDL_BlitSurface(back, 0, screen, &makeRect());
   SDL_BlitSurface(image, 0, screen, &makeRect(50,50));
   
      SDL_BlitSurface(s, 0, screen, &makeRect(200,200));
   SDL_BlitSurface(cursor, 0, screen, &makeRect(
      CURSOR_OFFSET_X + mouseX, CURSOR_OFFSET_Y + mouseY));
   debug.display();
   bool test = SDL_Flip(screen) == 0;
   assert(test);
}

void updateState(){}
