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
   typeNum_t getNumBuildingIcons(const GameData &game){
      return game.buildingTypes.size();
   }

   //Units bar: 
   typeNum_t getNumUnitIcons(const GameData &game){
      typeNum_t count = 0;
      if (game.buildingSelected != 0)
         for (unitTypes_t::const_iterator it = game.unitTypes.begin();
              it != game.unitTypes.end(); ++it)
            if (it->getOriginBuilding() == game.buildingSelected->getTypeIndex())
               ++count;
      return count;
   }


//UIBar::surfaceFun_

   SDL_Surface *getBuildingTypeIcons(typeNum_t i,
                                     typeNum_t size,
                                     const GameData &game){
      return game.buildingTypes[i].getIcon();
   }

   SDL_Surface *getUnitTypeIcons(typeNum_t i,
                                 typeNum_t size,
                                 const GameData &game){
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

   void selectBuilding(typeNum_t index, typeNum_t size, GameData &game){
      game.toBuild = index;
      debug("Building to construct: ",
            game.buildingTypes[game.toBuild].getName());
      game.mode = MODE_CONSTRUCTION;
   }