// (C) 2010 Tim Gurto

#include <cassert>
#include <sstream>
#include <ctime>
#include "update.h"
#include "game.h"
#include "UIBar.h"
#include "Building.h"
#include "Unit.h"

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
      VerticalMovement v = (*it)->getVerticalMovement();
      if (v != VM_NONE)
         reSort(game.entities, it, v);
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
               case NORMAL_MODE:
                  if (!game.leftMouse.dragging)
                     setSelectedTargets(game);
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
                        (*it)->click(index);
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
                     if(!isKeyPressed(SDLK_LSHIFT)){
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

   //four SDL calls every tick... might be better to calculate once,
   //or maybe keep a static pointer around.
   //arrow keys
   if (isKeyPressed(SDLK_UP))
      game.map.y += scroll;
   if (isKeyPressed(SDLK_DOWN))
      game.map.y -= scroll;
   if (isKeyPressed(SDLK_LEFT))
      game.map.x += scroll;
   if (isKeyPressed(SDLK_RIGHT))
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
   bool entitySelected = false;
   //loop backwards, so objects in front have priority to be
   //selected
   for (entities_t::reverse_iterator it = game.entities.rbegin();
        it != game.entities.rend(); ++it){
      if ((*it)->selectable()){
         //unselect everything
         if (!(isKeyPressed(SDLK_LCTRL) || isKeyPressed(SDLK_LSHIFT)))
            (*it)->selected = false;
         
         //if single point click, don't waste time looking for
         //multiple selections
         if (!entitySelected || game.leftMouse.dragging){

            //determine collision
            bool collides;
            if (game.leftMouse.dragging)
               //selection box: collision(SDL_Rect, SDL_Rect)
               collides = collision((*it)->getDrawRect(),
                                    getSelectionRect(game) -
                                    locRect(game.map));
            else
               //single point: collision(SDL_Rect, Point)
               collides = collision((*it)->getDrawRect(),
                                    game.mousePos - 
                                    Point(game.map));

            if (collides){
               if (isKeyPressed(SDLK_LCTRL))
                  (*it)->toggleSelect(); //Ctrl: toggle
               else
                  (*it)->selected = true; //No ctrl: select
               entitySelected = true;
            }// if collides

         }// if single point etc.

      }// if selectable

      //exit loop if possible
      if (entitySelected && 
          !game.leftMouse.dragging &&
          (isKeyPressed(SDLK_LCTRL) || isKeyPressed(SDLK_LSHIFT)))
         break;

   }// for entities
}

void setSelectedTargets(GameData &game){
   for (entities_t::iterator it = game.entities.begin();
        it != game.entities.end(); ++it){
      if ((*it)->classID() == UNIT &&
          (*it)->selected){
         Unit *unitP = (Unit *)(*it); //change to Unit*
         unitP->setTarget();
      }
   }
}

void reSort(entities_t &entities, entities_t::iterator it,
            VerticalMovement verticalMovement){
   entities_t::iterator old = it;
   switch(verticalMovement){
   case UP:
      if (it != entities.begin()){
         entities_t::iterator next = it;
         --next;
         while (it != entities.begin() && **it < **next){
            std::iter_swap(it, next);
            it = next;
            --next;
         }
      }

   case DOWN:
      entities_t::iterator next = it;
      ++next;
      while (next != entities.end() && **next < **it){
         std::iter_swap(it, next);
         it = next;
         ++next;
      }

   }
}