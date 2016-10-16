// (C) 2009-2010 Tim Gurto

#include <cassert>
#include <cstdlib>
#include <ctime>
#include <map>
#include <string>

#include "SDL.h"
#include "SDL_mixer.h"

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
//extern Debug deltaLog;

//higher = slower game speed.
//Game is normalized around this delta.
const int DELTA_MODIFIER = 40; //40;

void gameMode(){

   //TODO init functions

   //initialize screen and debug objects
   SDL_Surface *screen = setScreen();
   debug.initScreen(screen);
   //deltaLog.initScreen(screen);

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
   setColorKey(entitiesTemp);
   SDL_SetAlpha(particleShadow, SDL_SRCALPHA, SHADOW_ALPHA);
   SDL_SetAlpha(cursorShadow, SDL_SRCALPHA, SHADOW_ALPHA);

   std::string musicPath = SOUND_PATH + "greensleeves.wav";
   Mix_Music *music = Mix_LoadMUS(musicPath.c_str());
   if (!DEBUG)
      Mix_PlayMusic(music, -1);

   SDL_ShowCursor(SDL_DISABLE);

   //init
   GameData game(8, 8);
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
   UIBar unitsBar(BOTTOM_LEFT, HORIZONTAL,
                  &getNumUnitIcons,
                  &getUnitTypeIcons,
                  &trainUnit,
                  &unitHelp,
                  MODE_BUILDING);
   UIBar techsBar(BOTTOM_LEFT, HORIZONTAL,
                  &getNumTechIcons,
                  &getTechIcons,
                  &researchTech,
                  &techHelp,
                  MODE_BUILDING);
   bars.push_back(&buildingsBar);
   bars.push_back(&unitsBar);
   bars.push_back(&techsBar);

   //more init
   Player::init(&game, &buildingsBar);

   //Message Boxes
   messageBoxes_t messageBoxes;
   MessageBox contextHelp(WHITE,
                          2, SCREEN_HEIGHT - ICON_SIZE - 1,
                          1,
                          darkMap,
                          "Woodbrush.ttf", 18,
                          true);
   MessageBox resourcesBox(WHITE,
                           2, 2, 1,
                           darkMap,
                           "Woodbrush.ttf", 18);
   MessageBox fpsDisplay(WHITE,
                         SCREEN_WIDTH / 2 - 40, 2,
                         1,
                         darkMap,
                         "Dina.fon", 0,
                         false, DEBUG);
   messageBoxes.push_back(&contextHelp);
   messageBoxes.push_back(&resourcesBox);
   messageBoxes.push_back(&fpsDisplay);

   //TODO load from files
   //=================================================
   initializeData("Data/dawn.dat", game);

   //starting enemy shrine
   Building *enemyShrine = new Building(1, makeRect(1000, 1000),
                                        2, 1750);
   addEntity(game, enemyShrine);

   //starting rocks
   for (int i = 0; i != 70; ++i)
      addEntity(game, new Decoration(0, Point(
                                rand() % game.map.w,
                                rand() % game.map.h)));

   //human start: one grunt
   Unit *newGrunt = new Unit(1,
                             Point(rand() % game.map.w,
                                   rand() % game.map.h),
                             HUMAN_PLAYER,
                             1000);
   addEntity(game, newGrunt);
   centerMap(game, newGrunt->getLoc());

   //computer start: a bunch of generics
   for (int i = 0; i != 30; ++i){
      pixels_t x, y;
      do{
         x = rand() % game.map.w;
         y = rand() % game.map.h;
      }while (!noCollision(game, game.unitTypes[0],
                          Point(x, y)));
      addEntity(game, new Unit(0, Point(x, y), 2, 1000));
   }

   //nature start: some deer
   for (int i = 0; i != 20; ++i){
      pixels_t x, y;
      do{
         x = rand() % game.map.w;
         y = rand() % game.map.h;
      }while (!noCollision(game, game.unitTypes[2],
                          Point(x, y)));
      addEntity(game, new Unit(2, Point(x, y), NATURE_PLAYER, 1000));
   }

   //starting trees
   for (int i = 0; i != 100; ++i){
      pixels_t x, y;
      do{
         x = rand() % game.map.w;
         y = rand() % game.map.h;
      }while (!noCollision(game, game.resourceNodeTypes[0],
                          Point(x, y)));
      addEntity(game, new ResourceNode(0, Point(x, y)));
   }

   checklist_t noneResearched(game.techs.size(), false);
   game.players.push_back(Player(0x735e3e, noneResearched)); //0: nature
   game.players.push_back(Player(0xca6500, noneResearched)); //1: human
   game.players.push_back(Player(0x882222, noneResearched));
   //=================================================

   timer_t oldTicks = SDL_GetTicks();
   game.loop = true;
   while (game.loop){

      //time stuff
      timer_t newTicks = SDL_GetTicks();
      timer_t delta = newTicks - oldTicks;
      oldTicks = newTicks;
      double deltaMod = 1.0 * delta / DELTA_MODIFIER;
      
      double fps = delta == 0 ? 0 : 1000 / delta;
      //deltaLog("Framerate: ", fps);
      //deltaLog("    Delta: ", delta, "ms");
      fpsDisplay(format3(fps), "fps  |  ", delta, "ms ");

      //update state
      updateState(deltaMod, game, screen, bars,
                  contextHelp, resourcesBox, fpsDisplay);

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

   Mix_HaltMusic(); 
   Mix_FreeMusic(music);

}

void addEntity(GameData &game, Entity *entity){
   game.entities.insert(std::lower_bound(game.entities.begin(),
                                         game.entities.end(), entity,
                                         dereferenceLessThan),
                        entity);
}

void initializeData(char *filename, GameData &game){
   std::ifstream data(filename);

   typeNum_t resourceCount = 0;
   std::map<std::string, typeNum_t> resourceIndices;
   std::vector<std::string> resourceNames;

   debug("");
   while (data){
      
      //initialize synthesized data
      std::string
         name = "",
         soundFile = "",
         deathSoundFile = "",
         hitSoundFile = "";
      bool
         collides = false,
         builder = false,
         gatherer = false;
      typeNum_t
         index = NO_TYPE,
         prereqBuilding = NO_TYPE,
         prereqTech = NO_TYPE,
         prereqTech2 = NO_TYPE,
         originBuilding = NO_TYPE,
         resourceAtDeath = NO_TYPE;
      damage_t
         maxHealth = 1,
         armor = 0,
         attack = 0;
      pixels_t
         speed = 8;
      progress_t
         progressCost = 0;
      frames_t
         maxFrameCounter = 0,
         frameCount = 1,
         maxCombatFrameCounter = 0,
         combatFrameCount = 1,
         combatWait = 1;
      int
         color = 0;
      SDL_Rect
         drawRect = makeRect(),
         baseRect = makeRect();
      Point
         selectionCenter = Point();
      TechBonuses
         bonus;
      resources_t
         cost(resourceCount, 0),
         maxResources(resourceCount, 0),
         yield(resourceCount, 0);

      //read object name
      std::string object = parseToken(data);
      if (object == ";")
         break;
      assert (object[object.size() - 1] == '{');
      removeLast(object);
      debug(object + ":");
      while (data){
         std::string attr = parseToken(data);
         if (attr == "}")
            break;

         assert (attr[attr.size()-1] == '=');
         removeLast(attr);
         std::string val = parseToken(data);
         if (val[val.size() - 1] == ',')
            removeLast(val);
         double numVal = 0;
         bool isNum = val[0] != '\"';
         if (isNum)
            numVal = atod(val);
         else
            val = val.substr(1, val.length()-3);

         if (isNum)
            debug(attr + " = #", numVal);
         else
            debug(attr + " = " +  val);

         //special case: keeping track of resources
         if (object == "resource" && attr == "name"){
            resourceIndices[val] = resourceCount;
            ++resourceCount;
         }

         //read attribute into relevant var
         if (attr == "name")
            name = val;
         else if (attr == "soundFile")
            soundFile = val;
         else if (attr == "deathSoundFile")
            deathSoundFile = val;
         else if (attr == "hitSoundFile")
            hitSoundFile = val;
         else if (attr == "collides")
            collides = numVal != 0;
         else if (attr == "builder")
            builder = numVal != 0;
         else if (attr == "gatherer")
            gatherer = numVal != 0;
         else if (attr == "index")
            index = typeNum_t(numVal);
         else if (attr == "prereqBuilding")
            prereqBuilding = typeNum_t(numVal);
         else if (attr == "originBuilding")
            originBuilding = typeNum_t(numVal);
         else if (attr == "resourceAtDeath")
            resourceAtDeath = typeNum_t(numVal);
         else if (attr == "maxHealth")
            maxHealth = damage_t(numVal);
         else if (attr == "armor")
            armor = damage_t(numVal);
         else if (attr == "attack")
            attack = damage_t(numVal);
         else if (attr == "speed")
            speed = pixels_t(numVal);
         else if (attr == "progressCost")
            progressCost = progress_t(numVal);
         else if (attr == "maxFrameCounter")
            maxFrameCounter = frames_t(numVal);
         else if (attr == "frameCount")
            frameCount = frames_t(numVal);
         else if (attr == "maxCombatFrameCounter")
            maxCombatFrameCounter = frames_t(numVal);
         else if (attr == "combatFrameCount")
            combatFrameCount = frames_t(numVal);
         else if (attr == "combatWait")
            combatWait = frames_t(numVal);
         else if (attr == "color")
            color = EntityColor(int(numVal + CLR_DEFAULT));
         else if (attr.substr(0, 9) == "drawRect."){
            char c = attr[9];
            if (c == 'x')
               drawRect.x = pixels_t(numVal);
            else if (c == 'y')
               drawRect.y = pixels_t(numVal);
            else if (c == 'w')
               drawRect.w = pixels_t(numVal);
            else if (c == 'h')
               drawRect.h = pixels_t(numVal);
         }else if (attr.substr(0, 9) == "baseRect."){
            char c = attr[9];
            if (c == 'x')
               baseRect.x = pixels_t(numVal);
            else if (c == 'y')
               baseRect.y = pixels_t(numVal);
            else if (c == 'w')
               baseRect.w = pixels_t(numVal);
            else if (c == 'h')
               baseRect.h = pixels_t(numVal);
         }else if (attr.substr(0, 16) == "selectionCenter."){
            char c = attr[16];
            if (c == 'x')
               selectionCenter.x = pixels_t(numVal);
            else if (c == 'y')
               selectionCenter.y = pixels_t(numVal);
         }else if (attr.substr(0, 16) == "bonus.gathering."){
            typeNum_t i = resourceIndices[attr.substr(16)];
            //ensure gathering bonus is big enough
            while (bonus.gathering.size() < i)
               bonus.gathering.push_back(0);
            bonus.gathering[i] = resource_t(numVal);
         }else if (attr.substr(0, 6) == "bonus."){
            std::string sub = attr.substr(6);
            if (sub == "unitHealth")
               bonus.unitHealth = damage_t(numVal);
            else if (sub == "unitArmor")
               bonus.unitArmor = damage_t(numVal);
            else if (sub == "unitAttack")
               bonus.attack = damage_t(numVal);
            else if (sub == "buildingHealth")
               bonus.buildingHealth = damage_t(numVal);
            else if (sub == "buildingArmor")
               bonus.buildingArmor = damage_t(numVal);
            else if (sub == "unitSpeed")
               bonus.speed = pixels_t(numVal);
            else if (sub == "trainingSpeed")
               bonus.trainingSpeed = progress_t(numVal);
            else if (sub == "buildingSpeed")
               bonus.buildingSpeed = damage_t(numVal);
            else if (sub == "unitArmor")
               bonus.unitArmor = damage_t(numVal);
            else if (sub == "unitArmor")
               bonus.unitArmor = damage_t(numVal);
            else if (sub == "unitArmor")
               bonus.unitArmor = damage_t(numVal);
            else if (sub == "unitArmor")
               bonus.unitArmor = damage_t(numVal);
            else if (sub == "unitArmor")
               bonus.unitArmor = damage_t(numVal);
         }else if (attr.substr(0, 5) == "cost."){
            typeNum_t i = resourceIndices[attr.substr(5)];
            //ensure cost is big enough
            while (cost.size() < i)
               cost.push_back(0);
            cost[i] = resource_t(numVal);
         }else if (attr.substr(0, 13) == "maxResources."){
            typeNum_t i = resourceIndices[attr.substr(13)];
            //ensure maxResources is big enough
            while (maxResources.size() < i)
               maxResources.push_back(0);
            maxResources[i] = resource_t(numVal);
         }else if (attr.substr(0, 6) == "yield."){
            typeNum_t i = resourceIndices[attr.substr(6)];
            //ensure yield is big enough
            while (yield.size() < i)
               yield.push_back(0);
            yield[i] = resource_t(numVal);
         }

      }
      debug("");

      //initialize objects
      if (object == "resource"){
         resourceNames.push_back(name);
      while (cost.size() < resourceNames.size())
         cost.push_back(0);
      }else if (object == "buildingType"){
         BuildingType temp(index, name, drawRect, baseRect,
                           selectionCenter, cost, maxHealth, armor,
                           progressCost, prereqBuilding, prereqTech,
                           soundFile, deathSoundFile);
         game.buildingTypes.push_back(temp);
      }else if (object == "decorationType"){
         DecorationType temp(index, name, drawRect, baseRect, collides);
         game.decorationTypes.push_back(temp);
      }else if (object == "unitType"){
         UnitType temp(index, name, drawRect, baseRect, selectionCenter,
                       cost, speed, maxFrameCounter, frameCount,
                       maxCombatFrameCounter, combatFrameCount, combatWait,
                       maxHealth, attack, armor, builder, gatherer,
                       originBuilding, progressCost, resourceAtDeath,
                       prereqTech, soundFile, deathSoundFile, hitSoundFile);
         game.unitTypes.push_back(temp);
      }else if (object == "resourceNodeType"){
         ResourceNodeType temp(index, name, drawRect, baseRect,
                               selectionCenter, maxResources, yield,
                               EntityColor(color));
         game.resourceNodeTypes.push_back(temp);
      }else if (object == "technology"){
         Tech temp(index, name, bonus, originBuilding, cost, prereqTech,
                   prereqTech2);
         game.techs.push_back(temp);
      }

   }
   data.close();

   Resources::init(resourceNames);

   //check data
   //indices are ordered
   for (typeNum_t i = 0; i != game.buildingTypes.size(); ++i)
      assert (game.buildingTypes[i].getIndex() == i);
   for (typeNum_t i = 0; i != game.decorationTypes.size(); ++i)
      assert (game.decorationTypes[i].getIndex() == i);
   for (typeNum_t i = 0; i != game.unitTypes.size(); ++i)
      assert (game.unitTypes[i].getIndex() == i);
   for (typeNum_t i = 0; i != game.resourceNodeTypes.size(); ++i)
      assert (game.resourceNodeTypes[i].getIndex() == i);
   for (typeNum_t i = 0; i != game.techs.size(); ++i)
      assert (game.techs[i].getIndex() == i);
   
   
//TODO check all prereqs, originBuildings etc. are in-bounds or NO_TYPE
}
