// (C) 2010 Tim Gurto

#include <cassert>
#include "SDL.h"
#include "render.h"
#include "update.h"
#include "GameData.h"
#include "Debug.h"
#include "MessageBox.h"
#include "misc.h"

extern Debug debug, deltaLog;

//Alpha of the selection rectangle
const Uint8 SELECTION_RECT_ALPHA = 0x66;

void render(SDL_Surface *screen, SDL_Surface *selection,
            SDL_Surface *diagGreen, SDL_Surface *diagRed,
            SDL_Surface *map, SDL_Surface *darkMap,
            SDL_Surface *cursor, SDL_Surface *cursorShadow,
            SDL_Surface *cursorPause, SDL_Surface *cursorColor,
            SDL_Surface **cursorIndex,
            const CoreData &core, const GameData &game,
            const UIBars_t &bars, const messageBoxes_t &messageBoxes){

   assert (screen);

   renderMap(screen, game, map, darkMap);
   renderDecorations(screen, game);
   renderSelection(screen, game, selection);
   if (game.mode == MODE_CONSTRUCTION && !game.rightMouse.dragging)
       renderFootprint(screen, core, game, diagGreen, diagRed);
   renderEntities(screen, game);
   if (game.mode != MODE_CONSTRUCTION && game.leftMouse.dragging)
      renderSelectionRect(screen, game);
   renderInterface(bars);
   renderMessageBoxes(screen, messageBoxes);
   renderCursor(screen, game, cursor, cursorShadow, cursorPause,
                cursorColor, cursorIndex);
   renderParticles(game);

   if (game.paused){
      SDL_Surface *darkCover = createSurface();
      SDL_FillRect(darkCover, &darkCover->clip_rect, BLACK_UINT);
      SDL_SetAlpha(darkCover, SDL_SRCALPHA, SHADOW_ALPHA);
      SDL_BlitSurface(darkCover, 0, screen, 0);
      freeSurface(darkCover);
   }

   //Debug text
   debug.display();
   //deltaLog.display();

   //Finalize
   bool test = SDL_Flip(screen) == 0;
   assert(test);
}

void renderCursor (SDL_Surface *screen, const GameData &game,
                   SDL_Surface *cursor, SDL_Surface *shadow,
                   SDL_Surface *pause, SDL_Surface *color,
                   SDL_Surface **cursorIndex){
   Point
      cursorPos = game.mousePos + CURSOR_OFFSET,
      shadowPos = cursorPos;

   if (game.paused)
      SDL_BlitSurface(pause, 0, screen, &makeRect(cursorPos));

   //cursor might appear 'raised' from the wall
   bool raised = game.rightMouse.dragging;
   if (raised){
      cursorPos -= CURSOR_RAISED;
      shadowPos += CURSOR_RAISED;
   }

   SDL_BlitSurface(shadow, 0, screen, &makeRect(shadowPos)); //shadow
   SDL_BlitSurface(cursor, 0, screen, &makeRect(cursorPos)); //cursor

   //color
   int colorIndex = game.cursorColor;
   if (colorIndex != CLR_MAX){
      //same technique as with coloring entities
      assert (colorIndex < CLR_MAX);
      assert (cursorIndex);
      SDL_Surface *&index = cursorIndex[colorIndex];
      
      //create colored surface if it doesn't exist
      if (!index){
         assert (color);
         index = createSurface(color->w, color->h);
         setColorKey(index);
         SDL_FillRect(index, 0, getEntityColor(game, colorIndex));
         SDL_BlitSurface(color, 0, index, 0);
      }

      //colored index definitely exists now
      SDL_BlitSurface(index, 0, screen, &makeRect(cursorPos));


   }
}

//Draws the terrain and border tiles
void renderMap(SDL_Surface *screen, const GameData &game,
               SDL_Surface *map, SDL_Surface *border){
   assert (screen);
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
               tile = border;

            SDL_BlitSurface(tile, 0, screen, &tileRect);
         } //if collision
      } //for y
}

//Draws markers at the locations of selected entities
void renderSelection(SDL_Surface *screen, const GameData &game,
                     SDL_Surface *selection){
   for (entities_t::const_iterator it = game.entities.begin();
        it != game.entities.end(); ++it)
      if ((*it)->selected && (*it)->onScreen()){
         SDL_Rect dest = (*it)->getSelectionDest(selection);
         SDL_BlitSurface(selection, 0, screen, &dest);
      }
}

//Draws a building footprint at the cursor
void renderFootprint(SDL_Surface *screen,
                     const CoreData &core, const GameData &game,
                     SDL_Surface *goodImage, SDL_Surface *badImage){
   SDL_Rect baseRect = game.mousePos +
                       core.buildingTypes[game.toBuild].getBaseRect();
   if (game.buildLocationOK &&
       game.players[HUMAN_PLAYER].
          sufficientResources(core.buildingTypes[game.toBuild].getCost()))
      SDL_BlitSurface(goodImage, &dimRect(baseRect),
                      screen, &SDL_Rect(baseRect));
   else
      SDL_BlitSurface(badImage, &dimRect(baseRect),
                      screen, &SDL_Rect(baseRect));
}

//Draws decoration entities
void renderDecorations(SDL_Surface *screen, const GameData &game){
   for (entities_t::const_iterator it = game.entities.begin();
        it != game.entities.end(); ++it)
      if ((*it)->classID() == ENT_DECORATION && (*it)->onScreen())
         (*it)->draw(screen);
}

//Draws all entities, except decorations
void renderEntities(SDL_Surface *screen, const GameData &game){

   //Masks on: draws entities onto an intermediate surface, along
   //with filled key-color backgrounds, to create cleaner looking
   //entities when overlapping
   //Actual drawing of the masks occurs in Entity::colorBlit(),
   //along with the regular sprite drawing
   if (ENTITY_MASKS){
      //intermediate surface
      SDL_Surface *entitiesTemp = createSurface();
      SDL_SetColorKey(entitiesTemp, SDL_SRCCOLORKEY,
                      SDL_MapRGB(entitiesTemp->format,
                                 ENTITY_BACKGROUND.r,
                                 ENTITY_BACKGROUND.g,
                                 ENTITY_BACKGROUND.b));
      SDL_FillRect(entitiesTemp, 0, ENTITY_BACKGROUND_UINT);
      for (entities_t::const_iterator it = game.entities.begin();
           it != game.entities.end(); ++it)
         //only draw entities that are on-screen
         if ((*it)->classID() != ENT_DECORATION && (*it)->onScreen())
            (*it)->draw(entitiesTemp);
      SDL_BlitSurface(entitiesTemp, 0, screen, 0);
      freeSurface(entitiesTemp);

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
void renderSelectionRect(SDL_Surface *screen, const GameData &game){
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
   
   //Draw rectangle
   SDL_Surface *temp = createSurface(selRect.w, selRect.h);
   SDL_SetAlpha(temp, SDL_SRCALPHA, SELECTION_RECT_ALPHA);
   SDL_FillRect(temp, &dimRect(selRect), WHITE_UINT);
   SDL_BlitSurface(temp, 0, screen, &selRect);
   freeSurface(temp);
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

void renderMessageBoxes(SDL_Surface *screen,
                        const messageBoxes_t messageBoxes){
   for (messageBoxes_t::const_iterator it = messageBoxes.begin();
        it != messageBoxes.end(); ++it)
      (*it)->display(screen);
}