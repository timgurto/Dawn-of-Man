// (C) 2010 Tim Gurto

#include <cassert>
#include "uiBarFunctions.h"
#include "GameData.h"
#include "Debug.h"
#include "BuildingType.h"
#include "Building.h"

extern Debug debug;

//iconCountFun_
   
   //Buildings bar: number of BuildingType icons
   typeNum_t buildingIconCountFun(const GameData &game){
      return game.buildingTypes.size();
   }

   //Units bar: 
   typeNum_t unitIconCountFun(const GameData &game){
      
   }


//UIBar::surfaceFun_

   SDL_Surface *getBuildingTypeIcons(typeNum_t i,
                                     const GameData &game){
      return game.buildingTypes[i].getIcon();
   }

   SDL_Surface *getUnitTypeIcons(typeNum_t i,
                                 const GameData &game){
      Building &building = *game.buildingSelected;
      int count = 0;
      //find units that come from this building
      for (typeNum_t loop = 0;
           loop != game.unitTypes.size(); ++loop){
         if (game.unitTypes[loop].getOriginBuilding() == building.typeIndex){
            if (count == i)
               return game.unitTypes[loop].getIcon();
            ++count;
         }
      }
      assert (false);
      return 0;
   }


//UIBar::clickFun_

   void selectBuilding(typeNum_t index, GameData &game){
      game.toBuild = index;
      debug("Building to construct: ",
            game.buildingTypes[game.toBuild].getName());
      game.mode = MODE_CONSTRUCTION;
   }