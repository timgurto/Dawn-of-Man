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
#include "ResourceNodeType.h"
#include "ResourceNode.h"
#include "UIBar.h"
#include "Particle.h"
#include "Point.h"
#include "Resources.h"
#include "MessageBox.h"
#include "Tech.h"
#include "TechBonuses.h"

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
   Entity::init(&game, screen, diagGreen);
   Particle::init(screen, particle, particleShadow);

   //UI Bars
   UIBars_t bars;
   UIBar::init(&game, screen, vBar, hBar);
   UIBar buildingsBar(BOTTOM_LEFT, HORIZONTAL,
                      &getNumBuildingIcons,
                      &getBuildingTypeIcons,
                      &selectBuilding,
                      &buildingHelp,
                      MODE_BUILDER);
   bars.push_back(&buildingsBar);
   UIBar unitsBar(BOTTOM_LEFT, HORIZONTAL,
                  &getNumUnitIcons,
                  &getUnitTypeIcons,
                  &trainUnit,
                  &unitHelp,
                  MODE_BUILDING);
   bars.push_back(&unitsBar);

   //Message Boxes
   messageBoxes_t messageBoxes;
   MessageBox contextHelp(WHITE,
                          2, SCREEN_HEIGHT - ICON_SIZE - 1,
                          275, 1,
                          darkMap,
                          "Woodbrush.ttf", 18,
                          true);
   messageBoxes.push_back(&contextHelp);
   MessageBox resourcesBox(WHITE,
                           2, 2, 150, 1,
                           darkMap,
                           "Woodbrush.ttf", 18);
   messageBoxes.push_back(&resourcesBox);

   //TODO load from files
   //=================================================
   std::vector<std::string> resourceNames;
   resourceNames.push_back("Wood");
   Resources::init(1, resourceNames);

   game.players.push_back(Player(0xca6500)); //0x... = color
   game.players.push_back(Player(0x882222));

   //building types
   resources_t campfireCost; campfireCost.push_back(100);
   BuildingType campfire(0, "Campfire",
                         makeRect(-41, -91, 79, 111),
                         makeRect(-42, -23, 81, 42),
                         Point(2, -37), campfireCost, 1250);
   game.buildingTypes.push_back(campfire);
   resources_t shrineCost; shrineCost.push_back(180);
   BuildingType shrine(1, "Shrine",
                       makeRect(-75, -67, 152, 77),
                       makeRect(-76, -17, 154, 28),
                       Point(1, -20), shrineCost, 1750);
   game.buildingTypes.push_back(shrine);
   for (typeNum_t i = 0; i != game.buildingTypes.size(); ++i)
      assert (game.buildingTypes[i].getIndex() == i);

   //decoration types
   DecorationType rock(0, "Rock",
                  makeRect(-11, -8, 22, 16),
                  makeRect(-11, -8, 22, 16),
                  Point(0, 0),
                  CLR_DECORATION);
   game.decorationTypes.push_back(rock);
   for (int i = 0; i != 15; ++i)
      addEntity(game, new Decoration(0, Point(
                                rand() % game.map.w,
                                rand() % game.map.h)));
   for (typeNum_t i = 0; i != game.decorationTypes.size(); ++i)
      assert (game.decorationTypes[i].getIndex() == i);

   //unit types
   resources_t genericCost; genericCost.push_back(50);
   UnitType generic(0, "Warrior",
                  makeRect(-22, -107, 70, 113),
                  makeRect(-22,-6, 53, 11),
                  Point(3, -55),
                  genericCost,
                  8, //speed
                  25, 8, //frames
                  10, 4, 25, //combat frames
                  50, 10, 2, //combat details
                  false, //builder
                  false, //gatherer
                  1, //origin building
                  1000); //progress cost
   game.unitTypes.push_back(generic);
   resources_t gruntCost; gruntCost.push_back(30);
   UnitType grunt(1, "Caveman",
                  makeRect(-22, -107, 70, 113),
                  makeRect(-22,-6, 53, 11),
                  Point(3, -55),
                  gruntCost,
                  8, //speed
                  25, 8, //frames
                  10, 4, 25, //combat frames
                  25, 5, 0, //combat details
                  true, //builder
                  true, //gatherer
                  0, //origin building
                  1000); //progress cost
   game.unitTypes.push_back(grunt);
   for (typeNum_t i = 0; i != game.unitTypes.size(); ++i)
      assert (game.unitTypes[i].getIndex() == i);

   //human start: one grunt
   Unit *newGrunt = new Unit(1,
                             Point(rand() % game.map.w,
                                   rand() % game.map.h),
                             HUMAN_PLAYER,
                             1000);
   addEntity(game, newGrunt);
   centerMap(game, newGrunt->getLoc());
   //computer start: a bunch of generics
   for (int i = 0; i != 10; ++i){
      pixels_t x, y;
      do{
         x = rand() % game.map.w;
         y = rand() % game.map.h;
      }while (!noCollision(game, game.buildingTypes[1],
                          Point(x, y)));
      addEntity(game, new Unit(0, Point(x, y), 1, 1000));
   }

   //resource node types
   resources_t treeMax, treeYield;
   treeMax.push_back(75);
   treeYield.push_back(5);
   ResourceNodeType tree(0, "Tree",
                     makeRect(-56, -132, 114, 133),
                     makeRect(-17, -5, 34, 12),
                     Point(2, -78),
                     treeMax,
                     treeYield,
                     CLR_RESOURCE_WOOD);
   game.resourceNodeTypes.push_back(tree);
   for (int i = 0; i != 10; ++i){
      pixels_t x, y;
      do{
         x = rand() % game.map.w;
         y = rand() % game.map.h;
      }while (!noCollision(game, game.resourceNodeTypes[0],
                          Point(x, y)));
      addEntity(game, new ResourceNode(0, Point(x, y)));
   }

   //techs
   resources_t fireShieldCost;
   fireShieldCost.push_back(200);
   TechBonuses fireShieldBonuses(5);
   Tech fireArmor(0, "Fire Armor", fireShieldBonuses, 0, fireShieldCost, NO_TYPE);
   game.techs.push_back(fireArmor);
   //=================================================

   timer_t oldTicks = SDL_GetTicks();
   game.loop = true;
   while (game.loop){

      //time stuff
      timer_t newTicks = SDL_GetTicks();
      timer_t delta = newTicks - oldTicks;
      oldTicks = newTicks;
      double deltaMod = 1.0 * delta / DELTA_MODIFIER;
      
      //TODO some better means of displaying FPS in Release
      deltaLog("Framerate: ", (delta == 0 ? 0 : 1000 / delta));
      deltaLog("    Delta: ", delta, "ms");

      //update state
      updateState(deltaMod, game, screen, bars,
                  contextHelp, resourcesBox);

      //render
      render(screen, glow, diagGreen, diagRed, map, darkMap, cursor,
             cursorShadow, cursorPause, game, bars, messageBoxes);

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