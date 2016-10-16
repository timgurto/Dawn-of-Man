// (C) 2010 Tim Gurto

#include <cassert>
#include <sstream>
#include <ctime>
#include "update.h"
#include "game.h"
#include "util.h"
#include "misc.h"
#include "UIBar.h"
#include "Building.h"
#include "Unit.h"
#include "Debug.h"
#include "MessageBox.h"
#include "Entity.h"
#include "Screen.h"

extern Debug debug;

void updateState(double delta, const CoreData &core, GameData &game,
                 UIBars_t &bars, MessageBox &contextHelp,
                 MessageBox &resourcesBox, MessageBox &fpsDisplay){

   //Interface stuff
   handleEvents(core, game, bars, contextHelp, fpsDisplay);
   scrollMap(game, delta);

   if (game.selectionChanged)
      setModeFromSelection(game, bars);

   //Actual updates
   if (!game.paused){

      //Entities
         for (entities_t::iterator it = game.entities.begin();
           it != game.entities.end(); ++it){
         (*it)->tick(delta);

         //re-sort entity if it has moved vertically
         VerticalMovement v = (*it)->getVerticalMovement();
         if (v != VM_NONE)
            reSort(game.entities, it, v);
      }
      Entity::emptyTrash();

      //TODO tick non-human, non-nature Players

      //UI bars, if necessary
      if (game.recalcBars){
         game.recalcBars = false;
         for (UIBars_t::iterator it = bars.begin(); it != bars.end(); ++it)
            if ((*it)->isActive())
               (*it)->calculateRect();
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
      
      //Misc.
      resourcesBox(game.players[HUMAN_PLAYER].getResources());

   }

}

void handleEvents(const CoreData &core, GameData &game,
                  UIBars_t &bars,
                  MessageBox &contextHelp, MessageBox &fpsDisplay){
   SDL_Event event;
   while (SDL_PollEvent(&event))
      switch (event.type){



      //Window is exited
      case SDL_QUIT:
         game.loop = false;
         game.outcome = QUIT;
         break;



      //Mouse is moved
      case SDL_MOUSEMOTION:
         { //new scope for overBar, index
            game.cursorColor = CLR_MAX;
            contextHelp("");
            Screen::mousePos.x = event.motion.x;
            Screen::mousePos.y = event.motion.y;
            //check right mouse movement
            if (!game.rightMouse.dragging)
               game.rightMouse.checkDrag(Screen::mousePos);
            if (!game.leftMouse.dragging)
               game.leftMouse.checkDrag(Screen::mousePos - game.map);

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
               if (entityP){
                  contextHelp(entityP->getHelp());
                  game.cursorColor = entityP->getColor();
               }else
                  game.cursorColor = CLR_MAX; //no color
            }

            switch(game.mode){
            case MODE_CONSTRUCTION:
               game.buildLocationOK =
                  noCollision(game, core.buildingTypes[game.toBuild],
                              Screen::mousePos - locRect(game.map));
               break;
            }
         }
         break;



         //A key is pressed
         case SDL_KEYDOWN:
            {//new scope for key
               SDLKey key = event.key.keysym.sym;
               switch (key){
               case SDLK_PRINT:
                  { //new scope for os
                     std::ostringstream os;
                     os << SCREENSHOTS_PATH << "shot" << time(0) << ".bmp";
                     screenBuf.saveToBitmap(os.str());
                  }
                  break;

               case SDLK_KP_PLUS:
                  if (DEBUG)
                     game.players[HUMAN_PLAYER]
                        .addResources(resources_t(Resources::getResourceCount(), 1337));
                  break;

               case SDLK_KP_MINUS:
                  //HACK remove this cheat
                  if (DEBUG)
                     game.players[HUMAN_PLAYER].godMode();
                  break;

               case SDLK_ESCAPE:
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
                  { //new scope for selected
                     //find selected entities
                     for (entities_t::iterator it = game.entities.begin();
                        it != game.entities.end(); ++it)
                        if ((*it)->selected){
                           (*it)->kill();
                           break;
                        }
                  }
                  break;

               case SDLK_F4:
                  //Alt+F4: Exit program immediately
                  if (isKeyPressed(SDLK_LALT) || isKeyPressed(SDLK_RALT)){
                     game.loop = false;
                     game.outcome = ALT_F4;
                     return;
                  }
                  break;

               case SDLK_F11:
                  //F11: toggle FPS display
                  fpsDisplay.toggleVisibility();
                  break;

               case SDLK_F9:
                  //F9: restart
                  game.loop = false;
                  game.outcome = LOSS;
                  return;

               case SDLK_F10:
                  //F10: quit
                  game.loop = false;
                  game.outcome = QUIT;
                  return;
               
               case SDLK_F3:
               case SDLK_PAUSE:
                  //toggle pause
                  game.paused = !game.paused;
                  if (game.paused)
                     Mix_PauseMusic();
                  else
                     Mix_ResumeMusic();
                  break;

               //another key
               default:
                  
                  //control groups
                  if (key >= CONTROL_MIN &&
                      key <= CONTROL_MAX ||
                      key == CONTROL_NONE)

                     //Ctrl: Set control group
                     if (isKeyPressed(SDLK_LCTRL) ||
                         isKeyPressed(SDLK_RCTRL)){
                         for (entities_t::iterator it = game.entities.begin();
                              it != game.entities.end(); ++it)
                            if ((*it)->classID() == ENT_UNIT){
                               Unit &unit = (Unit &)(**it);
                               if (unit.controlGroup == key)
                                  unit.controlGroup = CONTROL_NONE;
                               if ((*it)->selected)
                                  unit.controlGroup = ControlGroup(key);
                            }
                     
                     //No Ctrl: select control group
                     }else{
                        if (key != CONTROL_NONE){ //0 nothing to select
                           const Sound *sound = 0; //selection sound to play
                           for (entities_t::iterator it = game.entities.begin();
                                it != game.entities.end(); ++it){

                              //no Shift: unselect all
                              if (!(isKeyPressed(SDLK_LSHIFT) ||
                                    isKeyPressed(SDLK_RSHIFT)))
                                 (*it)->selected = false;

                              //select if right control group
                              if ((*it)->classID() == ENT_UNIT){
                                 Unit &unit = (Unit &)(**it);
                                 if (unit.controlGroup == key){
                                    unit.selected = true;
                                    if (!sound)
                                       sound = &unit.type().getSound();
                                 }
                              }
                           }
                           game.selectionChanged = true;
                           assert(sound);
                           sound->play();
                        }
                     }

                  //home row keys - click equivalent UI bar button
                  else if (key == SDLK_a ||
                           key == SDLK_s ||
                           key == SDLK_d ||
                           key == SDLK_f ||
                           key == SDLK_g ||
                           key == SDLK_h ||
                           key == SDLK_j ||
                           key == SDLK_k ||
                           key == SDLK_l){
                     typeNum_t index = NO_TYPE;
                     switch (key){
                     case SDLK_a:
                        index = 0; break;
                     case SDLK_s:
                        index = 1; break;
                     case SDLK_d:
                        index = 2; break;
                     case SDLK_f:
                        index = 3; break;
                     case SDLK_g:
                        index = 4; break;
                     case SDLK_h:
                        index = 5; break;
                     case SDLK_j:
                        index = 6; break;
                     case SDLK_k:
                        index = 7; break;
                     case SDLK_l:
                        index = 8; break;
                     }
                     for (UIBars_t::iterator it = bars.begin();
                          it != bars.end(); ++it)
                        if ((*it)->isActive() &&
                            (*it)->size() > index)
                           (*it)->click(index);
                  }
                  else
                     debug("unhandled keypress: ", key);
               }
            }
         break;



      //TODO * clicking on a useless entity (eg. tree) = try next one
      //A mouse button is pressed
      case SDL_MOUSEBUTTONDOWN:
         debug("Mouse down: ", int(event.button.button));
         switch (event.button.button){
         case MOUSE_BUTTON_LEFT:
            game.leftMouse.mouseDown(Screen::mousePos - game.map);
            { //new scope for barClicked
               //if not clicking a button
               bool barClicked = false;
               for (UIBars_t::iterator it = bars.begin();
                    !barClicked && it != bars.end(); ++it)
                  if ((*it)->isActive())
                     if ((*it)->mouseIndex() != NO_TYPE)
                        barClicked = true;
               if (!barClicked){
                  //initialize selection box stuff
                  game.leftMouse.mouseDown(Screen::mousePos - game.map);
               }
               break;
            }
         case MOUSE_BUTTON_RIGHT:
            //initialize right-drag scroll stuff
            game.rightMouse.mouseDown(Screen::mousePos);
            break;
         }// switch mouse button
         pushMouseMove();
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
                     for (UIBars_t::iterator it = bars.begin();
                          it != bars.end(); ++it)
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
                     if (game.buildLocationOK){
                        game.constructBuilding(game.toBuild,
                                               Screen::mousePos - game.map,
                                               HUMAN_PLAYER);
                        //Shift key: multiple constructions
                        if(!isKeyPressed(SDLK_LSHIFT)){
                           game.mode = MODE_BUILDER;
                           for (UIBars_t::iterator it = bars.begin();
                                it != bars.end(); ++it)
                              if ((*it)->isActive())
                                 (*it)->calculateRect();
                        }
                     }else
                        debug ("Bad location for building");

                  }else{ //not construction mode
                     select(game);
                     setModeFromSelection(game, bars);
                  }
               }// if !barClicked
            }else{ //if dragging
               select(game);
               setModeFromSelection(game, bars);
            }
            game.leftMouse.mouseUp();
            break;
         }
         pushMouseMove();
         break;
      } //switch event
}

void scrollMap(GameData &game, double delta){

   bool scrolling = false;

   //right-dragging
   if (game.rightMouse.dragging){
      scrolling = true;
      Point rmbDisplacement = Screen::mousePos - game.rightMouse.dragBegin;
      Point scroll = rmbDisplacement * (delta * RMB_SCROLL_MULTIPLIER);
      if (game.scrollLockX)
         scroll.x = 0;
      if (game.scrollLockY)
         scroll.y = 0;
      game.map = game.map - scroll;
   }

   pixels_t scroll = pixels_t(delta * SCROLL_AMOUNT);

   //edge of screen
   if (!game.scrollLockX)
      if (Screen::mousePos.x < EDGE_SCROLL_MARGIN){
         scrolling = true;
         game.map.x += scroll;
      }else if (Screen::mousePos.x > Screen::getScreenRes().x - EDGE_SCROLL_MARGIN){
         scrolling = true;
         game.map.x -= scroll;
      }
   if (!game.scrollLockY)
      if (Screen::mousePos.y < EDGE_SCROLL_MARGIN){
         scrolling = true;
         game.map.y += scroll;
      }else if (Screen::mousePos.y > Screen::getScreenRes().y - EDGE_SCROLL_MARGIN){
         scrolling = true;
         game.map.y -= scroll;
      }

   //arrow keys
   if (!game.scrollLockX){
      if (isKeyPressed(SDLK_LEFT)){
         scrolling = true;
         game.map.x += scroll;
      }
      if (isKeyPressed(SDLK_RIGHT)){
         scrolling = true;
         game.map.x -= scroll;
      }
   }
   if (!game.scrollLockY){
      if (isKeyPressed(SDLK_UP)){
         scrolling = true;
         game.map.y += scroll;
      }
      if (isKeyPressed(SDLK_DOWN)){
         scrolling = true;
         game.map.y -= scroll;
      }
   }

   //Enforce scroll boundaries
   if (!game.scrollLockX){
      if (game.map.x > SCROLL_MARGIN){
         scrolling = true;
         game.map.x = SCROLL_MARGIN;
      }
      if (game.map.x + game.map.w < Screen::getScreenRes().x - SCROLL_MARGIN){
         scrolling = true;
         game.map.x = Screen::getScreenRes().x - SCROLL_MARGIN - game.map.w;
      }
   }
   if (!game.scrollLockY){
      if (game.map.y > SCROLL_MARGIN){
         scrolling = true;
         game.map.y = SCROLL_MARGIN;
      }
      if (game.map.y + game.map.h < Screen::getScreenRes().y - SCROLL_MARGIN){
         scrolling = true;
         game.map.y = Screen::getScreenRes().y - SCROLL_MARGIN - game.map.h;
      }
   }

   if (scrolling)
      pushMouseMove();
}

SDL_Rect getSelectionRect(const GameData &game){
   const SDL_Rect &map = game.map;
   const MouseButton &leftMouse = game.leftMouse;
   Point modMouse = Screen::mousePos - map;
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
   bool soundPlayed = false;
   game.selectionChanged = true;

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
                                    Screen::mousePos - 
                                    Point(game.map));

            if (collides){
               if (isKeyPressed(SDLK_LCTRL))
                  (*it)->toggleSelect(); //Ctrl: toggle
               else
                  (*it)->selected = true; //No ctrl: select
               entitySelected = true;
               if ((*it)->selected)
                  if (!soundPlayed){
                     (*it)->type().getSound().play();
                     soundPlayed = true;
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
         if (!targetEntity){
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

      //skip decorations
      if ((*it)->classID() == ENT_DECORATION)
         continue;

      //filtering
      //only targetable entities
      if (onlyTargetable && !(*it)->targetable())
         continue;

      if (collision((*it)->getDrawRect(),
                    Screen::mousePos - Point(game.map)))
         return *it;
   }
   return 0;
}

void setModeFromSelection(GameData &game, UIBars_t &bars){
   game.selectionChanged = false;
   game.buildingSelected = 0;
   ControlMode oldMode = game.mode;
   game.mode = MODE_NONE;

   //loop backwards, so objects in front have priority to be
   //selected
   for (entities_t::const_iterator it = game.entities.begin();
        it != game.entities.end(); ++it){
      if ((*it)->selected){
         EntityTypeID classType = (*it)->classID();

         //builder
         if (classType == ENT_UNIT &&
             ((const Unit &)(**it)).isBuilder()){
            game.mode = MODE_BUILDER;
            break;
         }

         //building
         else if (classType == ENT_BUILDING){
            game.mode = MODE_BUILDING;
            game.buildingSelected = (Building *)(*it);
         }
      }
   }

   if (oldMode == MODE_CONSTRUCTION &&
       game.mode == MODE_BUILDER)
         game.mode = MODE_CONSTRUCTION;

   for (UIBars_t::iterator it = bars.begin(); it != bars.end(); ++it)
      (*it)->calculateRect();
}

void checkVictory(GameData &game){
   //set each player initially to dead
   for (players_t::iterator it = game.players.begin();
        it != game.players.end(); ++it)
      it->alive = false;
   
   //change players based on which entities exist
   for (entities_t::const_iterator it = game.entities.begin();
        it != game.entities.end(); ++it){
      typeNum_t player = (*it)->getPlayer();
      if (player != NO_TYPE)
         game.players[player].alive = true;
   }

   //determine victory/loss
   bool enemiesAlive = false;
   for (typeNum_t i = 0; i != game.players.size(); ++i)
      if (i != HUMAN_PLAYER &&
          i != NATURE_PLAYER &&
          game.players[i].alive)
         enemiesAlive = true; //not a victory
   if (!enemiesAlive){
      game.outcome = VICTORY;
      game.loop = false;
   }else if (!game.players[HUMAN_PLAYER].alive){
      game.outcome = LOSS;
      game.loop = false;
   }
}