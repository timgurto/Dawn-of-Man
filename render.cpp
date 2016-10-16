// (C) 2010 Tim Gurto

#include <cassert>
#include "render.h"
#include "update.h"
#include "GameData.h"
#include "Debug.h"
#include "MessageBox.h"
#include "Surface.h"
#include "misc.h"

extern Debug debug, deltaLog;

void renderLoadingScreen(Surface &screen, const Surface &loading){
   screen.fill(BLACK);
   loading.draw(screen, &makeRect((screen->w - loading->w) / 2,
                                  (screen->h - loading->h) / 2));
   screen.flip();

}

void render(Surface &screen, Surface &selection,
            Surface &diagGreen, Surface &diagRed,
            Surface &map, Surface &darkMap,
            Surface &cursor, Surface &cursorShadow,
            Surface &cursorPause, Surface &cursorColor,
            Surface &selRectImage,
            Surface *cursorIndex,
            const CoreData &core, const GameData &game,
            const UIBars_t &bars, const messageBoxes_t &messageBoxes,
            Surface outcomeMessage){

   assert (screen);

   screen.fill();

   renderMap(screen, game, map, darkMap);
   renderSelection(screen, game, selection);
   if (game.mode == MODE_CONSTRUCTION && !game.rightMouse.dragging)
       renderFootprint(screen, core, game, diagGreen, diagRed);
   renderEntities(screen, game);
   if (game.mode != MODE_CONSTRUCTION && game.leftMouse.dragging)
      renderSelectionRect(screen, game, selRectImage);
   renderInterface(bars);
   renderMessageBoxes(screen, messageBoxes);
   renderCursor(screen, game, cursor, cursorShadow, cursorPause,
                cursorColor, cursorIndex);
   renderParticles(game);

   if (game.paused){
      Surface darkCover(SUR_BLANK);
      darkCover.fill(BLACK);
      darkCover.setAlpha(SHADOW_ALPHA);
      darkCover.draw(screen);
   }

   if (outcomeMessage){
      pixels_t
         x = (screen->w - outcomeMessage->w) / 2,
         y = (screen->h - outcomeMessage->h) / 2;
      screen.fill(game.players[HUMAN_PLAYER].getColor(),
                  &makeRect(x, y, outcomeMessage->w, outcomeMessage->h));
      outcomeMessage.draw(screen, &makeRect(x, y));                                     
   }

   debug.display();
   //deltaLog.display();

   //Finalize
   screen.flip();
}

void renderCursor (Surface &screen, const GameData &game,
                   Surface &cursor, Surface &shadow,
                   Surface &pause, Surface &color,
                   Surface *cursorIndex){
   Point
      cursorPos = game.mousePos + CURSOR_OFFSET,
      shadowPos = cursorPos;

   if (game.paused)
      pause.draw(screen, &makeRect(cursorPos));

   //cursor might appear 'raised' from the wall
   bool raised = game.rightMouse.dragging;
   if (raised){
      cursorPos -= CURSOR_RAISED;
      shadowPos += CURSOR_RAISED;
   }

   shadow.draw(screen, &makeRect(shadowPos)); //cursor shadow
   cursor.draw(screen, &makeRect(cursorPos)); //cursor

   //color
   int colorIndex = game.cursorColor;
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
         index.draw(color);
      }

      //colored index definitely exists now
      index.draw(screen, &makeRect(cursorPos));

   }
}

//Draws the terrain and border tiles
void renderMap(Surface &screen, const GameData &game,
               Surface &map, Surface &border){
   assert (screen);
   for (int x = -1; x != game.mapX + 1; ++x)
      for (int y = -1; y != game.mapY + 1; ++y){
         SDL_Rect tileRect = makeRect(game.map.x + x * MAP_TILE_SIZE,
                                      game.map.y + y * MAP_TILE_SIZE,
                                      MAP_TILE_SIZE,
                                      MAP_TILE_SIZE);
         //if tile is inside the screen
         if (collision(tileRect, screen->clip_rect)){

            //which image to blit?
            Surface tile((x == -1 || x == game.mapX ||
                          y == -1 || y == game.mapY) ?
                             border : //outside map
                             map);    //inside map
            
            tile.draw(screen, &tileRect);

         } //if collision
      } //for y
}

//Draws markers at the locations of selected entities
void renderSelection(Surface &screen, const GameData &game,
                     Surface &selection){
   for (entities_t::const_iterator it = game.entities.begin();
        it != game.entities.end(); ++it)
      if ((*it)->selected && (*it)->onScreen()){
         SDL_Rect dest = (*it)->getSelectionDest(selection);
         selection.draw(screen, &dest);
      }
}

//Draws a building footprint at the cursor
void renderFootprint(Surface &screen,
                     const CoreData &core, const GameData &game,
                     Surface &goodImage, Surface &badImage){
   SDL_Rect baseRect = game.mousePos +
                       core.buildingTypes[game.toBuild].getBaseRect();
   if (game.buildLocationOK &&
       game.players[HUMAN_PLAYER].
          sufficientResources(core.buildingTypes[game.toBuild].getCost()))
      goodImage.draw(screen, &SDL_Rect(baseRect), &dimRect(baseRect));
   else
      badImage .draw(screen, &SDL_Rect(baseRect), &dimRect(baseRect));
}

//Draws decoration entities
void renderDecorations(Surface &screen, const GameData &game){
   for (entities_t::const_iterator it = game.entities.begin();
        it != game.entities.end(); ++it)
      if ((*it)->classID() == ENT_DECORATION && (*it)->onScreen())
         (*it)->draw();
}

//Draws all entities, except decorations
void renderEntities(Surface &screen, const GameData &game){

   //Masks on: draws entities onto an intermediate surface, along
   //with filled key-color backgrounds, to create cleaner looking
   //entities when overlapping
   //Actual drawing of the masks occurs in Entity::colorBlit(),
   //along with the regular sprite drawing
   if (ENTITY_MASKS){
      //intermediate surface
      Surface entitiesTemp(SUR_BLANK, SCREEN_WIDTH, SCREEN_HEIGHT,
                           ENTITY_BACKGROUND);
      entitiesTemp.fill(ENTITY_BACKGROUND);

      for (entities_t::const_iterator it = game.entities.begin();
           it != game.entities.end(); ++it)
         //only draw entities that are on-screen
         if ((*it)->onScreen())
            (*it)->draw(entitiesTemp);

      entitiesTemp.draw(screen);

   //Masks off: draws entities straight to the screen.
   //Considerably faster.
   }else
      //some duplicate code, but cleaner this way
      for (entities_t::const_iterator it = game.entities.begin();
           it != game.entities.end(); ++it)
         if ((*it)->classID() != ENT_DECORATION && (*it)->onScreen())
            (*it)->draw(screen);
}

//Draws the selection rectangle
void renderSelectionRect(Surface &screen, const GameData &game,
                         Surface &selRectImage){
   //Get rectangle
   SDL_Rect selRect = getSelectionRect(game);

   //only draw what's on the screen
   if (selRect.x < 0){
      selRect.w += selRect.x;
      selRect.x = 0;
   }else if ((selRect.x + selRect.w) > SCREEN_WIDTH)
      selRect.w = SCREEN_WIDTH - selRect.x;
   if (selRect.y < 0){
      selRect.h += selRect.y;
      selRect.y = 0;
   }else if ((selRect.y + selRect.h) > SCREEN_WIDTH)
      selRect.h = SCREEN_WIDTH - selRect.y;

   selRectImage.draw(screen, &selRect, &makeRect(0, 0,
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

void renderMessageBoxes(Surface &screen,
                        const messageBoxes_t messageBoxes){
   for (messageBoxes_t::const_iterator it = messageBoxes.begin();
        it != messageBoxes.end(); ++it)
      (*it)->display(screen);
}