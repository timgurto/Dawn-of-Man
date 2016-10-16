// (C) 2010 Tim Gurto

#include <cassert>
#include <sstream>
#include <ctime>
#include "SDL.h"
#include "update.h"
#include "game.h"
#include "UIBar.h"
#include "Building.h"
#include "Unit.h"
#include "Debug.h"
#include "MessageBox.h"
#include "Entity.h"

extern Debug debug;

const Uint8 MOUSE_BUTTON_LEFT        = 1;
const Uint8 MOUSE_BUTTON_MIDDLE      = 2;
const Uint8 MOUSE_BUTTON_RIGHT       = 3;
const Uint8 MOUSE_BUTTON_SCROLL_UP   = 4;
const Uint8 MOUSE_BUTTON_SCROLL_DOWN = 5;

void updateState(double delta, const CoreData &core, GameData &game,
                 SDL_Surface *screen, UIBars_t &bars,
                 MessageBox &contextHelp,
                 MessageBox &resourcesBox, MessageBox &fpsDisplay){

   //Interface stuff
   handleEvents(core, game, screen, bars, contextHelp, fpsDisplay);
   scrollMap(game, delta);

   //Actual updates
   if (!game.paused){

      //Entities
         for (entities_t::iterator it = game.entities.begin();
           it != game.entities.end(); ++it){
         (*it)->tick(delta);
         VerticalMovement v = (*it)->getVerticalMovement();
         if (v != VM_NONE)
            reSort(game.entities, it, v);
      }
      Entity::emptyTrash();

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
      
      //Misc.
      resourcesBox(game.players[HUMAN_PLAYER].getResources());

   }

}

void handleEvents(const CoreData &core, GameData &game,
                  SDL_Surface *screen, UIBars_t &bars,
                  MessageBox &contextHelp, MessageBox &fpsDisplay){

   SDL_Event event;
   while (SDL_PollEvent(&event)){
      switch (event.type){



      //Window is exited
      case SDL_QUIT:
         game.loop = false;
         break;



      //Mouse is moved
      case SDL_MOUSEMOTION:
         {//new scope for overBar, index
            contextHelp("");
            game.mousePos.x = event.motion.x;
            game.mousePos.y = event.motion.y;
            //check right mouse movement
            game.rightMouse.checkDrag(game.mousePos);
            game.leftMouse.checkDrag(game.mousePos - game.map);

            //if over a UI bar
            bool overBar = false;
            typeNum_t index;
            for (UIBars_t::iterator it = bars.begin();
                 it != bars.end(); ++it)
               if ((*it)->isActive()){
                  index = (*it)->mouseIndex();
                  if (index != NO_TYPE){
                     //mousing over this bar
                     contextHelp((*it)->helpText(index));
                     overBar = true;
                     break;
                  }
               }

            if (!overBar){
               Entity *entityP = findEntity(game, false);
               if (entityP != 0)
                  contextHelp(entityP->type().getName());
            }

            switch(game.mode){
            case MODE_CONSTRUCTION:
               game.buildLocationOK =
                  noCollision(game, core.buildingTypes[game.toBuild],
                              game.mousePos - locRect(game.map));
               break;
            }
         }
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

         case SDLK_KP_PLUS:
            //HACK remove this cheat
            {
               resources_t cheatResources(Resources::getResourceCount(), 1337);
               game.players[HUMAN_PLAYER].addResources(cheatResources);
            }
            break;

         case SDLK_ESCAPE:
            //HACK remove this exit
            {
               game.loop = false;
               return;
            }

            switch(game.mode){
            //unselect all
            case MODE_BUILDING:
               game.buildingSelected->selected = false;
               game.buildingSelected = 0;
               //fall-through
            case MODE_BUILDER:
            case MODE_NORMAL:
               for (entities_t::iterator it = game.entities.begin();
                    it != game.entities.end(); ++it)
                  (*it)->selected = false;
               game.mode = MODE_NORMAL;
               break;
            case MODE_CONSTRUCTION:
               game.toBuild = NO_TYPE;
               game.mode = MODE_NORMAL;
               for (UIBars_t::iterator it = bars.begin(); it != bars.end(); ++it)
               if ((*it)->isActive())
                  (*it)->calculateRect();
               break;
            }
            break;

         case SDLK_DELETE:
            //HACK repeats loop for each deletion, to avoid invalidated iterator
            bool deleted;
            do{
               deleted = false;
               for (entities_t::iterator it = game.entities.begin();
                  it != game.entities.end(); ++it){
                     if ((*it)->selected){
                        (*it)->kill();
                        deleted = true;
                        break;
                     }
               }
            }while (deleted);
            Entity::emptyTrash();
            break;

         case SDLK_F11:
            //F11: toggle FPS display
            fpsDisplay.toggleVisibility();
            break;

         case SDLK_F4:
            //Alt+F4 = quit
            if (isKeyPressed(SDLK_LALT) || isKeyPressed(SDLK_RALT)){
               game.loop = false;
               return;
            }
            break;
         
         case SDLK_F3:
         case SDLK_PAUSE:
            //toggle pause
            game.paused = !game.paused;
            if (game.paused)
               Mix_PauseMusic();
            else
               Mix_ResumeMusic();
            break;
         }
         break;



      //A mouse button is pressed
      case SDL_MOUSEBUTTONDOWN:
         //debug("Mouse down: ", int(event.button.button));
         switch (event.button.button){
         case MOUSE_BUTTON_LEFT:
            game.leftMouse.mouseDown(game.mousePos - game.map);
            {//new scope for barClicked
               //if not clicking a button
               bool barClicked = false;
               for (UIBars_t::iterator it = bars.begin();
                    !barClicked && it != bars.end(); ++it)
                  if ((*it)->isActive())
                     if ((*it)->mouseIndex() != NO_TYPE)
                        barClicked = true;
               if (!barClicked)

                  //initialize selection box stuff
                  game.leftMouse.mouseDown(game.mousePos - game.map);
               break;
            }
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
               if (!game.leftMouse.dragging)
                  switch(game.mode){
                  case MODE_CONSTRUCTION:
                     //cancel build mode
                     game.toBuild = NO_TYPE;
                     game.mode = MODE_BUILDER;
                     for (UIBars_t::iterator it = bars.begin(); it != bars.end(); ++it)
                        if ((*it)->isActive())
                           (*it)->calculateRect();
                     break;
                  default:
                     setSelectedTargets(game);
                  }
            game.rightMouse.mouseUp();
            break;

         case MOUSE_BUTTON_LEFT:
            //check UI bars
            if (!game.leftMouse.dragging){
               bool barClicked = false; //whether a UIBar was clicked
               for (UIBars_t::iterator it = bars.begin();
                    !barClicked && it != bars.end(); ++it)
                  if ((*it)->isActive()){
                     typeNum_t index = (*it)->mouseIndex();
                     if (index != NO_TYPE){
                        (*it)->click();
                        barClicked = true;
                     }
                  }
               if (!barClicked){
                  //place new building
                  if (game.mode == MODE_CONSTRUCTION){
                     assert (game.toBuild != NO_TYPE);
                     const BuildingType &buildingType = core.buildingTypes[game.toBuild];
                     if (game.players[HUMAN_PLAYER].
                        sufficientResources(buildingType.getCost()) &&
                         game.buildLocationOK){

                        playSound(buildingType.getSound());

                        //Subtract resource cost
                        game.players[HUMAN_PLAYER].
                           subtractResources(buildingType.getCost());

                        //create new building
                        Building *newBuilding =
                           new Building(game.toBuild, game.mousePos -
                                                      game.map);
                        addEntity(game, newBuilding);

                        //assign selected builders to the construction
                        for (entities_t::iterator it = game.entities.begin();
                             it != game.entities.end(); ++it)
                           if ((*it)->classID() == ENT_UNIT){
                              Unit *unitP = (Unit *)(*it);
                              if (unitP->selected)
                                 if (unitP->isBuilder())
                                    unitP->setTarget(newBuilding);
                           }

                        //Shift key: multiple constructions
                        if(!isKeyPressed(SDLK_LSHIFT)){
                           game.mode = MODE_BUILDER;
                           game.toBuild = NO_TYPE;
                           for (UIBars_t::iterator it = bars.begin(); it != bars.end(); ++it)
                              if ((*it)->isActive())
                                 (*it)->calculateRect();
                        }
                     }else{ //not enough resources, or bad place
                        debug("Cannot construct building");
                     }
                  }else //not construction mode
                     select(game, bars);
               }// if !barClicked
            }else //if dragging
               select(game, bars);
            game.leftMouse.mouseUp();
            break;
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

void select(GameData &game, UIBars_t &bars){
   game.buildingSelected = 0;
   bool
      entitySelected = false,
      builderSelected = false;
   bool soundPlayed = false;

   //loop backwards, so objects in front have priority to be
   //selected
   for (entities_t::reverse_iterator it = game.entities.rbegin();
        it != game.entities.rend(); ++it){
      if ((*it)->selectable()){
         //unselect everything
         if (!(isKeyPressed(SDLK_LCTRL) || isKeyPressed(SDLK_LSHIFT))){
            (*it)->selected = false;
         }
         
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
               if ((*it)->selected)
                  if (!soundPlayed){
                     playSound((*it)->type().getSound());
                     soundPlayed = true;
                  }
                  //if building, set buildingSelected ptr and flag
                  if ((*it)->classID() == ENT_BUILDING)
                     game.buildingSelected = (Building *)(*it);
                  //if unit, set flag
                  else if ((*it)->classID() == ENT_UNIT){
                     const Unit &unit = (const Unit &)(**it);
                     if (unit.isBuilder())
                        builderSelected = true;
                  }
            }// if collides

         }// if single point etc.

      }// if selectable

      //exit loop if possible
      if (entitySelected && 
          !game.leftMouse.dragging &&
          (isKeyPressed(SDLK_LCTRL) || isKeyPressed(SDLK_LSHIFT)))
         break;

   }// for entities

   //if a builder is selected
   if (builderSelected)
      game.mode = MODE_BUILDER;
   //if a building is selected
   else if (game.buildingSelected != 0)
      game.mode = MODE_BUILDING;
   else
      game.mode = MODE_NORMAL;

   for (UIBars_t::iterator it = bars.begin(); it != bars.end(); ++it)
      (*it)->calculateRect();
}

void setSelectedTargets(GameData &game){
   Entity *targetEntity = findEntity(game);
   for (entities_t::iterator it = game.entities.begin();
        it != game.entities.end(); ++it){
      //only selected units have their targets set
      if ((*it)->classID() == ENT_UNIT &&
          (*it)->selected){
         Unit *unitP = (Unit *)(*it);

         //no entity
         if (targetEntity == 0){
            unitP->setTarget();
            continue;
         }

         EntityTypeID targetClass = targetEntity->classID();

         //enemy unit or building
         if ((targetClass == ENT_UNIT || targetClass == ENT_BUILDING) &&
             targetEntity->getPlayer() != unitP->getPlayer()){
            unitP->setTarget(targetEntity);
            continue;
         }

         //friendly, unfinished building
         if (targetClass == ENT_BUILDING &&
             targetEntity->getPlayer() == unitP->getPlayer() &&
             unitP->isBuilder() &&
             !((Building *)(targetEntity))->isFinished()){
            unitP->setTarget(targetEntity);
            continue;
         }

         //resource node
         if (targetClass == ENT_RESOURCE_NODE &&
             unitP->isGatherer())
            unitP->setTarget(targetEntity);
         continue;
      }
   }
}

void reSort(entities_t &entities, entities_t::iterator it,
            VerticalMovement verticalMovement){
   entities_t::iterator old = it;
   switch(verticalMovement){
   case DIR_UP:
      if (it != entities.begin()){
         entities_t::iterator next = it;
         --next;
         while (next != entities.begin() && **it < **next){
            std::iter_swap(it, next);
            it = next;
            --next;
         }
      }
   break;

   case DIR_DOWN:
      entities_t::iterator next = it;
      ++next;
      while (next != entities.end() && **next < **it){
         std::iter_swap(it, next);
         it = next;
         ++next;
      }

   }
}

Entity *findEntity(GameData &game, bool onlyTargetable){
   //loop backwards, so objects in front have priority
   for (entities_t::reverse_iterator it = game.entities.rbegin();
        it != game.entities.rend(); ++it){

      //filtering
      //only targetable entities
      if (onlyTargetable && !(*it)->targetable())
         continue;

      if (collision((*it)->getDrawRect(),
                    game.mousePos - Point(game.map)))
         return *it;
   }
   return 0;
}