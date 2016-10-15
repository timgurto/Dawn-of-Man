// (C) 2010 Tim Gurto

#include <cassert>
#include "SDL.h"
#include "render.h"
#include "update.h"
#include "GameData.h"
#include "Debug.h"
#include "misc.h"

extern Debug debug, deltaLog;

//TODO break into smaller functions
void render(SDL_Surface *screen, SDL_Surface *glow,
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

   //Selection markers
   for (entities_t::const_iterator it = game.entities.begin();
      it != game.entities.end(); ++it)
      if ((*it)->selected && (*it)->onScreen()){
         SDL_Rect dest = (*it)->getSelectionDest(glow);
         SDL_BlitSurface(glow, 0, screen, &dest);
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
         (*it)->draw(ENTITY_MASKS ? entitiesTemp : screen);
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