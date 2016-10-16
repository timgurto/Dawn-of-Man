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
#include "CoreData.h"

extern Debug debug;
//extern Debug deltaLog;

//higher = slower game speed.
//Game is normalized around this delta.
const int DELTA_MODIFIER = 40; //40;

void gameMode(){

   //initialize screen and debug objects
   SDL_Surface *screen = setScreen();
   debug.initScreen(screen);

   //seed random generator
   srand(unsigned(time(0)));

   //load core data (must be done after screen is initialized)
   CoreData core(DATA_PATH + "core.dat");

   //init
   GameData::init(&core);
   Entity::init(&core);
   Player::init(&core);

   //load surfaces
   SDL_Surface
      *map = loadImage(MISC_IMAGE_PATH + "back.png"),
      *darkMap = loadImage(MISC_IMAGE_PATH + "dark.png"),
      *cursor = loadImage(MISC_IMAGE_PATH + "cursor.png", GREEN),
      *cursorShadow = loadImage(MISC_IMAGE_PATH +
                                "cursorShadow.PNG", GREEN),
      *cursorPause = loadImage(MISC_IMAGE_PATH +
                               "cursorPause.PNG", GREEN),
      *cursorColor = loadImage(MISC_IMAGE_PATH +
                               "cursorColor.PNG", MAGENTA),
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

   //colored cursors, generated as they're used
   SDL_Surface *cursorIndex[CLR_MAX];
   for (int i = 0; i != CLR_MAX; ++i)
      cursorIndex[i] = 0;

   std::string musicPath = SOUND_PATH + "twoSteps.wav";
   Mix_Music *music = Mix_LoadMUS(musicPath.c_str());
   if (!DEBUG)
      Mix_PlayMusic(music, -1);

   SDL_ShowCursor(SDL_DISABLE);

   //create game data object, and load data from file
   GameData game(DATA_PATH + "game.dat");
   
   //more init
   Particle::init(screen, particle, particleShadow);
   Entity::init(&core, &game, screen, diagGreen);

   //UI Bars
   UIBars_t bars;
   UIBar::init(&core, &game, screen, vBar, hBar);
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
   Player::init(&core, &game, &buildingsBar);

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

   timer_t oldTicks = SDL_GetTicks();
   game.loop = true;
   while (game.loop){

      //time stuff
      timer_t newTicks = SDL_GetTicks();
      timer_t delta = newTicks - oldTicks;
      oldTicks = newTicks;
      double deltaMod = 1.0 * delta / DELTA_MODIFIER;
      
      double fps = delta == 0 ? 0 : 1000 / delta;
      fpsDisplay(format3(fps), "fps  |  ", delta, "ms ");

      //update state
      updateState(deltaMod, core, game, screen, bars,
                  contextHelp, resourcesBox, fpsDisplay);

      //render
      render(screen, glow, diagGreen, diagRed, map, darkMap, cursor,
             cursorShadow, cursorPause, cursorColor, cursorIndex, core,
             game, bars, messageBoxes);

   }

   //Clean up
   SDL_ShowCursor(SDL_ENABLE);

   freeSurface(map);
   freeSurface(darkMap);
   freeSurface(cursor);
   freeSurface(cursorShadow);
   freeSurface(cursorPause);
   freeSurface(cursorColor);
   freeSurface(vBar);
   freeSurface(hBar);
   freeSurface(particle);
   freeSurface(particleShadow);
   freeSurface(diagGreen);
   freeSurface(diagRed);
   freeSurface(entitiesTemp);
   freeSurface(glow);

   for (int i = 0; i != CLR_MAX; ++i)
      freeSurface(cursorIndex[i]);

   Mix_HaltMusic(); 
   Mix_FreeMusic(music);

}

void addEntity(GameData &game, Entity *entity){
   game.entities.insert(std::lower_bound(game.entities.begin(),
                                         game.entities.end(), entity,
                                         dereferenceLessThan),
                        entity);
}