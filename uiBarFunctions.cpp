// (C) 2010 Tim Gurto

#include <cassert>
#include "uiBarFunctions.h"
#include "update.h"
#include "GameData.h"
#include "Debug.h"
#include "BuildingType.h"
#include "Building.h"
#include "Point.h"
#include "Unit.h"
#include "CoreData.h"

extern Debug debug;

const int NUM_PLACEMENT_TRIES = 40;
const pixels_t PLACEMENT_MARGIN = 1;


bool validBuilding(const CoreData &core,
                   const GameData &game, typeNum_t i){
   if (i == NO_TYPE)
      return false;
   const BuildingType &type = core.buildingTypes[i];
   const Player &human = game.players[HUMAN_PLAYER];
   return
      //prerequisites
      human.hasTech(type.getPrereqTech()) &&
      human.hasBuilding(type.getPrereqBuilding());
}

bool validUnit(const CoreData &core,
               const GameData &game, typeNum_t i){
   if (i == NO_TYPE)
      return false;
   const UnitType &type = core.unitTypes[i];
   return
      //unit comes from the selected building
      game.buildingSelected &&
      game.buildingSelected->getTypeIndex() == type.getOriginBuilding() &&
      //prerequisites
      game.players[HUMAN_PLAYER].hasTech(type.getPrereqTech());
}

bool validTech(const CoreData &core,
               const GameData &game, typeNum_t i){
   if (i == NO_TYPE)
      return false;
   typeNum_t techIndex = i;
   const Tech &tech = core.techs[techIndex];
   const Player &human = game.players[HUMAN_PLAYER];
   return
      //tech comes from the selected building
      game.buildingSelected &&
      game.buildingSelected->getTypeIndex() == tech.getOriginBuilding() &&
      //prerequisites
      human.hasTech(tech.getPrereqTech1()) &&
      human.hasTech(tech.getPrereqTech2()) &&
      //player hasn't researched it yet
      !human.hasTech(techIndex);
}

typeNum_t getValidBuilding(const CoreData &core, const GameData &game,
                           typeNum_t i){
   typeNum_t index;
   for (index = 0; i != 0; ++index)
      if (validBuilding(core, game, index))
         --i;
   return index;
}

typeNum_t getValidUnit(const CoreData &core, const GameData &game,
                       typeNum_t i){
   typeNum_t index;
   for (index = 0; i != 0; ++index)
      if (validUnit(core, game, index))
         --i;
   return index;
}

typeNum_t getValidTech(const CoreData &core, const GameData &game,
                       typeNum_t i){
   typeNum_t index;
   for (index = 0; i != 0; ++index)
      if (validTech(core, game, index))
         --i;
   return index;
}


//UIBar::iconCountFun_
//Returns the number of icons in the bar
   
   //Buildings bar: number of BuildingType icons
   typeNum_t getNumBuildingIcons(const CoreData &core, const GameData &game){
      if (game.mode != MODE_BUILDER)
         return 0;
      typeNum_t count = 0;
      for (typeNum_t index = 0; index != core.buildingTypes.size(); ++index)
         if (validBuilding(core, game, index))
            ++count;
      return count;
   }

   //Units bar: 
   typeNum_t getNumUnitIcons(const CoreData &core, const GameData &game){
      if (game.mode != MODE_BUILDING)
         return 0;
      typeNum_t count = 0;
      for (typeNum_t index = 0; index != core.unitTypes.size(); ++index)
         if (validUnit(core, game, index))
            ++count;
      return count;
   }

   //Techs bar:
   typeNum_t getNumTechIcons(const CoreData &core, const GameData &game){
      if (!game.buildingSelected)
         return 0;
      typeNum_t count = 0;
      for (techs_t::const_iterator it = core.techs.begin();
           it != core.techs.end(); ++it)
         //if tech comes from this building,
         if (it->getOriginBuilding() == game.buildingSelected->getTypeIndex() &&
             //and hasn't been researched yet
             !game.players[HUMAN_PLAYER].hasTech(it->getIndex()))
            ++count;
      return count;
   }



//UIBar::surfaceFun_
//Returns the icon surface for the specified index

   SDL_Surface *getBuildingTypeIcons(typeNum_t i, const CoreData &core,
                                     const GameData &game){
      return core.buildingTypes[getValidBuilding(core, game, i)].getIcon();
   }

   SDL_Surface *getUnitTypeIcons(typeNum_t i, const CoreData &core,
                                 const GameData &game){
      return core.unitTypes[getValidUnit(core, game, i)].getIcon();
   }

   SDL_Surface *getTechIcons(typeNum_t i, const CoreData &core,
                             const GameData &game){
      return core.techs[getValidTech(core, game, i)].getIcon();
   }



//UIBar::clickFun_
//Executes the click on the specified button

   void selectBuilding(typeNum_t index, const CoreData &core,
                       GameData &game){
      game.toBuild = getValidBuilding(core, game, index);
      game.mode = MODE_CONSTRUCTION;
   }

   void trainUnit(typeNum_t index, const CoreData &core,
                  GameData &game){
      Building &building = *game.buildingSelected;

      //get type
      typeNum_t typeIndex = getValidUnit(core, game, index);      
      const UnitType &unitType = core.unitTypes[typeIndex];

      //check player's resources
      if (game.players[HUMAN_PLAYER].
             sufficientResources(unitType.getCost())){

         //find location
         Point loc;

         int tries = 0;
         do{
            ++tries;
            Direction dir = Direction(rand() % DIR_MAX);
            SDL_Rect
               buildingBaseRect = building.getBaseRect(),
               unitTypeBaseRect = unitType.getBaseRect();

            switch(dir){
            case DIR_UP:
               loc.y = buildingBaseRect.y -
                       unitTypeBaseRect.h -
                       unitTypeBaseRect.y -
                       PLACEMENT_MARGIN;
               break;
            case DIR_DOWN:
               loc.y = buildingBaseRect.y +
                       buildingBaseRect.h -
                       unitTypeBaseRect.y +
                       PLACEMENT_MARGIN;
               break;
            case DIR_LEFT:
               loc.x = buildingBaseRect.x -
                       unitTypeBaseRect.w -
                       unitTypeBaseRect.x -
                       PLACEMENT_MARGIN;
               break;
            case DIR_RIGHT:
               loc.x = buildingBaseRect.x +
                       buildingBaseRect.w -
                       unitTypeBaseRect.x +
                       PLACEMENT_MARGIN;
            }

            if (dir == DIR_UP || dir == DIR_DOWN)
               loc.x = rand() % (buildingBaseRect.w +
                                 unitTypeBaseRect.w) +
                       buildingBaseRect.x -
                       unitTypeBaseRect.w -
                       unitTypeBaseRect.x;
            else
               loc.y = rand() % (buildingBaseRect.h +
                                 unitTypeBaseRect.h) +
                       buildingBaseRect.y -
                       unitTypeBaseRect.h -
                       unitTypeBaseRect.y;

         }while (tries != NUM_PLACEMENT_TRIES &&
                 !noCollision(game, unitType, loc));

         if (tries != NUM_PLACEMENT_TRIES){
            //place unit
            Unit *unit = new Unit(typeIndex, loc);
            addEntity(game, unit);
            game.players[HUMAN_PLAYER].
               subtractResources(core.unitTypes[typeIndex].getCost());
         }else
            debug("Unable to place unit");
      }else //insufficient resources
         debug("Insufficient resources");
   }

   void researchTech(typeNum_t index, const CoreData &core,
                     GameData &game){
      Player &human = game.players[HUMAN_PLAYER];

      //get type
      typeNum_t techIndex = getValidTech(core, game, index);
      const Tech &tech = core.techs[techIndex];
      
      //check player's resources
      if (human.sufficientResources(tech.getCost())){

         //research tech
         human.subtractResources(tech.getCost());
         human.researchTech(techIndex);

      }else //insufficient resources
         debug("Insufficient resources");
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
         ": " + tech.getCostString();
   }