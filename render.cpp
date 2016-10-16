// (C) 2010 Tim Gurto

#include <cassert>
#include "render.h"
#include "update.h"
#include "GameData.h"
#include "Debug.h"
#include "MessageBox.h"
#include "Surface.h"
#include "Screen.h"
#include "misc.h"

extern Debug debug, deltaLog;

void renderLoadingScreen(Surface &screen, const Surface &loading){
   screen.fill(BLACK);
   loading.draw(screen, &makeRect((screen->w - loading->w) / 2,
                                  (screen->h - loading->h) / 2));
   screen.flip();

}

void render(Surface &selection,
            Surface &diagGreen, Surface &diagRed,
            Surface &map, Surface &darkMap,
            Surface &cursor, Surface &cursorShadow,
            Surface &cursorPause, Surface &cursorColor,
            Surface &selRectImage,
            Surface *cursorIndex,
            const CoreData &core, const GameData &game,
            const UIBars_t &bars, const messageBoxes_t &messageBoxes,
            Surface outcomeMessage){

   assert (screenBuf);

   screenBuf.fill();

   renderMap(game, map, darkMap);
   renderSelection(game, selection);
   if (game.mode == MODE_CONSTRUCTION && !game.rightMouse.dragging)
       renderFootprint(core, game, diagGreen, diagRed);
   renderDecorations(game);
   renderEntities(game);
   if (game.mode != MODE_CONSTRUCTION && game.leftMouse.dragging)
      renderSelectionRect(game, selRectImage);
   renderInterface(bars);
   renderMessageBoxes(messageBoxes);
   renderCursor(game, cursor, cursorShadow, cursorPause,
                cursorColor, cursorIndex);
   renderParticles(game);

   if (game.paused){
      Surface darkCover(SUR_BLANK);
      darkCover.fill(BLACK);
      darkCover.setAlpha(SHADOW_ALPHA);
      screenBuf << darkCover;
   }

   if (outcomeMessage){
      pixels_t
         x = (screenBuf->w - outcomeMessage->w) / 2,
         y = (screenBuf->h - outcomeMessage->h) / 2;
      screenBuf.fill(game.players[HUMAN_PLAYER].getColor(),
                  &makeRect(x, y, outcomeMessage->w, outcomeMessage->h));
      outcomeMessage.draw(screenBuf, &makeRect(x, y));                                     
   }

   debug.display();
   //deltaLog.display();

   //Finalize
   screenBuf.flip();
}

void renderCursor (const GameData &game,
                   Surface &cursor, Surface &shadow,
                   Surface &pause, Surface &color,
                   Surface *cursorIndex){
   Point
      cursorPos = Screen::mousePos + CURSOR_OFFSET,
      shadowPos = cursorPos;

   if (game.paused)
      pause.draw(screenBuf, &makeRect(cursorPos));

   //cursor might appear 'raised' from the wall
   bool raised = game.rightMouse.dragging;
   if (raised){
      cursorPos -= CURSOR_RAISED;
      shadowPos += CURSOR_RAISED;
   }

   shadow.draw(screenBuf, &makeRect(shadowPos)); //cursor shadow
   cursor.draw(screenBuf, &makeRect(cursorPos)); //cursor

   //color
   unsigned colorIndex = game.cursorColor;
   if (colorIndex != CLR_MAX){
      //same technique as with coloring entities
      assert (colorIndex < CLR_MAX);
      assert (cursorIndex);
      

      Surface &index = cursorIndex[colorIndex];
      
      //create colored surface if it doesn't exist
      if (!index){
         assert (color);
         index = Surface(SUR_BLANK, color->w, color->h, ENTITY_BACKGROUND);
         index.fill(getEntityColor(game, colorIndex));
         index << color;
      }

      //colored index definitely exists now
      index.draw(screenBuf, &makeRect(cursorPos));

   }
}

//Draws the terrain and border tiles
void renderMap(const GameData &game,
               Surface &map, Surface &border){
   for (int x = -1; x != game.mapX + 1; ++x)
      for (int y = -1; y != game.mapY + 1; ++y){
         SDL_Rect tileRect = makeRect(game.map.x + x * MAP_TILE_SIZE,
                                      game.map.y + y * MAP_TILE_SIZE,
                                      MAP_TILE_SIZE,
                                      MAP_TILE_SIZE);
         //if tile is inside the screen
         if (collision(tileRect, screenBuf->clip_rect)){

            //which image to blit?
            Surface tile((x == -1 || x == game.mapX ||
                          y == -1 || y == game.mapY) ?
                             border : //outside map
                             map);    //inside map
            
            tile.draw(screenBuf, &tileRect);

         } //if collision
      } //for y
}

//Draws markers at the locations of selected entities
void renderSelection(const GameData &game,
                     Surface &selection){
   for (entities_t::const_iterator it = game.entities.begin();
        it != game.entities.end(); ++it)
      if ((*it)->selected && (*it)->onScreen()){
         SDL_Rect dest = (*it)->getSelectionDest(selection);
         selection.draw(screenBuf, &dest);
      }
}

//Draws a building footprint at the cursor
void renderFootprint(const CoreData &core, const GameData &game,
                     Surface &goodImage, Surface &badImage){
   SDL_Rect baseRect = Screen::mousePos +
                       core.buildingTypes[game.toBuild].getBaseRect();
   if (game.buildLocationOK &&
       game.players[HUMAN_PLAYER].
          sufficientResources(core.buildingTypes[game.toBuild].getCost()))
      goodImage.draw(screenBuf, &SDL_Rect(baseRect), &dimRect(baseRect));
   else
      badImage .draw(screenBuf, &SDL_Rect(baseRect), &dimRect(baseRect));
}

//Draws decoration entities
void renderDecorations(const GameData &game){
   for (entities_t::const_iterator it = game.entities.begin();
        it != game.entities.end(); ++it)
      if ((*it)->classID() == ENT_DECORATION && (*it)->onScreen())
         (*it)->draw();
}

//Draws all entities, except decorations
void renderEntities(const GameData &game){

   //Masks on: draws entities onto an intermediate surface, along
   //with filled key-color backgrounds, to create cleaner looking
   //entities when overlapping
   //Actual drawing of the masks occurs in Entity::colorBlit(),
   //along with the regular sprite drawing
   if (ENTITY_MASKS){
      //intermediate surface
      Surface entitiesTemp(SUR_BLANK, Screen::screenRes.x, Screen::screenRes.y,
                           ENTITY_BACKGROUND);
      entitiesTemp.fill(ENTITY_BACKGROUND);

      for (entities_t::const_iterator it = game.entities.begin();
           it != game.entities.end(); ++it){

         const Entity &ent = **it;
         debug(ent.getLoc().y);

         //only draw entities that are on-screen
         if (ent.classID() != ENT_DECORATION && ent.onScreen()){
            ent.type().mask_.draw(entitiesTemp,
                                  &makeRect(Point(game.map) +
                                            ent.getDrawRect()));
            ent.draw(entitiesTemp);
         }
      }

      screenBuf << entitiesTemp;

   //Masks off: draws entities straight to the screen.
   //Considerably faster.
   }else
      //some duplicate code, but cleaner this way
      for (entities_t::const_iterator it = game.entities.begin();
           it != game.entities.end(); ++it){

         const Entity &ent = **it;
         if (ent.classID() != ENT_DECORATION && ent.onScreen())
            ent.draw();
      }
}

//Draws the selection rectangle
void renderSelectionRect(const GameData &game,
                         Surface &selRectImage){
   //Get rectangle
   SDL_Rect selRect = getSelectionRect(game);

   //only draw what's on the screen
   if (selRect.x < 0){
      selRect.w += selRect.x;
      selRect.x = 0;
   }else if ((selRect.x + selRect.w) > Screen::screenRes.x)
      selRect.w = Screen::screenRes.x - selRect.x;
   if (selRect.y < 0){
      selRect.h += selRect.y;
      selRect.y = 0;
   }else if ((selRect.y + selRect.h) > Screen::screenRes.y)
      selRect.h = Screen::screenRes.x - selRect.y;

   selRectImage.draw(screenBuf, &selRect, &makeRect(0, 0,
                                                    selRect.w,
                                                    selRect.h));
}

//Draws all UIBars
void renderInterface(const UIBars_t &bars){
   for (UIBars_t::const_iterator it = bars.begin();
        it != bars.end(); ++it){
      (*it)->draw();
   }
}

//Draws all active particles
void renderParticles(const GameData &game){
   for (particles_t::const_iterator it = game.particles.begin();
        it != game.particles.end(); ++it)
      it->draw(game);
}

void renderMessageBoxes(const messageBoxes_t messageBoxes){
   for (messageBoxes_t::const_iterator it = messageBoxes.begin();
        it != messageBoxes.end(); ++it)
      (*it)->draw();
}