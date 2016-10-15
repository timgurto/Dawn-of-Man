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
      //TODO engrave bar images
      *vBar = loadImage(INTERFACE_IMAGE_PATH + "vBar.PNG"),
      *hBar = loadImage(INTERFACE_IMAGE_PATH + "hBar.PNG"),
      *diagGreen = loadImage(MISC_IMAGE_PATH + "diagGreen.PNG", GREEN),
      *diagRed = loadImage(MISC_IMAGE_PATH + "diagRed.PNG", GREEN),
      *particle = loadImage(MISC_IMAGE_PATH + "particle.PNG", GREEN),
      *particleShadow = loadImage(MISC_IMAGE_PATH +
                                  "particleShadow.PNG", GREEN),
      *entitiesTemp = createSurface(),
      *selection = loadImage(MISC_IMAGE_PATH + "selection.PNG");
   SDL_SetColorKey(entitiesTemp, SDL_SRCCOLORKEY,
                   SDL_MapRGB(entitiesTemp->format,
                              ENTITY_BACKGROUND.r,
                              ENTITY_BACKGROUND.g,
                              ENTITY_BACKGROUND.b));
   SDL_SetAlpha(particleShadow, SDL_SRCALPHA, SHADOW_ALPHA);
   SDL_SetAlpha(cursorShadow, SDL_SRCALPHA, SHADOW_ALPHA);

   SDL_ShowCursor(SDL_DISABLE);

   //init
   GameData game(4, 3);
   Entity::init(&game, screen);
   Particle::init(screen, particle, particleShadow);
   game.mode = NORMAL_MODE;

   //TODO load from files
   //=================================================
   BuildingType campfire(0, "Campfire",
                         makeRect(-42, -92, 81, 113),
                         makeRect(-42, -23, 81, 42),
                         1250);
   game.buildingTypes.push_back(campfire);
   BuildingType shrine(1, "Shrine",
                       makeRect(-76, -68, 154, 79),
                       makeRect(-76, -17, 154, 28),
                       1750);
   game.buildingTypes.push_back(shrine);
   DecorationType rock(0, "Rock",
                  makeRect(-12, -9, 24, 18),
                  makeRect(-12, -9, 24, 18));
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
      
      //TODO some means of displaying FPS in Release
      deltaLog("    Framerate: ", (delta == 0 ? 0 : 1000 / delta));
      deltaLog("        Delta: ", delta, "ms");

      //update state
      updateState(deltaMod, game, screen, bars);

      //render
      render(screen, diagGreen, diagRed, map, darkMap, cursor,
             cursorShadow, entitiesTemp, game, bars);

   }

   //Clean up
   SDL_ShowCursor(SDL_ENABLE);

   freeSurface(map);
   freeSurface(darkMap);
   freeSurface(cursor);
   freeSurface(cursorShadow);
   freeSurface(vBar);
   freeSurface(hBar);
   freeSurface(particle);
   freeSurface(particleShadow);
   freeSurface(diagGreen);
   freeSurface(diagRed);
   freeSurface(entitiesTemp);
   freeSurface(selection);

}

void updateState(double delta, GameData &game,
                 SDL_Surface *screen, UIBars_t &bars){

   //handle user input since last tick
   handleEvents(game, screen, bars);

   //Map scrolling
   scrollMap(game, delta);

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
         //debug("particle expired");
         it = game.particles.erase(it);
         if (it == game.particles.end())
            break;
      }
   }

}

void render(SDL_Surface *screen,
            SDL_Surface *diagGreen, SDL_Surface *diagRed,
            SDL_Surface *map, SDL_Surface *darkMap,
            SDL_Surface *cursor, SDL_Surface *cursorShadow,
            SDL_Surface *entitiesTemp,
            const GameData &game, const UIBars_t &bars){

   //Map
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

   //Building footprint
   if (game.mode == BUILD_MODE &&
       !game.rightMouse.dragging){
      SDL_Rect baseRect = game.mousePos +
                          game.buildingTypes[game.toBuild].getBaseRect();
      if (game.buildLocationOK)
         SDL_BlitSurface(diagGreen, &dimRect(baseRect), screen, &SDL_Rect(baseRect));
      else
         SDL_BlitSurface(diagRed, &dimRect(baseRect), screen, &SDL_Rect(baseRect));
   }

   //Entities
   if (ENTITY_MASKS)
      SDL_FillRect(entitiesTemp, 0, ENTITY_BACKGROUND_UINT);
   for (entities_t::const_iterator it = game.entities.begin();
        it != game.entities.end(); ++it)
      //only draw entities that are on-screen
      if ((*it)->onScreen()){
         if (!(*it)->selected) (*it)->draw(ENTITY_MASKS ? entitiesTemp : screen);
      }
   if (ENTITY_MASKS)
      SDL_BlitSurface(entitiesTemp, 0, screen, 0);

   //Selection rectangle
   if (game.mode != BUILD_MODE &&
       game.leftMouse.dragging){
      
      //Get rectangle
      SDL_Rect selRect = getSelectionRect(game);
      
      //Draw rectangle
      SDL_Surface *temp = createSurface(selRect.w, selRect.h);
      SDL_SetAlpha(temp, SDL_SRCALPHA, SELECTION_RECT_ALPHA);
      SDL_FillRect(temp, &dimRect(selRect), WHITE_UINT);
      SDL_BlitSurface(temp, 0, screen, &selRect);
      freeSurface(temp);
   }

   //Interface
   for (UIBars_t::const_iterator it = bars.begin(); it != bars.end(); ++it){
      (*it)->draw();
   }

   //Cursor
   blitCursor(cursor, cursorShadow, screen, game);

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
         //check right mouse movement
         game.rightMouse.checkDrag(game.mousePos);
         game.leftMouse.checkDrag(game.mousePos - game.map);
         switch(game.mode)
         case BUILD_MODE:
            game.buildLocationOK =
               noCollision(game, game.buildingTypes[game.toBuild],
                           game.mousePos);
            break;
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



      //A mouse button is pressed
      case SDL_MOUSEBUTTONDOWN:
         //debug("Mouse down: ", int(event.button.button));
         switch (event.button.button){
         case MOUSE_BUTTON_LEFT:
            {//new scope for barClicked
               //if not clicking a button
               bool barClicked = false;
               for (UIBars_t::iterator it = bars.begin();
                    !barClicked && it != bars.end(); ++it)
                  if ((*it)->isActive(game.mode))
                     if ((*it)->mouseIndex(game.mousePos) != NO_TYPE)
                        barClicked = true;
               if (!barClicked)

                  //initialize selection box stuff
                  game.leftMouse.mouseDown(game.mousePos - game.map);
            }
            break;
         case MOUSE_BUTTON_RIGHT:
            //initialize right-drag scroll stuff
            game.rightMouse.mouseDown(game.mousePos);
            break;
         }// switch mouse button
         break;



      //A mouse button is released
      case SDL_MOUSEBUTTONUP:

         switch (event.button.button){
         case MOUSE_BUTTON_RIGHT:
            if (!game.rightMouse.dragging)
               switch(game.mode){
               case BUILD_MODE:
                  //cancel build mode
                  game.mode = NORMAL_MODE;
                  break;
               }
            game.rightMouse.mouseUp();
            break;
         case MOUSE_BUTTON_LEFT:
            bool barClicked = false; //whether a UIBar was clicked
            //check UI bars
            if (!game.leftMouse.dragging){
               for (UIBars_t::iterator it = bars.begin();
                    !barClicked && it != bars.end(); ++it)
                  if ((*it)->isActive(game.mode)){
                     typeNum_t index = (*it)->mouseIndex(game.mousePos);
                     if (index != NO_TYPE){
                        (*it)->clickFun(index, game);
                        barClicked = true;
                     }
                  }
            }

            if (!barClicked){
               //place new building
               if (game.mode == BUILD_MODE){
                  assert (game.toBuild != NO_TYPE);
                  if (game.buildLocationOK){
                     addEntity(game, new Building(game.toBuild,
                               game.mousePos - game.map));

                     //Shift key: construct multiple buildings
                     Uint8 *keyStates = SDL_GetKeyState(0);
                     if(!keyStates[SDLK_LSHIFT]){
                        game.mode = NORMAL_MODE;
                        game.toBuild = NO_TYPE;
                     }
                  }
               }else
                  select(game);
               game.leftMouse.mouseUp();
            }// if barClicked

         }
         break;

      } //event switch
   } //event while
}

void blitCursor (SDL_Surface *cursor, SDL_Surface *shadow,
                 SDL_Surface *screen, const GameData &game){
   //cursor might appear 'raised' from the wall
   bool raised = game.rightMouse.dragging/* ||
                 (game.mode == BUILD_MODE && !game.buildLocationOK)*/;

   Point
      cursorPos = game.mousePos + CURSOR_OFFSET,
      shadowPos = cursorPos;
   if (raised){
      cursorPos -= CURSOR_RAISED;
      shadowPos += CURSOR_RAISED;
   }
   SDL_BlitSurface(shadow, 0, screen, &makeRect(shadowPos));
   SDL_BlitSurface(cursor, 0, screen, &makeRect(cursorPos));
}

void addEntity(GameData &game, Entity *entity){
   game.entities.insert(std::lower_bound(game.entities.begin(),
                                         game.entities.end(), entity,
                                         dereferenceLessThan),
                        entity);
}

void removeEntity(entities_t::iterator it){
   //delete() the Entity*
}

void scrollMap(GameData &game, double delta){

   //right-dragging
   if (game.rightMouse.dragging){
      Point rmbDisplacement = game.mousePos - game.rightMouse.dragBegin;
      game.map = game.map - rmbDisplacement * (delta *
                                               RMB_SCROLL_MULTIPLIER);
   }

   pixels_t scroll = pixels_t(delta * SCROLL_AMOUNT);

   //edge of screen
   if (game.mousePos.x < EDGE_SCROLL_MARGIN)
      game.map.x += scroll;
   else if (game.mousePos.x > SCREEN_WIDTH - EDGE_SCROLL_MARGIN)
      game.map.x -= scroll;
   if (game.mousePos.y < EDGE_SCROLL_MARGIN)
      game.map.y += scroll;
   else if (game.mousePos.y > SCREEN_HEIGHT - EDGE_SCROLL_MARGIN)
      game.map.y -= scroll;

   //arrow keys
   Uint8 *keyStates = SDL_GetKeyState(0);
   if (keyStates[SDLK_UP])
      game.map.y += scroll;
   if (keyStates[SDLK_DOWN])
      game.map.y -= scroll;
   if (keyStates[SDLK_LEFT])
      game.map.x += scroll;
   if (keyStates[SDLK_RIGHT])
      game.map.x -= scroll;

   //Enforce scroll boundaries
   if (game.map.x > SCROLL_MARGIN)
      game.map.x = SCROLL_MARGIN;
   if (game.map.x + game.map.w < SCREEN_WIDTH - SCROLL_MARGIN)
      game.map.x = SCREEN_WIDTH - SCROLL_MARGIN - game.map.w;
   if (game.map.y > SCROLL_MARGIN)
      game.map.y = SCROLL_MARGIN;
   if (game.map.y + game.map.h < SCREEN_HEIGHT - SCROLL_MARGIN)
      game.map.y = SCREEN_HEIGHT - SCROLL_MARGIN - game.map.h;
}

SDL_Rect getSelectionRect(const GameData &game){
   const SDL_Rect &map = game.map;
   const MouseButton &leftMouse = game.leftMouse;
   Point modMouse = game.mousePos - map;
   SDL_Rect selRect;

   if (leftMouse.dragBegin.x < modMouse.x){
      selRect.x = leftMouse.dragBegin.x;
      selRect.w = modMouse.x - leftMouse.dragBegin.x;
   }else{
      selRect.x = modMouse.x;
      selRect.w = leftMouse.dragBegin.x - modMouse.x;
   }
   if (leftMouse.dragBegin.y < modMouse.y){
      selRect.y = leftMouse.dragBegin.y;
      selRect.h = modMouse.y - leftMouse.dragBegin.y;
   }else{
      selRect.y = modMouse.y;
      selRect.h = leftMouse.dragBegin.y - modMouse.y;
   }
   return selRect + map;
}

void select(GameData &game){
   Uint8 *keyStates = SDL_GetKeyState(0);
   bool entitySelected = false;
   for (entities_t::iterator it = game.entities.begin();
        it != game.entities.end(); ++it){
      if ((*it)->selectable()){
         //unselect everything
         if (!keyStates[SDLK_LCTRL])
            (*it)->selected = false;

         //determine collision
         bool collides;
         if (game.leftMouse.dragging) //selection box
            //collision(SDL_Rect, SDL_Rect)
            collides = collision((*it)->getDrawRect(),
                                 getSelectionRect(game) -
                                 Point(game.map));
         else                         //single point
            //collision(SDL_Rect, Point)
            collides = collision((*it)->getDrawRect(),
                                 game.mousePos - Point(game.map));

         if (collides){
            if (keyStates[SDLK_LCTRL])
               (*it)->toggleSelect(); //Ctrl: toggle
            else
               (*it)->selected = true; //No ctrl: select
            entitySelected = true;
            if (!game.leftMouse.dragging) //if single point click
               break;
         } //if collides
      } // if selectable
   } // for entities
   if (entitySelected) debug("Something selected");
}