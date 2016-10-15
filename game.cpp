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
#include "UIBar.h"

extern Debug debug;

void gameMode(){
   SDL_Surface *screen = setScreen();
   debug.setScreen(screen);

   SDL_Surface *back = loadImage(IMAGE_PATH + "back.png");
   SDL_Surface *cursor = loadImage(IMAGE_PATH + "cursor.png", GREEN);
   SDL_Surface *vBar = loadImage(IMAGE_PATH + "vBar.PNG");
   SDL_Surface *hBar = loadImage(IMAGE_PATH + "hBar.PNG");
   SDL_Surface *entitiesTemp = SDL_CreateRGBSurface(SDL_HWSURFACE,
                                                    SCREEN_WIDTH,
                                                    SCREEN_HEIGHT,
                                                    SCREEN_BPP, 0, 0, 0, 0);
   SDL_SetColorKey(entitiesTemp, SDL_SRCCOLORKEY,
                   SDL_MapRGB(entitiesTemp->format,
                              ENTITY_BACKGROUND.r,
                              ENTITY_BACKGROUND.g,
                              ENTITY_BACKGROUND.b));
   Point mousePos(SCREEN_WIDTH/2, SCREEN_HEIGHT/2);

   SDL_ShowCursor(SDL_DISABLE);

   //init
   GameData game;
   Entity::setGame(&game);
   game.mode = NORMAL_MODE;

   typeNum_t toBuild = NO_TYPE;

   //TODO: load from files
   BuildingType campfire(0, "Campfire",
                         makeRect(-42, -92, 81, 113),
                         //makeRect(-42, -92, 81, 113),
                         makeRect(-42, -22, 81, 42),
                         1250);
   game.buildingTypes.push_back(campfire);
   BuildingType shrine(1, "Shrine",
                         makeRect(-76, -67, 154, 79),
                         //makeRect(-76, -67, 154, 79),
                         makeRect(-76, -17, 154, 28),
                         1750);
   game.buildingTypes.push_back(shrine);

   UIBars_t bars;
   UIBar::set(&game, vBar, hBar);
   UIBar buildingsBar(BOTTOM_LEFT, VERTICAL,
                      &getBuildingTypeIcons,
                      game.buildingTypes.size(), NORMAL_MODE);
   bars.push_back(&buildingsBar);
   

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

         case SDL_QUIT:
            loop = false;
            break;

         case SDL_MOUSEMOTION:
            mousePos.x = event.motion.x;
            mousePos.y = event.motion.y;
            break;

         case SDL_KEYDOWN:
            switch (event.key.keysym.sym){
            case SDLK_PRINT:
               {std::ostringstream os;
               os << "shot" << time(0) << ".bmp";
               SDL_SaveBMP(screen, os.str().c_str());}
               break;
            case SDLK_ESCAPE:
               switch(game.mode){
               case BUILD_MODE:
                  game.mode = NORMAL_MODE;
                  break;
               }
               break;
            }
            break;

         case SDL_MOUSEBUTTONDOWN:
            debug("Mouse down: ", int(event.button.button));
            switch (event.button.button){
            case 1: //left click
               switch (game.mode){
               case NORMAL_MODE:
                  toBuild = buildingsBar.mouseIndex(mousePos);
                  debug("toBuild = ", toBuild);
                  if (toBuild != NO_TYPE){
                     game.mode = BUILD_MODE;
                  }
                  break;
               case BUILD_MODE:
                  //if not over any bars
                  assert (toBuild != NO_TYPE);
                  if (noCollision(game, game.buildingTypes[toBuild], mousePos)){
                     addEntity(game, new Building(toBuild, mousePos));
                     Uint8 *keyStates = SDL_GetKeyState(0);
                     if(!keyStates[SDLK_LSHIFT]){
                        game.mode = NORMAL_MODE;
                        toBuild = NO_TYPE;
                     }
                  }
               }
               break;
            case 3: //right click
               switch(game.mode){
               case BUILD_MODE:
                  game.mode = NORMAL_MODE;
                  break;
               }
            }
            break;
         }

      }
      
      ticks = SDL_GetTicks();

      // Update state if necessary
      if (ticks - lastCalcTick >= CALC_MS){
         //debug("Tick: updating state");
         updateState(game);
         lastCalcTick = MIN_WAIT ? ticks : lastCalcTick + CALC_MS;
      }


      // Redraw if necessary
      if (ticks - lastDrawTick >= DRAW_MS){
         //debug("Tick: redrawing");
         drawEverything(screen, back, cursor, entitiesTemp,
                        mousePos, game, bars, toBuild);
         lastDrawTick = MIN_WAIT ? ticks : lastDrawTick + DRAW_MS;
      }

   }

   SDL_ShowCursor(SDL_ENABLE);

   freeSurface(back);
   freeSurface(cursor);
   freeSurface(vBar);
   freeSurface(hBar);
   SDL_FreeSurface(entitiesTemp);

}

void drawEverything(SDL_Surface *screen, SDL_Surface *back,
                    SDL_Surface *cursor, SDL_Surface *entitiesTemp,
                    const Point &mousePos, const GameData &game,
                    const UIBars_t &bars, typeNum_t toBuild){

   //Background
   SDL_FillRect(screen, 0, 0);
   assert (back != 0);
   for (int i = 0; i != SCREEN_WIDTH / back->w + 1; ++i)
      for (int j = 0; j != SCREEN_HEIGHT / back->h + 1; ++j)
         SDL_BlitSurface(back, 0, screen,
                         &makeRect(i * back->w, j * back->h));

   //Building footprint
   if (game.mode == BUILD_MODE){
      SDL_Rect baseRect = mousePos + game.buildingTypes[toBuild].getBaseRect();
      Uint32 footprintColor;
      if (noCollision(game, game.buildingTypes[toBuild], mousePos))
         footprintColor = FOOTPRINT_COLOR_GOOD;
      else
         footprintColor = FOOTPRINT_COLOR_BAD;
      SDL_FillRect(screen, &baseRect, footprintColor);
   }
   
   //Entities
   if (ENTITY_MASKS)
      SDL_FillRect(entitiesTemp, 0, 0x00ff00);
   for (entities_t::const_iterator it = game.entities.begin();
        it != game.entities.end(); ++it){
      (*it)->draw(ENTITY_MASKS ? entitiesTemp : screen);
   }
   if (ENTITY_MASKS)
      SDL_BlitSurface(entitiesTemp, 0, screen, 0);

   //Interface
   for (UIBars_t::const_iterator it = bars.begin(); it != bars.end(); ++it){
      (*it)->draw(screen);
   }

   //Cursor
   blitCursor(cursor, screen, mousePos);

   //Debug text
   debug.display();

   //Finalize
   bool test = SDL_Flip(screen) == 0;
   assert(test);
}

void updateState(GameData &game){
   for (entities_t::iterator it = game.entities.begin();
        it != game.entities.end(); ++it){
      (*it)->tick();
   }
}

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