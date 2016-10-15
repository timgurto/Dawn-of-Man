// (C) 2010 Tim Gurto

#include <cassert>
#include "SDL.h"
#include "render.h"
#include "update.h"
#include "GameData.h"
#include "Debug.h"
#include "misc.h"

extern Debug debug, deltaLog;

//Alpha of the selection rectangle
const Uint8 SELECTION_RECT_ALPHA = 0x66;

void render(SDL_Surface *screen, SDL_Surface *selection,
            SDL_Surface *diagGreen, SDL_Surface *diagRed,
            SDL_Surface *map, SDL_Surface *darkMap,
            SDL_Surface *cursor, SDL_Surface *cursorShadow,
            SDL_Surface *entitiesTemp,
            const GameData &game, const UIBars_t &bars){

   assert (screen != 0);

   renderMap(screen, game, map, darkMap);
   renderSelection(screen, game, selection);
   if (game.mode == MODE_CONSTRUCTION && game.rightMouse.dragging)
       renderFootprint(screen, game, diagRed, diagGreen);
   renderEntities(screen, game);
   if (game.mode != MODE_CONSTRUCTION && game.leftMouse.dragging)
      renderSelectionRect(screen, game);
   renderInterface(screen, bars);
   renderCursor(screen, game, cursor, cursorShadow);
   renderParticles(screen, game);

   //Debug text
   debug.display();
   deltaLog.display();

   //Finalize
   bool test = SDL_Flip(screen) == 0;
   assert(test);

}

void renderCursor (SDL_Surface *screen, const GameData &game,
                   SDL_Surface *cursor, SDL_Surface *shadow){
   //cursor might appear 'raised' from the wall
   bool raised = game.rightMouse.dragging;

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

//Draws the terrain and border tiles
void renderMap(SDL_Surface *screen, const GameData &game,
               SDL_Surface *map, SDL_Surface *border){
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
void renderFootprint(SDL_Surface *screen, const GameData &game,
                     SDL_Surface *goodImage, SDL_Surface *badImage){
   SDL_Rect baseRect = game.mousePos +
                       game.buildingTypes[game.toBuild].getBaseRect();
   if (game.buildLocationOK)
      SDL_BlitSurface(goodImage, &dimRect(baseRect),
                      screen, &SDL_Rect(baseRect));
   else
      SDL_BlitSurface(badImage, &dimRect(baseRect),
                      screen, &SDL_Rect(baseRect));
}

//Draws all entities
void renderEntities(SDL_Surface *screen, const GameData &game){
   SDL_Surface *entitiesTemp;
   if (ENTITY_MASKS){
      entitiesTemp = createSurface();
      SDL_SetColorKey(entitiesTemp, SDL_SRCCOLORKEY,
                      SDL_MapRGB(entitiesTemp->format,
                                 ENTITY_BACKGROUND.r,
                                 ENTITY_BACKGROUND.g,
                                 ENTITY_BACKGROUND.b));
      SDL_FillRect(entitiesTemp, 0, ENTITY_BACKGROUND_UINT);
   }
   for (entities_t::const_iterator it = game.entities.begin();
        it != game.entities.end(); ++it)
      //only draw entities that are on-screen
      if ((*it)->onScreen()){
         (*it)->draw(ENTITY_MASKS ? entitiesTemp : screen);
      }
   if (ENTITY_MASKS){
      SDL_BlitSurface(entitiesTemp, 0, screen, 0);
      freeSurface(entitiesTemp);
   }
}

//Draws the selection rectangle
void renderSelectionRect(SDL_Surface *screen, const GameData &game){
   //Get rectangle
   SDL_Rect selRect = getSelectionRect(game);
   
   //Draw rectangle
   SDL_Surface *temp = createSurface(selRect.w, selRect.h);
   SDL_SetAlpha(temp, SDL_SRCALPHA, SELECTION_RECT_ALPHA);
   SDL_FillRect(temp, &dimRect(selRect), WHITE_UINT);
   SDL_BlitSurface(temp, 0, screen, &selRect);
   freeSurface(temp);
}

//Draws all UIBars
void renderInterface(SDL_Surface *screen, const UIBars_t &bars){
   for (UIBars_t::const_iterator it = bars.begin();
        it != bars.end(); ++it){
      (*it)->draw();
   }
}

//Draws all active particles
void renderParticles(SDL_Surface *screen, const GameData &game){
   for (particles_t::const_iterator it = game.particles.begin();
        it != game.particles.end(); ++it)
      it->draw(game);
}