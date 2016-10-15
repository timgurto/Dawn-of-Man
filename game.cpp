// (C) 2009 Tim Gurto

#include <cassert>
#include <vector>
#include <sstream>
#include <time.h>
#include <algorithm>

#include "SDL.h"
#include "misc.h"
#include "Debug.h"
#include "GameData.h"
#include "globals.h"
#include "game.h"
#include "Entity.h"
#include "EntityPtr.h"

extern Debug debug;

void gameMode(){
   SDL_Surface *screen = setScreen();
   debug.setScreen(screen);

   SDL_Surface *back = loadImage(IMAGE_PATH + "back.png");
   SDL_Surface *image = loadImage(IMAGE_PATH + "c0.png", MAGENTA);
   SDL_Surface *cursor = loadImage(IMAGE_PATH + "cursor.png", GREEN);

   //TODO make mouse position a Point
   pixels_t
      mouseX(SCREEN_WIDTH/2),
      mouseY(SCREEN_HEIGHT/2);

   SDL_ShowCursor(SDL_DISABLE);

   //init
   GameData game;

   BuildingType campfire(0, "Campfire",
                         makeRect(-61, -118, 114, 143),
                         makeRect(-61, -15, 111, 40),
                         GREEN);
   game.buildingTypes.push_back(campfire);

   typeNum_t toBuild = 0;
   

   timer_t ticks(SDL_GetTicks());
   timer_t lastCalcTick(ticks), lastDrawTick(ticks);
   debug("Redraw frequency: ", DRAW_MS, "ms");
   debug("State update frequency: ", CALC_MS, "ms");

   bool loop = true;
   while (loop){
      // Handle events
      SDL_Event event;
      while (SDL_PollEvent(&event) ){
         switch (event.type){
         case SDL_QUIT: //If the user has Xed out the window
            loop = false;
            break;
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
         case SDL_MOUSEBUTTONDOWN:
            //debug("Mouse down: ", int(event.button.button));
            switch (event.button.button){
            case 1: //left click
               EntityPtr newBuilding(new Building(0, Point(mouseX, mouseY)));
               game.entities.push_back(newBuilding);
               //TODO fix sort
               //should be individually inserted rather than sorting the whole list
               game.entities.sort();
               break;
            }
            break;
         }
      }
      
      ticks = SDL_GetTicks();

      // Update state if necessary
      if (ticks - lastCalcTick >= CALC_MS){
         //debug("Tick: updating state");
         updateState();
         lastCalcTick = MIN_WAIT ? ticks : lastCalcTick + CALC_MS;
      }


      // Redraw if necessary
      if (ticks - lastDrawTick >= DRAW_MS){
         //debug("Tick: redrawing");
         drawEverything(screen, back, image, cursor, mouseX, mouseY, game);
         lastDrawTick = MIN_WAIT ? ticks : lastDrawTick + DRAW_MS;
      }

   }

   SDL_ShowCursor(SDL_ENABLE);

   //debug("Unloading Back");
   freeSurface(back);
   //debug("Unloading Image");
   freeSurface(image);
   //debug("Unloading Cursor");
   freeSurface(cursor);
   //debug("Done unloading");

}

void drawEverything(SDL_Surface *screen, SDL_Surface *back,
                    SDL_Surface *image, SDL_Surface *cursor,
                    pixels_t mouseX, pixels_t mouseY,
                    const GameData &game){
   SDL_FillRect(screen, 0, 0);
   SDL_BlitSurface(back, 0, screen, &makeRect());
   SDL_BlitSurface(image, 0, screen, &makeRect(50,50));
   

   for (std::list<EntityPtr>::const_iterator it = game.entities.begin();
        it != game.entities.end(); ++it){
      it->draw(screen, game);
   }

   //TODO use blitCursor
   SDL_BlitSurface(cursor, 0, screen, &makeRect(CURSOR_OFFSET_X + mouseX,
                                                CURSOR_OFFSET_Y + mouseY));

   debug.display();
   bool test = SDL_Flip(screen) == 0;
   assert(test);
}

void updateState(){}
