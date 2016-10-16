// (C) 2010 Tim Gurto

#include <cassert>
#include "misc.h"
#include "uiBarFunctions.h"
#include "update.h"
#include "GameData.h"
#include "Debug.h"
#include "BuildingType.h"
#include "Building.h"
#include "Point.h"
#include "Unit.h"
#include "CoreData.h"
#include "UIBar.h"

extern Debug debug;

typeNum_t getValidBuilding(const CoreData &core, const GameData &game,
                           typeNum_t i){
   ++i;
   typeNum_t index;
   for (index = 0; i != 0; ++index)
      if (game.validBuilding(HUMAN_PLAYER, index))
         --i;
   return index - 1;
}

typeNum_t getValidUnit(const CoreData &core, const GameData &game,
                       typeNum_t i){
   ++i;
   typeNum_t index;
   for (index = 0; i != 0; ++index)
      if (game.validUnit(HUMAN_PLAYER, index,
                         game.buildingSelected->getTypeIndex()))
         --i;
   return index -1 ;
}

typeNum_t getValidTech(const CoreData &core, const GameData &game,
                       typeNum_t i){
   ++i;
   typeNum_t index;
   for (index = 0; i != 0; ++index)
      if (game.validTech(HUMAN_PLAYER, index,
                         game.buildingSelected->getTypeIndex()))
         --i;
   return index - 1;
}


//UIBar::iconCountFun_
//Returns the number of icons in the bar
   
   //Buildings bar: number of BuildingType icons
   typeNum_t getNumBuildingIcons(const CoreData &core, const GameData &game){
      if (game.mode != MODE_BUILDER)
         return 0;
      typeNum_t count = 0;
      for (typeNum_t index = 0; index != core.buildingTypes.size(); ++index)
         if (game.validBuilding(HUMAN_PLAYER, index))
            ++count;
      return count;
   }

   //Units bar: 
   typeNum_t getNumUnitIcons(const CoreData &core, const GameData &game){
      if (game.mode != MODE_BUILDING)
         return 0;
      typeNum_t count = 0;
      for (typeNum_t index = 0; index != core.unitTypes.size(); ++index)
         if (game.validUnit(HUMAN_PLAYER, index,
                            game.buildingSelected->getTypeIndex()))
            ++count;
      return count;
   }

   //Techs bar:
   typeNum_t getNumTechIcons(const CoreData &core, const GameData &game){
      if (!game.buildingSelected)
         return 0;
      typeNum_t count = 0;
      for (typeNum_t index = 0; index != core.techs.size(); ++index)
         if (game.validTech(HUMAN_PLAYER, index,
                            game.buildingSelected->getTypeIndex()))
            ++count;
      return count;
   }



//UIBar::surfaceFun_
//Returns the icon surface for the specified index

   const Surface &getBuildingTypeIcons(typeNum_t i, const CoreData &core,
                                       const GameData &game){
      return core.buildingTypes[getValidBuilding(core, game, i)].getIcon();
   }

   const Surface &getUnitTypeIcons(typeNum_t i, const CoreData &core,
                                   const GameData &game){
      return core.unitTypes[getValidUnit(core, game, i)].getIcon();
   }

   const Surface &getTechIcons(typeNum_t i, const CoreData &core,
                               const GameData &game){
      return core.techs[getValidTech(core, game, i)].getIcon();
   }



//UIBar::clickFun_
//Executes the click on the specified button

   void selectBuilding(typeNum_t index, const CoreData &core,
                       GameData &game){
      
      UIBar::clickSound();
      game.toBuild = getValidBuilding(core, game, index);
      game.mode = MODE_CONSTRUCTION;
   }

   void trainUnit(typeNum_t index, const CoreData &core,
                  GameData &game){
      game.trainUnit(getValidUnit(core, game, index),
                     *game.buildingSelected,
                     HUMAN_PLAYER);
   }

   void researchTech(typeNum_t index, const CoreData &core,
                     GameData &game){
      game.researchTech(getValidTech(core, game, index), HUMAN_PLAYER);
   }

//UIBar::helpFun_
//Returns relevant context help text for the specified button

   std::string buildingHelp(typeNum_t index, const CoreData &core,
                            GameData &game){
      const BuildingType &type =
         core.buildingTypes[getValidBuilding(core, game, index)];
      return
         "Build " + type.getName() +
         ": " + type.getCostString();
   }

   std::string unitHelp(typeNum_t index, const CoreData &core,
                        GameData &game){
      const UnitType &type =
         core.unitTypes[getValidUnit(core, game, index)];
      return
         "Train " + type.getName() +
         ": " + type.getCostString();
      //return "";
   }

   std::string techHelp(typeNum_t index, const CoreData &core,
                        GameData &game){
      const Tech &tech = core.techs[getValidTech(core, game, index)];
      return
         "Research " + tech.getName() +
         ": " + tech.getCostString() +
         ": " + tech.getDescription();
   }