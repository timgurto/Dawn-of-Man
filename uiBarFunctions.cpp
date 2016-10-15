// (C) 2010 Tim Gurto

#include <cassert>
#include "uiBarFunctions.h"
#include "GameData.h"
#include "Debug.h"
#include "BuildingType.h"
#include "Building.h"
#include "Point.h"
#include "Unit.h"
#include "game.h"

extern Debug debug;

const int NUM_PLACEMENT_TRIES = 40;
const pixels_t PLACEMENT_MARGIN = 1;



//UIBar::iconCountFun_
//Returns the number of icons in the bar
   
   //Buildings bar: number of BuildingType icons
   typeNum_t getNumBuildingIcons(const GameData &game){
      return game.buildingTypes.size();
   }

   //Units bar: 
   typeNum_t getNumUnitIcons(const GameData &game){
      if (game.buildingSelected == 0)
         return 0;
      typeNum_t count = 0;
      for (unitTypes_t::const_iterator it = game.unitTypes.begin();
           it != game.unitTypes.end(); ++it)
         //if unit comes from this building
         if (it->getOriginBuilding() == game.buildingSelected->getTypeIndex())
            ++count;
      return count;
   }

   //Techs bar:
   typeNum_t getNumTechIcons(const GameData &game){
      if (game.buildingSelected  == 0)
         return 0;
      typeNum_t count = 0;
      for (techs_t::const_iterator it = game.techs.begin();
           it != game.techs.end(); ++it)
         //if tech comes from this building,
         if (it->getOriginBuilding() == game.buildingSelected->getTypeIndex() &&
             //and hasn't been researched yet
             !game.players[HUMAN_PLAYER].isTechResearched(it->getIndex()))
            ++count;
      return count;
   }



//UIBar::surfaceFun_
//Returns the icon surface for the specified index

   SDL_Surface *getBuildingTypeIcons(typeNum_t i,
                                     typeNum_t size,
                                     const GameData &game){
      assert (i < size);
      return game.buildingTypes[i].getIcon();
   }

   SDL_Surface *getUnitTypeIcons(typeNum_t i,
                                 typeNum_t size,
                                 const GameData &game){
      assert (i < size);
      Building &building = *game.buildingSelected;
      int count = 0;
      //find units that come from this building
      for (typeNum_t loop = 0; loop != game.unitTypes.size(); ++loop)
         if (game.unitTypes[loop].getOriginBuilding() ==
             building.getTypeIndex()){
            if (count == i)
               return game.unitTypes[loop].getIcon();
            ++count;
         }
      assert (false);
      return 0;
   }

   SDL_Surface *getTechIcons(typeNum_t i,
                             typeNum_t size,
                             const GameData &game){
      assert (i < size);
      Building &building = *game.buildingSelected;
      int count = 0;
      //find techs that come from this building
      for (typeNum_t loop = 0; loop != game.techs.size(); ++loop)
         if (game.techs[loop].getOriginBuilding() == building.getTypeIndex() &&
             !game.players[HUMAN_PLAYER].isTechResearched(loop)){
            if (count == i)
               return game.techs[loop].getIcon();
            ++count;
         }
      assert (false);
      return 0;
   }



//UIBar::clickFun_
//Executes the click on the specified button

   void selectBuilding(typeNum_t index, GameData &game){
      game.toBuild = index;
      debug("Building to construct: ",
            game.buildingTypes[index].getName());
      game.mode = MODE_CONSTRUCTION;
   }

   void trainUnit(typeNum_t index, GameData &game){
      Building &building = *game.buildingSelected;

      //get type
      typeNum_t count = 0, typeIndex = 0;
      for (typeIndex = 0; typeIndex != game.unitTypes.size(); ++typeIndex)
         if (game.unitTypes[typeIndex].getOriginBuilding() ==
             building.getTypeIndex()){
            if (count == index)
               break;
            ++count;
         }
      
      const UnitType &unitType = game.unitTypes[typeIndex];

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
               subtractResources(game.unitTypes[typeIndex].getCost());
         }else
            debug("Unable to place unit");
      }else //insufficient resources
         debug("Insufficient resources");
   }

   void researchTech(typeNum_t index, GameData &game){
      Building &building = *game.buildingSelected;

      //get type
      typeNum_t count = 0, typeIndex = 0;
      for (typeIndex = 0; typeIndex != game.techs.size(); ++typeIndex)
         if (game.unitTypes[typeIndex].getOriginBuilding() == building.getTypeIndex() &&
             !game.players[HUMAN_PLAYER].isTechResearched(typeIndex)){
            if (count == index)
               break;
            ++count;
         }
      
      //check player's resources
      if (game.players[HUMAN_PLAYER].
             sufficientResources(game.techs[typeIndex].getCost())){

         //research tech
         game.players[HUMAN_PLAYER].
            subtractResources(game.techs[typeIndex].getCost());
         game.players[HUMAN_PLAYER].
            researchTech(typeIndex);

      }else //insufficient resources
         debug("Insufficient resources");
   }

//UIBar::helpFun_
//Returns relevant context help text for the specified button

   std::string buildingHelp(typeNum_t index,
                            GameData &game){
      return
         "Build " +
         game.buildingTypes[index].getName() +
         ": " +
         game.buildingTypes[index].getCostString();
   }

   std::string unitHelp(typeNum_t index,
                        GameData &game){
      const Building &building = *game.buildingSelected;
      int count = 0;
      //find units that come from this building
      for (typeNum_t loop = 0; loop != game.unitTypes.size(); ++loop)
         if (game.unitTypes[loop].getOriginBuilding() ==
             building.getTypeIndex()){
            if (count == index)
               return
                  "Train " +
                  game.unitTypes[loop].getName() +
                  ": " +
                  game.unitTypes[loop].getCostString();
            ++count;
         }
      assert(false);
      return "";
   }

   std::string techHelp(typeNum_t index,
                        GameData &game){
      const Building &building = *game.buildingSelected;
      int count = 0;
      //find units that come from this building
      for (typeNum_t loop = 0; loop != game.techs.size(); ++loop)
         if (game.techs[loop].getOriginBuilding() ==
             building.getTypeIndex() &&
             !game.players[HUMAN_PLAYER].isTechResearched(loop)){
            if (count == index)
               return
                  "Research " +
                  game.techs[loop].getName() +
                  ": " +
                  game.techs[loop].getCostString();
            ++count;
         }
      assert(false);
      return "";
   }