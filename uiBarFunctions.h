// (C) 2010 Tim Gurto

#ifndef UI_BAR_FUNCTIONS_H
#define UI_BAR_FUNCTIONS_H

//contains the functions used by the UIBar class to fetch icons
//surfaces, and execute actions when clicked.

#include <string>
#include "types.h"

struct SDL_Surface;
struct GameData;
struct CoreData;

//whether the given item is available for the human player
bool validBuilding(const CoreData &core, const GameData &game,
                   typeNum_t i);
bool validUnit(const CoreData &core, const GameData &game,
               typeNum_t i);
bool validTech(const CoreData &core, const GameData &game,
               typeNum_t i);

//fetches the ith valid item's (see above) index
typeNum_t getValidBuilding(const CoreData &core, const GameData &game,
                           typeNum_t i);
typeNum_t getValidUnit(const CoreData &core, const GameData &game,
                       typeNum_t i);
typeNum_t getValidTech(const CoreData &core, const GameData &game,
                       typeNum_t i);



//iconCountFun_
typedef typeNum_t iconCountFun(const CoreData &data,
                               const GameData &game);
typedef iconCountFun *iconCountFunPtr;
   
   //Buildings bar: number of BuildingType icons
   iconCountFun getNumBuildingIcons;

   //Units bar:
   iconCountFun getNumUnitIcons;

   //Techs bar:
   iconCountFun getNumTechIcons;



//surfaceFun_
typedef SDL_Surface *surfaceFun(typeNum_t index, const CoreData &data,
                                const GameData &game);
typedef surfaceFun *surfaceFunPtr;
   
   //Buildings bar: BuildingType icons
   surfaceFun getBuildingTypeIcons;

   //Units bar: UnitType icons, based on the selected building
   surfaceFun getUnitTypeIcons;

   //Techs bar: Tech icons, as above
   surfaceFun getTechIcons;


//TODO enable for all players
//clickFun_
typedef void clickFun(typeNum_t index, const CoreData &data,
                      GameData &game);
typedef clickFun *clickFunPtr;

   //Buildings bar: Selects a building to construct
   clickFun selectBuilding;

   //Units bar: Trains the chosen unit, at the selected building
   clickFun trainUnit;

   //Techs bar: Researches the chosen tech
   clickFun researchTech;



//helpFun_
   typedef std::string helpFun(typeNum_t index, const CoreData &data,
                               GameData &game);
   typedef helpFun *helpFunPtr;

   //Buildings bar: returns the name of the building
   helpFun buildingHelp;

   //Units bar: returns the name of the unit
   helpFun unitHelp;

   //Techs bar: returns the name of the tech
   helpFun techHelp;

#endif