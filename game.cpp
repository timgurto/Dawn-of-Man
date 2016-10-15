// (C) 2009-2010 Tim Gurto

#include <cassert>
#include <cstdlib>
#include <ctime>

#include "SDL.h"
#include "game.h"
#include "globals.h"
#include "update.h"
#include "render.h"
#include "misc.h"
#include "Debug.h"
#include "GameData.h"
#include "EntityType.h"
#include "Entity.h"
#include "BuildingType.h"
#include "Building.h"
#include "DecorationType.h"
#include "Decoration.h"
#include "UnitType.h"
#include "Unit.h"
#include "UIBar.h"
#include "Particle.h"
#include "Point.h"

extern Debug debug;
extern Debug deltaLog;

//higher = slower game speed.
//Game is normalized around this delta.
const int DELTA_MODIFIER = 40; //40;

void gameMode(){

   //TODO init functions

   //initialize screen and debug objects
   SDL_Surface *screen = setScreen();
   debug.initScreen(screen);
   deltaLog.initScreen(screen);

   //seed random generator
   srand(unsigned(time(0)));

   //load surfaces
   SDL_Surface
      *map = loadImage(MISC_IMAGE_PATH + "back.png"),
      *darkMap = loadImage(MISC_IMAGE_PATH + "dark.png"),
      *cursor = loadImage(MISC_IMAGE_PATH + "cursor.png", GREEN),
      *cursorShadow = loadImage(MISC_IMAGE_PATH +
                                "cursorShadow.PNG", GREEN),
      *cursorPause = loadImage(MISC_IMAGE_PATH +
                               "cursorPause.PNG", GREEN),
      *vBar = loadImage(INTERFACE_IMAGE_PATH + "vBar.PNG"),
      *hBar = loadImage(INTERFACE_IMAGE_PATH + "hBar.PNG"),
      *diagGreen = loadImage(MISC_IMAGE_PATH + "diagGreen.PNG", GREEN),
      *diagRed = loadImage(MISC_IMAGE_PATH + "diagRed.PNG", GREEN),
      *particle = loadImage(MISC_IMAGE_PATH + "particle.PNG", GREEN),
      *particleShadow = loadImage(MISC_IMAGE_PATH +
                                  "particleShadow.PNG", GREEN),
      *entitiesTemp = createSurface(),
      *glow = loadImage(MISC_IMAGE_PATH + "glow.PNG", true);
   SDL_SetColorKey(entitiesTemp, SDL_SRCCOLORKEY,
                   SDL_MapRGB(entitiesTemp->format,
                              ENTITY_BACKGROUND.r,
                              ENTITY_BACKGROUND.g,
                              ENTITY_BACKGROUND.b));
   SDL_SetAlpha(particleShadow, SDL_SRCALPHA, SHADOW_ALPHA);
   SDL_SetAlpha(cursorShadow, SDL_SRCALPHA, SHADOW_ALPHA);

   SDL_ShowCursor(SDL_DISABLE);

   //init
   GameData game(4, 4);
   Entity::init(&game, screen);
   Particle::init(screen, particle, particleShadow);
   game.mode = MODE_NORMAL;

   //TODO load from files
   //=================================================
   game.players.push_back(0xca6500); //0x... = color
   game.players.push_back(0x882222);
   BuildingType campfire(0, "Campfire",
                         makeRect(-42, -92, 81, 113),
                         makeRect(-42, -23, 81, 42),
                         Point(2, -37), 1250);
   game.buildingTypes.push_back(campfire);
   BuildingType shrine(1, "Shrine",
                       makeRect(-76, -68, 154, 79),
                       makeRect(-76, -17, 154, 28),
                       Point(1, -20), 1750);
   game.buildingTypes.push_back(shrine);
   DecorationType rock(0, "Rock",
                  makeRect(-12, -9, 24, 18),
                  makeRect(-12, -9, 24, 18),
                  Point(0, 0));
   game.decorationTypes.push_back(rock);
   for (int i = 0; i != 15; ++i)
      addEntity(game, new Decoration(0, Point(
                                rand() % game.map.w,
                                rand() % game.map.h)));
   UnitType generic(0, "Generic",
                  makeRect(-22, -107, 70, 113),
                  makeRect(-22,-6, 53, 11),
                  Point(3, -55),
                  8, //speed
                  25, 8, //frames
                  10, 4, 25, //combat frames
                  50, 10, 2, //combat details
                  false, //builder
                  1, //origin building
                  1000); //progress cost
   game.unitTypes.push_back(generic);
   UnitType grunt(1, "Grunt",
                  makeRect(-22, -107, 70, 113),
                  makeRect(-22,-6, 53, 11),
                  Point(3, -55),
                  8, //speed
                  25, 8, //frames
                  10, 4, 25, //combat frames
                  25, 5, 0, //combat details
                  true, //builder
                  0, //origin building
                  1000); //progress cost
   game.unitTypes.push_back(grunt);

   //human start: one grunt
   Unit *newGrunt = new Unit(1,
                             Point(rand() % game.map.w,
                                   rand() % game.map.h),
                             HUMAN_PLAYER,
                             1000);
   addEntity(game, newGrunt);
   //centerMap(game, newGrunt->getLoc);
   game.map.x = SCREEN_WIDTH / 2 - newGrunt->getLoc().x;
   game.map.y = SCREEN_HEIGHT / 2 - newGrunt->getLoc().y;

   //computer start: a bunch of generics
   for (int i = 0; i != 10; ++i){
      pixels_t x, y;
      do{
         x = rand() % game.map.w;
         y = rand() % game.map.h;
      }while (noCollision(game, game.buildingTypes[1],
                          Point(x, y)));
      addEntity(game, new Unit(0, Point(x, y), 1, 1000));
   }
   //=================================================

   UIBars_t bars;
   UIBar::init(&game, screen, vBar, hBar);
   UIBar buildingsBar(BOTTOM_LEFT, VERTICAL,
                      &getNumBuildingIcons,
                      &getBuildingTypeIcons,
                      &selectBuilding,
                      MODE_BUILDER);
   bars.push_back(&buildingsBar);
   UIBar unitsBar(BOTTOM_LEFT, HORIZONTAL,
                  &getNumUnitIcons,
                  &getUnitTypeIcons,
                  &trainUnit,
                  MODE_BUILDING);
   bars.push_back(&unitsBar);
   

   timer_t oldTicks = SDL_GetTicks();
   game.loop = true;
   while (game.loop){

      //time stuff
      timer_t newTicks = SDL_GetTicks();
      timer_t delta = newTicks - oldTicks;
      oldTicks = newTicks;
      double deltaMod = 1.0 * delta / DELTA_MODIFIER;
      
      //TODO some better means of displaying FPS in Release
      deltaLog("    Framerate: ", (delta == 0 ? 0 : 1000 / delta));
      deltaLog("        Delta: ", delta, "ms");

      //update state
      updateState(deltaMod, game, screen, bars);

      //render
      render(screen, glow, diagGreen, diagRed, map, darkMap, cursor,
             cursorShadow, cursorPause, game, bars);

   }

   //Clean up
   SDL_ShowCursor(SDL_ENABLE);

   freeSurface(map);
   freeSurface(darkMap);
   freeSurface(cursor);
   freeSurface(cursorShadow);
   freeSurface(cursorPause);
   freeSurface(vBar);
   freeSurface(hBar);
   freeSurface(particle);
   freeSurface(particleShadow);
   freeSurface(diagGreen);
   freeSurface(diagRed);
   freeSurface(entitiesTemp);
   freeSurface(glow);

}

void addEntity(GameData &game, Entity *entity){
   game.entities.insert(std::lower_bound(game.entities.begin(),
                                         game.entities.end(), entity,
                                         dereferenceLessThan),
                        entity);
}