// (C) 2009-2010 Tim Gurto

#include <cassert>
#include <map>
#include <string>

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
#include "Surface.h"
#include "Screen.h"

extern Debug debug;
//extern Debug deltaLog;

//higher = slower game speed.
//Game is normalized around this delta.
const int DELTA_MODIFIER = 40; //40;

//if the delta (ms) is more than this, the update is skipped.
const int DELTA_CUTOFF = 100;

//Alpha of the selection rectangle
const Uint8 SELECTION_RECT_ALPHA = 0x66;

//How long the victory/loss message is displayed
const timer_t END_MESSAGE_TIMER = 1250;

unsigned gameMode(Screen &thisScreen, const void *data){
   assert(data);
   const std::string &fileName = *((std::string*)data);

   //loading screen
   Surface loading(MISC_IMAGE_PATH + "loading.PNG");
   renderLoadingScreen(screenBuf, loading);

   //TODO move screen inits to dawn.cpp
   

   //load core data (must be done after screen is initialized)
   CoreData core(DATA_PATH + "core.dat");

   //init
   GameData::init(&core);
   Entity::init(&core);
   Player::init(&core);

   //load surfaces
   //redone for each game screen.  Slower loading each time, but not
   //that bad, and it's clearer this way
   Surface
      map           (MISC_IMAGE_PATH + "back.PNG"),
      darkMap       (MISC_IMAGE_PATH + "dark.PNG"),
      cursor        (MISC_IMAGE_PATH + "cursor.PNG",         GREEN),
      cursorShadow  (MISC_IMAGE_PATH + "cursorShadow.PNG",   GREEN),
      cursorPause   (MISC_IMAGE_PATH + "cursorPause.PNG",    GREEN),
      cursorColor   (MISC_IMAGE_PATH + "cursorColor.PNG",    MAGENTA),
      diagGreen     (MISC_IMAGE_PATH + "diagGreen.PNG",      GREEN),
      diagRed       (MISC_IMAGE_PATH + "diagRed.PNG",        GREEN),
      particle      (MISC_IMAGE_PATH + "particle.PNG",       GREEN),
      particleShadow(MISC_IMAGE_PATH + "particleShadow.PNG", GREEN),
      victory       (MISC_IMAGE_PATH + "victory.PNG",        GREEN),
      loss          (MISC_IMAGE_PATH + "loss.PNG",           GREEN),
      glow          (MISC_IMAGE_PATH + "glow.PNG", true),
      entitiesTemp  (SUR_BLANK, Screen::screenRes.x, Screen::screenRes.y,
                     ENTITY_BACKGROUND),
      selRectImage  (SUR_BLANK);

   //colored cursors, generated as they're used
   Surface cursorIndex[CLR_MAX];

   //shadows' alpha
   particleShadow.setAlpha(SHADOW_ALPHA);
   cursorShadow.setAlpha(SHADOW_ALPHA);
   
   //selection rectangle: translucent white
   selRectImage.fill(WHITE);
   selRectImage.setAlpha(SELECTION_RECT_ALPHA);

   std::string musicPath = SOUND_PATH + "music.wav";
   Mix_Music *music = Mix_LoadMUS(musicPath.c_str());
   if (!DEBUG)
      Mix_PlayMusic(music, -1);

   SDL_Sound *click = loadSound(SOUND_PATH + "interfaceClick.wav");

   SDL_ShowCursor(SDL_DISABLE);

   //create game data object, and load data from file
   GameData game(DATA_PATH + fileName);

   //more init
   Particle::init(&particle, &particleShadow);
   Entity::init(&core, &game, &diagGreen);

   //UI Bars
   UIBars_t bars;
   UIBar::init(&core, &game, &darkMap, click);
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
                          2, Screen::screenRes.y - ICON_SIZE - 1,
                          1,
                          darkMap,
                          "Thor.ttf", 18,
                          true);
   MessageBox resourcesBox(WHITE,
                           2, 2, 1,
                           darkMap,
                           "Thor.ttf", 18);
   MessageBox fpsDisplay(WHITE,
                         Screen::screenRes.x / 2 - 40, 2,
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

      //if delta is too high, skip update.  Adds an instance of lag to the game,
      //but prevents extremes in movment.
      if (delta <= DELTA_CUTOFF){

         double deltaMod = 1.0 * delta / DELTA_MODIFIER;
         
         double fps = delta == 0 ? 0 : 1000 / delta;
         fpsDisplay(format3(fps), "fps  |  ", delta, "ms ");

         //force interface refresh
         pushMouseMove(game);

         //update state
         updateState(deltaMod, core, game, bars,
                     contextHelp, resourcesBox, fpsDisplay);

         //render
         render(glow, diagGreen, diagRed, map, darkMap, cursor,
                cursorShadow, cursorPause, cursorColor, selRectImage,
                cursorIndex, core,
                game, bars, messageBoxes);
      }
   }

   //game over; display victory/loss message
   game.paused = true; //darken map
   thisScreen.mousePos = Point(screenBuf->w, screenBuf->h); //hide cursor
   render(glow, diagGreen, diagRed, map, darkMap, cursor,
          cursorShadow, cursorPause, cursorColor, selRectImage,
          cursorIndex, core,
          game, bars, messageBoxes,
          game.outcome == VICTORY ? //QUIT treated as loss
             victory :
             loss);
   //wait
   if (!DEBUG){
      oldTicks = SDL_GetTicks();
      while (SDL_GetTicks() - oldTicks < END_MESSAGE_TIMER);
   }

   //Clean up
   SDL_ShowCursor(SDL_ENABLE);

   freeSound(click);

   Mix_HaltMusic(); 
   Mix_FreeMusic(music);

   return game.outcome;
}