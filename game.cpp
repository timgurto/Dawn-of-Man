// (C) 2009-2010 Tim Gurto

#include <cassert>
#include <cstdlib>
#include <vector>
#include <list>
#include <sstream>
#include <time.h>
#include <algorithm>

#include "SDL.h"
#include "globals.h"
#include "game.h"
#include "misc.h"
#include "uiBarFunctions.h"
#include "Debug.h"
#include "GameData.h"
#include "EntityType.h"
#include "Entity.h"
#include "BuildingType.h"
#include "Building.h"
#include "DecorationType.h"
#include "Decoration.h"
#include "UIBar.h"
#include "Particle.h"

extern Debug debug;
extern Debug deltaLog;

void gameMode(){

   //initialize screen and debug objects
   SDL_Surface *screen = setScreen();
   debug.initScreen(screen);
   deltaLog.initScreen(screen);

   //seed random generator
   srand(unsigned(time(0)));

   SDL_Surface
      *map = loadImage(MISC_IMAGE_PATH + "back.png"),
      *darkMap = loadImage(MISC_IMAGE_PATH + "dark.png"),
      *cursor = loadImage(MISC_IMAGE_PATH + "cursor.png", GREEN),
      *cursorShadow = loadImage(MISC_IMAGE_PATH +
                                "cursorShadow.PNG", GREEN),
      *vBar = loadImage(INTERFACE_IMAGE_PATH + "vBar.PNG"),
      *hBar = loadImage(INTERFACE_IMAGE_PATH + "hBar.PNG"),
      *entitiesTemp = SDL_CreateRGBSurface(SDL_HWSURFACE,
                                           SCREEN_WIDTH,
                                           SCREEN_HEIGHT,
                                           SCREEN_BPP,
                                           0, 0, 0, 0);
   SDL_SetColorKey(entitiesTemp, SDL_SRCCOLORKEY,
                   SDL_MapRGB(entitiesTemp->format,
                              ENTITY_BACKGROUND.r,
                              ENTITY_BACKGROUND.g,
                              ENTITY_BACKGROUND.b));
   SDL_Surface
      *particle = loadImage(MISC_IMAGE_PATH + "particle.PNG", GREEN),
      *particleShadow = loadImage(MISC_IMAGE_PATH +
                                  "particleShadow.PNG", GREEN);
   SDL_SetAlpha(particleShadow, SDL_SRCALPHA, SHADOW_ALPHA);
   SDL_SetAlpha(cursorShadow, SDL_SRCALPHA, SHADOW_ALPHA);
   Particle::init(screen, particle, particleShadow);

   SDL_ShowCursor(SDL_DISABLE);

   //init
   GameData game(1, 1);
   Entity::init(&game, screen);
   game.mode = NORMAL_MODE;

   //TODO load from files
   //=================================================
   BuildingType campfire(0, "Campfire",
                         makeRect(-41, -91, 79, 111),
                         makeRect(-41, -22, 79, 40),
                         1250);
   game.buildingTypes.push_back(campfire);
   BuildingType shrine(1, "Shrine",
                       makeRect(-75, -67, 152, 77),
                       makeRect(-75, -16, 152, 26),
                       1750);
   game.buildingTypes.push_back(shrine);
   DecorationType rock(0, "Rock",
                  makeRect(-11, -8, 22, 16),
                  makeRect(-11, -8, 22, 16));
   game.decorationTypes.push_back(rock);
   for (int i = 0; i != 15; ++i)
      addEntity(game, new Decoration(0, Point(
                                rand() % game.map.w,
                                rand() % game.map.h)));
   game.players.push_back(0xca6500);
   //=================================================

   UIBars_t bars;
   UIBar::init(&game, screen, vBar, hBar);
   UIBar buildingsBar(BOTTOM_LEFT, VERTICAL,
                      &getBuildingTypeIcons,
                      &selectBuilding,
                      game.buildingTypes.size(), NORMAL_MODE);
   bars.push_back(&buildingsBar);
   

   timer_t oldTicks = SDL_GetTicks();
   game.loop = true;
   while (game.loop){

      //time stuff
      timer_t newTicks = SDL_GetTicks();
      timer_t delta = newTicks - oldTicks;
      oldTicks = newTicks;
      double deltaMod = 1.0 * delta / DELTA_MODIFIER;
      
      deltaLog("       Framerate: ", (delta == 0 ? 0 : 1000 / delta));
      deltaLog("           Delta: ", delta, "ms");
      deltaLog("Delta per entity: ", 0.01 * (100 * delta / game.entities.size()));

      //update state
      updateState(deltaMod, game, screen, bars);

      //render
      drawEverything(screen, map, darkMap, cursor, cursorShadow,
                     entitiesTemp, game, bars);

   }

   SDL_ShowCursor(SDL_ENABLE);

   freeSurface(map);
   freeSurface(darkMap);
   freeSurface(cursor);
   freeSurface(cursorShadow);
   freeSurface(vBar);
   freeSurface(hBar);
   freeSurface(particle);
   freeSurface(particleShadow);
   SDL_FreeSurface(entitiesTemp);

}

void updateState(double delta, GameData &game,
                 SDL_Surface *screen, UIBars_t &bars){
   
   handleEvents(game, screen, bars);

   //Map scrolling
   //TODO limit displacement
   Uint8 *keyStates = SDL_GetKeyState(0);
   if (keyStates[SDLK_UP])
      game.map.y += SCROLL_AMOUNT;
   if (keyStates[SDLK_DOWN])
      game.map.y -= SCROLL_AMOUNT;
   if (keyStates[SDLK_LEFT])
      game.map.x += SCROLL_AMOUNT;
   if (keyStates[SDLK_RIGHT])
      game.map.x -= SCROLL_AMOUNT;


   //Entities
   for (entities_t::iterator it = game.entities.begin();
        it != game.entities.end(); ++it){
      (*it)->tick(delta);
   }

   //Particles
   for (particles_t::iterator it = game.particles.begin();
        it != game.particles.end(); ++it){
      it->tick(delta);
      if (it->expired()){
         it = game.particles.erase(it);
         if (it == game.particles.end())
            break;
      }
   }

}

void drawEverything(SDL_Surface *screen,
                    SDL_Surface *map, SDL_Surface *darkMap,
                    SDL_Surface *cursor, SDL_Surface *cursorShadow,
                    SDL_Surface *entitiesTemp,
                    const GameData &game, const UIBars_t &bars){

   //const Point &mousePos = game.mousePos;
   //const typeNum_t &toBuild = game.toBuild;

   //Map
   SDL_FillRect(screen, 0, 0);

   for (int x = -1; x != game.mapX + 1; ++x)
      for (int y = -1; y != game.mapY + 1; ++y){
         SDL_Rect tileRect = makeRect(game.map.x + x * MAP_TILE_SIZE,
                                      game.map.y + y * MAP_TILE_SIZE,
                                      MAP_TILE_SIZE,
                                      MAP_TILE_SIZE);
         //if tile is inside the screen
         if (collision(tileRect, screen->clip_rect)){

            //which image to blit
            SDL_Surface *tile = map;
            if (x == -1 || x == game.mapX ||
                y == -1 || y == game.mapY)
               //outside the map
               tile = darkMap;

            SDL_BlitSurface(tile, 0, screen, &tileRect);
         }
      }

   
   //Entities
   if (ENTITY_MASKS)
      SDL_FillRect(entitiesTemp, 0, ENTITY_BACKGROUND_UINT);
   for (entities_t::const_iterator it = game.entities.begin();
        it != game.entities.end(); ++it)
      (*it)->draw(ENTITY_MASKS ? entitiesTemp : screen);
   if (ENTITY_MASKS)
      SDL_BlitSurface(entitiesTemp, 0, screen, 0);

   //Building footprint
   if (game.mode == BUILD_MODE){
      SDL_Rect baseRect = game.mousePos +
                          game.buildingTypes[game.toBuild].getBaseRect();
      Uint32 footprintColor;
      if (noCollision(game, game.buildingTypes[game.toBuild], game.mousePos))
         footprintColor = FOOTPRINT_COLOR_GOOD;
      else
         footprintColor = FOOTPRINT_COLOR_BAD;
      SDL_FillRect(screen, &baseRect, footprintColor);
   }

   //Interface
   for (UIBars_t::const_iterator it = bars.begin(); it != bars.end(); ++it){
      (*it)->draw();
   }

   //Cursor
   blitCursor(cursor, cursorShadow, screen, game.mousePos);

   //Particles
   for (particles_t::const_iterator it = game.particles.begin();
        it != game.particles.end(); ++it)
      it->draw(game);

   //Debug text
   debug.display();
   deltaLog.display();

   //Finalize
   bool test = SDL_Flip(screen) == 0;
   assert(test);
}

void handleEvents(GameData &game, SDL_Surface *screen, UIBars_t &bars){

   //bool &loop = game.loop;
   //Point &mousePos = game.mousePos;
   //typeNum_t &toBuild = game.toBuild;

   SDL_Event event;
   while (SDL_PollEvent(&event)){
      switch (event.type){

      //Window is exited
      case SDL_QUIT:
         game.loop = false;
         break;

      //Mouse is moved
      case SDL_MOUSEMOTION:
         game.mousePos.x = event.motion.x;
         game.mousePos.y = event.motion.y;
         break;

      //A key is pressed
      case SDL_KEYDOWN:
         switch (event.key.keysym.sym){
         case SDLK_PRINT:
            { //new scope for os
               std::ostringstream os;
               os << "shot" << time(0) << ".bmp";
               SDL_SaveBMP(screen, os.str().c_str());
            }
            break;
         case SDLK_ESCAPE:
            switch(game.mode){
            case NORMAL_MODE:
               game.loop = false;
               break;
            case BUILD_MODE:
               game.mode = NORMAL_MODE;
               break;
            }
            break;
         case SDLK_F11:
            //F11: toggle FPS display
            deltaLog.enabled = ! deltaLog.enabled;
            break;
         }
         break;

      //A mouse button is clicked
      case SDL_MOUSEBUTTONDOWN:
         //debug("Mouse down: ", int(event.button.button));
         switch (event.button.button){
         case MOUSE_BUTTON_LEFT:
            { //new scope for barClicked
               //check UI bars
               bool barClicked = false;
               for (UIBars_t::iterator it = bars.begin();
                    !barClicked && it != bars.end(); ++it)
                  if ((*it)->isActive(game.mode)){
                     typeNum_t index = (*it)->mouseIndex(game.mousePos);
                     if (index != NO_TYPE){
                        (*it)->clickFun(index, game);
                        barClicked = true;
                     }
                  }
               if (barClicked)
                  break;
            }

            switch (game.mode){
            case BUILD_MODE:
               assert (game.toBuild != NO_TYPE);
               if (noCollision(game, game.buildingTypes[game.toBuild],
                               game.mousePos)){
                  addEntity(game, new Building(game.toBuild,
                            game.mousePos - game.map));

                  //Shift key: construct multiple buildings
                  Uint8 *keyStates = SDL_GetKeyState(0);
                  if(!keyStates[SDLK_LSHIFT]){
                     game.mode = NORMAL_MODE;
                     game.toBuild = NO_TYPE;
                  }
               }
            }
            break;
         case MOUSE_BUTTON_RIGHT:
            switch(game.mode){
            case BUILD_MODE:
               game.mode = NORMAL_MODE;
               break;
            }
         }
         break;
      } //event switch
   } //event while
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

void blitCursor (SDL_Surface *cursor, SDL_Surface *shadow,
                 SDL_Surface *screen, const Point &coords){
   if (SHADOWS)
      SDL_BlitSurface(shadow, 0, screen, &makeRect(coords+CURSOR_OFFSET));
   SDL_BlitSurface(cursor, 0, screen, &makeRect(coords+CURSOR_OFFSET));
}