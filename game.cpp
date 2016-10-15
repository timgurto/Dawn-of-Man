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
#include "EntityType.h"
#include "Entity.h"
#include "BuildingType.h"
#include "Building.h"
#include "UI.h"

extern Debug debug;

void gameMode(){
   SDL_Surface *screen = setScreen();
   debug.setScreen(screen);

   SDL_Surface *back = loadImage(IMAGE_PATH + "back.png");
   SDL_Surface *cursor = loadImage(IMAGE_PATH + "cursor.png", GREEN);

   Point mousePos(SCREEN_WIDTH/2, SCREEN_HEIGHT/2);
   SDL_ShowCursor(SDL_DISABLE);

   //init
   GameData game;
   //TODO: move controlMode to GameData
   ControlMode controlMode = NORMAL_MODE;
   UI ui(IMAGE_PATH + INTERFACE_IMAGE_PATH + "topbar.PNG",
         IMAGE_PATH + INTERFACE_IMAGE_PATH + "bottombar.PNG",
         GREEN);

   typeNum_t toBuild = 0;

   //TODO: load from files
   BuildingType campfire(0, "Campfire",
                         makeRect(-61, -118, 114, 143),
                         makeRect(-61, -15, 111, 40),
                         GREEN);
   game.buildingTypes.push_back(campfire);
   BuildingType shrine(1, "Shrine",
                         makeRect(-76, -67, 154, 79),
                         makeRect(-76, -12, 154, 23),
                         GREEN);
   game.buildingTypes.push_back(shrine);



   timer_t ticks = SDL_GetTicks();
   timer_t lastCalcTick = ticks, lastDrawTick = ticks;
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
            mousePos.x = event.motion.x;
            mousePos.y = event.motion.y;
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
               if (pointCollision(mousePos, ui.bottomBarRect(game, controlMode))){
                  debug("Clicked the bottom bar");
                  switch(controlMode){
                  case NORMAL_MODE:
                     //Clicking on a building to construct
                     toBuild = (mousePos.y- 
                               ui.bottomBarRect(game, controlMode).y - UI_BAR_PADDING) /
                               ICON_SIZE;
                     debug("Selected ", game.buildingTypes[toBuild].name_);
                  }
               }
               Entity *newBuilding = new Building(1, mousePos);
               assert(newBuilding != 0);
               addEntity(game, newBuilding);
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
         drawEverything(screen, back, cursor, mousePos, game, controlMode, ui);
         lastDrawTick = MIN_WAIT ? ticks : lastDrawTick + DRAW_MS;
      }

   }

   SDL_ShowCursor(SDL_ENABLE);

   //debug("Unloading Back");
   freeSurface(back);
   //debug("Unloading Cursor");
   freeSurface(cursor);
   //debug("Done unloading");

}

void drawEverything(SDL_Surface *screen, SDL_Surface *back,
                    SDL_Surface *cursor,
                    const Point &mousePos, const GameData &game,
                    ControlMode controlMode, const UI &ui){

   //Background
   SDL_FillRect(screen, 0, 0);
   SDL_BlitSurface(back, 0, screen, &makeRect());
   
   //Entities
   for (entities_t::const_iterator it = game.entities.begin();
        it != game.entities.end(); ++it){
      (*it)->draw(screen, game);
   }

   //Interface
   ui.draw(screen, game, controlMode);

   //Cursor
   blitCursor(cursor, screen, mousePos);

   //Debug text
   debug.display();

   //Finalize
   bool test = SDL_Flip(screen) == 0;
   assert(test);
}

void updateState(){}

void addEntity(GameData &game, Entity *entity){

   game.entities.insert(std::lower_bound(game.entities.begin(),
                                         game.entities.end(), entity,
                                         dereferenceLessThan),
                        entity);
}

void removeEntity(){
   //delete() the Entity*
}

void blitCursor (SDL_Surface *cursor, SDL_Surface *screen, const Point &coords){
   SDL_BlitSurface(cursor, 0, screen, &makeRect(coords+CURSOR_OFFSET));
}