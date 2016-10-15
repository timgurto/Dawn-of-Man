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

const int NUM_PLACEMENT_TRIES = 20;
const pixels_t PLACEMENT_MARGIN = 2;



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
      if (game.buildingSelected != 0)
         for (unitTypes_t::const_iterator it = game.unitTypes.begin();
              it != game.unitTypes.end(); ++it)
            if (it->getOriginBuilding() == game.buildingSelected->getTypeIndex())
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
      for (typeNum_t loop = 0;
           loop != game.unitTypes.size(); ++loop){
         if (game.unitTypes[loop].getOriginBuilding() == building.getTypeIndex()){
            if (count == i)
               return game.unitTypes[loop].getIcon();
            ++count;
         }
      }
      assert (false);
      return 0;
   }



//UIBar::clickFun_
//Executes the click on the specified button

   void selectBuilding(typeNum_t index, GameData &game){
      game.toBuild = index;
      debug("Building to construct: ",
            game.buildingTypes[game.toBuild].getName());
      game.mode = MODE_CONSTRUCTION;
   }

   void trainUnit(typeNum_t index, GameData &game){
      Building &building = *game.buildingSelected;

      //get type
      typeNum_t typeIndex, loop;
      for (typeIndex = 0, loop = 0;
           typeIndex != game.unitTypes.size() && loop != index;
           ++typeIndex){
         if (game.unitTypes[typeIndex].getOriginBuilding() == building.getTypeIndex())
            ++loop;
      }
      //debug("Unit to train: ", game.unitTypes[typeIndex].getName());
      
      //find location
      Point loc;
      const UnitType &unitType = game.unitTypes[typeIndex];

      int tries = 0;
      do{
         ++tries;
         Direction dir = Direction(rand() % DIR_MAX);
         SDL_Rect
            buildingBaseRect = building.getBaseRect(),
            unitTypeBaseRect = unitType.getBaseRect();

         //TODO fix; right and down are too close
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
                    unitTypeBaseRect.y -
                    PLACEMENT_MARGIN;
            break;
         case DIR_RIGHT:
            loc.x = buildingBaseRect.x +
                    buildingBaseRect.w -
                    unitTypeBaseRect.x +
                    PLACEMENT_MARGIN;
         }

         if (dir == DIR_UP || dir == DIR_DOWN)
            loc.x = rand() % (buildingBaseRect.w + unitTypeBaseRect.w) +
                    buildingBaseRect.x -
                    unitTypeBaseRect.w +
                    unitTypeBaseRect.x;
         else
            loc.y = rand() % (buildingBaseRect.h + unitTypeBaseRect.h) +
                    buildingBaseRect.y -
                    unitTypeBaseRect.h +
                    unitTypeBaseRect.y;
      }while (tries != NUM_PLACEMENT_TRIES &&
              !noCollision(game, unitType, loc));

      if (tries != NUM_PLACEMENT_TRIES){
         //place unit
         Unit *unit = new Unit(typeIndex, loc);
         addEntity(game, unit);
      }else
         debug("Unable to place unit");
   }