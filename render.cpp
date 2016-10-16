// (C) 2010 Tim Gurto

#include <cassert>
#include "render.h"
#include "update.h"
#include "GameData.h"
#include "Debug.h"
#include "MessageBox.h"
#include "Surface.h"
#include "Screen.h"
#include "util.h"
#include "misc.h"
#include "Unit.h"

extern Debug debug, deltaLog;

void renderLoadingScreen(Surface &screen, const Surface &loading){
   screen.fill(BLACK);
   loading.draw(screen, /*&*/makeRect((screen->w - loading->w) / 2,
                                  (screen->h - loading->h) / 2));
   screen.flip();

}

//TODO move rendering to classes in other functions
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
      SDL_Rect dst = makeRect(x, y, outcomeMessage->w, outcomeMessage->h);
      screenBuf.fill(game.players[HUMAN_PLAYER].getColor(), &dst);
      outcomeMessage.draw(screenBuf, Point(x, y));                                     
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
      pause.draw(screenBuf, /*&*/makeRect(cursorPos));

   //cursor might appear 'raised' from the wall
   bool raised = game.rightMouse.dragging;
   if (raised){
      cursorPos -= CURSOR_RAISED;
      shadowPos += CURSOR_RAISED;
   }

   shadow.draw(screenBuf, /*&*/makeRect(shadowPos)); //cursor shadow
   cursor.draw(screenBuf, /*&*/makeRect(cursorPos)); //cursor

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
      index.draw(screenBuf, /*&*/makeRect(cursorPos));

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
   ITERATE(entities_t::const_iterator, game.entities, it)
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

//Draws all entities
void renderEntities(const GameData &game){

   //Masks on: draws entities onto an intermediate surface, along
   //with filled key-color backgrounds, to create cleaner looking
   //entities when overlapping
   //Actual drawing of the masks occurs in Entity::colorBlit(),
   //along with the regular sprite drawing
   if (ENTITY_MASKS){
      //intermediate surface
      Surface entitiesTemp(SUR_BLANK, Screen::getScreenRes().x,
                           Screen::getScreenRes().y, ENTITY_BACKGROUND);
      entitiesTemp.fill(ENTITY_BACKGROUND);

      //decorations first
      ITERATE(entities_t::const_iterator, game.entities, it)
         if ((*it)->classID() == ENT_DECORATION && (*it)->onScreen())
            (*it)->draw(entitiesTemp);

      //then the rest
      ITERATE(entities_t::const_iterator, game.entities, it){
         const Entity &ent = **it;
         //debug(ent.getLoc().y);

         SDL_Rect baseRect = ent.getBaseRect() + game.map;

         //only draw entities that are on-screen
         if (ent.classID() != ENT_DECORATION && ent.onScreen()){
            
            //DEBUG
            if (DEBUG){
               //base rectangle and location
               entitiesTemp.fill(WHITE, &baseRect);
               const Point &loc = ent.getLoc() + game.map;
               SDL_Rect dst = makeRect(loc.x - 2, loc.y, 5, 1);
               entitiesTemp.fill(BLACK, &dst);
               dst = makeRect(loc.x, loc.y - 2, 1, 5);
               entitiesTemp.fill(BLACK, &dst);

               //draw path
               if (ent.classID() == ENT_UNIT){
                  const Unit &unit = (const Unit &)ent;
                  path_t copy = unit.getPath();
                  Point first = unit.getLoc() + game.map;
                  while (!copy.empty()){
                     Point second = copy.front() + game.map;
                     copy.pop();
                     entitiesTemp.fill(makeColor(getEntityColor(game, unit.getColor())),
                                       &makePathRect(ent.type(), first, second));
                     drawLine(entitiesTemp, first, second,
                              getEntityColor(game, BLACK_UINT));
                     first = second;
                  }
               }
            }//DEBUG

            //mask drawn in Entity::draw()
            ent.draw(entitiesTemp);
         }
      }

      screenBuf << entitiesTemp;
drawLine(screenBuf, Point(50, 50), Point(200, 400), CYAN_UINT);
drawLine(screenBuf, Point(50, 50), Point(50, 400), CYAN_UINT);

   //Masks off: draws entities straight to the screen.
   //Considerably faster.
   }else
      //some duplicate code, but cleaner this way
      ITERATE(entities_t::const_iterator, game.entities, it){
         const Entity &ent = **it;
         if (ent.classID() != ENT_DECORATION && ent.onScreen())
            ent.draw();
      }
}

//Draws the selection rectangle
void renderSelectionRect(const GameData &game, Surface &selRectImage){
   //Get rectangle
   SDL_Rect selRect = getSelectionRect(game);

   //only draw what's on the screen
   if (selRect.x < 0){
      selRect.w += selRect.x;
      selRect.x = 0;
   }else if ((selRect.x + selRect.w) > Screen::getScreenRes().x)
      selRect.w = Screen::getScreenRes().x - selRect.x;
   if (selRect.y < 0){
      selRect.h += selRect.y;
      selRect.y = 0;
   }else if ((selRect.y + selRect.h) > Screen::getScreenRes().y)
      selRect.h = Screen::getScreenRes().x - selRect.y;

   SDL_Rect src = makeRect(0, 0, selRect.w, selRect.h);
   selRectImage.draw(screenBuf, &selRect, &src);
}

//Draws all UIBars
void renderInterface(const UIBars_t &bars){
   ITERATE(UIBars_t::const_iterator, bars, it)
      (*it)->draw();
}

//Draws all active particles
void renderParticles(const GameData &game){
   ITERATE(particles_t::const_iterator, game.particles, it)
      it->draw(game);
}

void renderMessageBoxes(const messageBoxes_t messageBoxes){
   ITERATE(messageBoxes_t::const_iterator, messageBoxes, it)
      (*it)->draw();
}