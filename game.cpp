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
   srand(unsigned(time(0)));

   SDL_Surface *screen = setScreen();
   debug.initScreen(screen);
   deltaLog.initScreen(screen);

   SDL_Surface
      *back = loadImage(IMAGE_PATH + "back.png"),
      *cursor = loadImage(IMAGE_PATH + "cursor.png", GREEN),
      *cursorShadow = loadImage(IMAGE_PATH +
                                "cursorShadow.PNG", GREEN),
      *vBar = loadImage(IMAGE_PATH +
                        INTERFACE_IMAGE_PATH + "vBar.PNG"),
      *hBar = loadImage(IMAGE_PATH +
                        INTERFACE_IMAGE_PATH + "hBar.PNG"),
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
      *particle = loadImage(IMAGE_PATH + "particle.PNG", GREEN),
      *particleShadow = loadImage(IMAGE_PATH +
                                  "particleShadow.PNG", GREEN);
   SDL_SetAlpha(particleShadow, SDL_SRCALPHA, SHADOW_ALPHA);
   SDL_SetAlpha(cursorShadow, SDL_SRCALPHA, SHADOW_ALPHA);
   Particle::init(screen, particle, particleShadow);

   SDL_ShowCursor(SDL_DISABLE);

   //init
   GameData game;
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
                                rand() % SCREEN_WIDTH,
                                rand() % SCREEN_HEIGHT)));
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

      //update state
      updateState(delta, game, screen, bars);

      //render
      drawEverything(screen,
                     back,
                     cursor,
                     cursorShadow,
                     entitiesTemp,
                     game,
                     bars);


   }

   SDL_ShowCursor(SDL_ENABLE);

   freeSurface(back);
   freeSurface(cursor);
   freeSurface(cursorShadow);
   freeSurface(vBar);
   freeSurface(hBar);
   freeSurface(particle);
   freeSurface(particleShadow);
   SDL_FreeSurface(entitiesTemp);

}

//TODO take delta into account when updating
void updateState(timer_t delta, GameData &game,
                 SDL_Surface *screen, UIBars_t &bars){
   deltaLog("Delta: ", delta);
   deltaLog("  FPS: ", 1000 / (delta != 0 ? delta : 1));
   handleEvents(game, screen, bars);

   //Entities
   for (entities_t::iterator it = game.entities.begin();
        it != game.entities.end(); ++it){
      (*it)->tick();
   }

   //Particles
   for (particles_t::iterator it = game.particles.begin();
        it != game.particles.end(); ++it){
      it->tick();
      if (it->expired()){
         it = game.particles.erase(it);
         if (it == game.particles.end())
            break;
      }
   }

}

void drawEverything(SDL_Surface *screen, SDL_Surface *back,
                    SDL_Surface *cursor, SDL_Surface *cursorShadow,
                    SDL_Surface *entitiesTemp,
                    const GameData &game, const UIBars_t &bars){

   //const Point &mousePos = game.mousePos;
   //const typeNum_t &toBuild = game.toBuild;

   //Background
   SDL_FillRect(screen, 0, 0);
   assert (back != 0);
   for (int i = 0; i != SCREEN_WIDTH / back->w + 1; ++i)
      for (int j = 0; j != SCREEN_HEIGHT / back->h + 1; ++j)
         SDL_BlitSurface(back, 0, screen,
                         &makeRect(i * back->w, j * back->h));

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
   
   //Entities
   if (ENTITY_MASKS)
      SDL_FillRect(entitiesTemp, 0, ENTITY_BACKGROUND_UINT);
   for (entities_t::const_iterator it = game.entities.begin();
        it != game.entities.end(); ++it)
      (*it)->draw(ENTITY_MASKS ? entitiesTemp : screen);
   if (ENTITY_MASKS)
      SDL_BlitSurface(entitiesTemp, 0, screen, 0);

   //Interface
   for (UIBars_t::const_iterator it = bars.begin(); it != bars.end(); ++it){
      (*it)->draw();
   }

   //Cursor
   blitCursor(cursor, cursorShadow, screen, game.mousePos);

   //Particles
   for (particles_t::const_iterator it = game.particles.begin();
        it != game.particles.end(); ++it)
      it->draw();

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
         }

         break;

      //A mouse button is clicked
      case SDL_MOUSEBUTTONDOWN:
         debug("Mouse down: ", int(event.button.button));
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
                            game.mousePos));

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
   SDL_BlitSurface(shadow, 0, screen, &makeRect(coords+CURSOR_OFFSET));
   SDL_BlitSurface(cursor, 0, screen, &makeRect(coords+CURSOR_OFFSET));
}