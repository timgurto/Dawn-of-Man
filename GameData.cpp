// (C) 2010 Tim Gurto

#include <fstream>
#include <cassert>
#include <string>
#include <map>
#include "types.h"
#include "update.h"
#include "util.h"
#include "misc.h"
#include "GameData.h"
#include "Debug.h"
#include "Building.h"
#include "Decoration.h"
#include "Unit.h"
#include "ResourceNode.h"
#include "CoreData.h"
#include "Resources.h"

extern Debug debug;

const int NUM_PLACEMENT_TRIES = 40;
const pixels_t PLACEMENT_MARGIN = 1;

const CoreData *GameData::core_ = 0;

GameData::GameData(std::string filename):
//initialize data that can be initialized at this point
//(including stuff which should be replaced by file data,
//just in case it isn't)
rightMouse(),
mapX(1),
mapY(0),
paused(false),
toBuild(NO_TYPE),
mode(MODE_NORMAL),
cursorColor(CLR_MAX), //no color
recalcBars(false),
selectionChanged(false),
buildingSelected(0),
outcome(IN_PROGRESS),
loop(true),
buildLocationOK(false),
scrollLockX(false),
scrollLockY(false){

   //load data from file
   std::ifstream data(filename.c_str());

   typeNum_t resourceCount = 0;
   std::map<std::string, typeNum_t> resourceIndices;
   std::vector<std::string> resourceNames;

   debug("");
   while (data){
      
      //initialize synthesized data
      int
         x = 1, //map size defaults are 1, so that there's
         y = 1, //definitely a map
         random = 0;
      Uint32
         color = ENTITY_DEFAULT_COLOR;
      typeNum_t
         type = NO_TYPE,
         player = NATURE_PLAYER, //what should default be?
         index = NO_TYPE;
      progress_t
         //we don't know what the default (full) is yet
         progress = DUMMY_PROGRESS;
      bool
         centerScreen = false;
      checklist_t
         techsResearched(core_->techs.size(), false),
         buildingsBuilt(core_->buildingTypes.size(), false);
      Point
         loc;
      resources_t
         resources(Resources::getResourceCount(), 0);


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
         std::string strVal = parseToken(data);
         double val = 0;
         val = atod(strVal);
         debug(attr + " = #", val);

         //read attribute into relevant var
         if (attr == "x")
            x = int(val);
         else if (attr == "y")
            y = int(val);
         else if (attr == "random")
            random = int(val);
         else if (attr == "color")
            color = Uint32(val);
         else if (attr == "type")
            type = typeNum_t(val);
         else if (attr == "player")
            player = typeNum_t(val);
         else if (attr == "index")
            index = typeNum_t(val);
         else if (attr == "centerMap")
            centerScreen = strVal == "true;";
         else if (attr == "techResearched")
            techsResearched[size_t(val)] = true;
         else if (attr.substr(0, 9) == "location."){
            char sub = attr[9];
            if (sub == 'x')
               loc.x = pixels_t(val);
            else if (sub == 'y')
               loc.y = pixels_t(val);
         }else if (attr.substr(0, 10) == "resources."){
            typeNum_t i = Resources::findResource(attr.substr(10));
            resources[i] = resource_t(val);
         }

      }
      debug("");

      //initialize object
      if (object == "map"){
         mapX = x;
         mapY = y;
         Uint16
            w = mapX * MAP_TILE_SIZE,
            h = mapY * MAP_TILE_SIZE;
         //center map
         map = makeRect((Screen::getScreenRes().x - w) / 2,
                        (Screen::getScreenRes().y - h) / 2,
                        w, h);

      }else if (object == "building"){
         if (progress == DUMMY_PROGRESS)
            progress = core_->buildingTypes[type].getMaxProgress() + 1;
         if (random != 0)
            for (int i = 0; i != random; ++i){
               do{
                  loc.x = rand() % map.w;
                  loc.y = rand() % map.h;
               }while (!noCollision(*this, core_->buildingTypes[type], loc));
               addEntity(new Building(type, loc, player, progress));
               buildingsBuilt[type] = true;
            }
         else{
            addEntity(new Building(type, loc, player, progress));
            buildingsBuilt[type] = true;
         }
         if (centerScreen)
            centerMap(*this, loc);
         players[player].registerBuilding(type, false);
         loc = loc;

      }else if (object == "decoration"){
         if (random != 0)
            for (int i = 0; i != random; ++i){
               do{
                  loc.x = rand() % map.w;
                  loc.y = rand() % map.h;
               }while (!noCollision(*this, core_->decorationTypes[type], loc));
               addEntity(new Decoration(type, loc));
            }
         else
            addEntity(new Decoration(type, loc));
         if (centerScreen)
            centerMap(*this, loc);

      }else if (object == "unit"){
         if (progress == DUMMY_PROGRESS)
            progress = core_->unitTypes[type].getMaxProgress() + 1;
         if (random != 0)
            for (int i = 0; i != random; ++i){
               do{
                  loc.x = rand() % map.w;
                  loc.y = rand() % map.h;
               }while (!noCollision(*this, core_->unitTypes[type], loc));
               addEntity(new Unit(type, loc, player, progress));
            }
         else
            addEntity(new Unit(type, loc, player, progress));
         if (centerScreen)
            centerMap(*this, loc);
      }else if (object == "resourceNode"){
         if (random != 0)
            for (int i = 0; i != random; ++i){
               do{
                  loc.x = rand() % map.w;
                  loc.y = rand() % map.h;
               }while (!noCollision(*this, core_->resourceNodeTypes[type], loc));
               addEntity(new ResourceNode(type, loc));
            }
         else
            addEntity(new ResourceNode(type, loc));
         if (centerScreen)
            centerMap(*this, loc);
      }else if (object == "player"){
         assert (index != NO_TYPE); //an index must be specified
         players.push_back(Player(color, resources, techsResearched,
                                  buildingsBuilt, players.size()));
      }

   }
   data.close();

   ITERATE(entities_t::const_iterator, entities, it){
      //kill if location not okay?
      //if (!noCollision(*this, (*it)->type(), (*it)->loc_, *it))
      //   (*it)->kill();

      //class-specific stuff
      switch ((*it)->classID()){

      case ENT_BUILDING:
         { //new scope for building and type
            Building &building = *(Building *)(*it);
            const BuildingType &type = core_->buildingTypes[building.typeIndex_];
            checkTypeIndex(building.typeIndex_, core_->buildingTypes.size());
            checkTypeIndex(building.player_, players.size());
            if (building.progress_ > type.getMaxProgress())
               building.progress_ = type.getMaxProgress();
            if (building.health_ > type.getMaxHealth())
               building.health_ = type.getMaxHealth();
         }
         break;

      case ENT_UNIT:
         { //new scope for unit and type
            Unit &unit = *(Unit *)(*it);
            const UnitType &type = core_->unitTypes[unit.typeIndex_];
            checkTypeIndex(unit.typeIndex_, core_->unitTypes.size());
            checkTypeIndex(unit.player_, players.size());
            if (unit.progress_ > type.getMaxProgress())
               unit.progress_ = type.getMaxProgress();
            if (unit.health_ > type.getMaxHealth())
               unit.health_ = type.getMaxHealth();
         }
         break;
      }
   }

}

GameData::~GameData(){

   //entities (delete)
   ITERATE(entities_t::iterator, entities, it)
      delete(*it);
}

void GameData::init(const CoreData *core){
   core_ = core;
}

bool GameData::trainUnit(typeNum_t index,
                         const Building &sourceBuilding,
                         typeNum_t playerID){

   const UnitType &unitType = core_->unitTypes[index];
   Player &player = players[playerID];

   //check player's resources
   if (player.sufficientResources(unitType.getCost())){

      //find location
      Point loc;

      int tries = 0;
      do{
         ++tries;
         Direction dir = Direction(rand() % DIR_MAX);
         SDL_Rect
            buildingBaseRect = sourceBuilding.getBaseRect(),
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
              !noCollision(*this, unitType, loc));

      if (tries != NUM_PLACEMENT_TRIES){
         //place unit
         UIBar::clickSound();
         addEntity(new Unit(index, loc, playerID));
         player.subtractResources(unitType.getCost());
         return true;
      }else
         debug("Unable to place unit");
   }else //insufficient resources
      debug("Insufficient resources to train unit");
   return false;
}

bool GameData::researchTech(typeNum_t index, typeNum_t playerID){

   Player &player = players[playerID];
   const Tech &tech = core_->techs[index];
   
   //check player's resources
   if (player.sufficientResources(tech.getCost())){

      //research tech
      UIBar::clickSound();
      player.subtractResources(tech.getCost());
      player.registerTech(index);
      return true;
   }
   //insufficient resources
   debug("Insufficient resources to research tech");
   return false;
}

bool GameData::constructBuilding(typeNum_t index, const Point &loc,
                                 typeNum_t playerID){

   const BuildingType &buildingType = core_->buildingTypes[index];
   Player &player = players[playerID];

   if (player.sufficientResources(buildingType.getCost())){
      buildingType.getSound().play();

      //Subtract resource cost
      player.subtractResources(buildingType.getCost());

      //create new building
      Building *newBuilding =
         new Building(index, loc, playerID);
      addEntity(newBuilding);

      //assign selected builders to the construction
      if (playerID == HUMAN_PLAYER)
         ITERATE(entities_t::iterator, entities, it)
            if ((*it)->classID() == ENT_UNIT){
               Unit *unitP = (Unit *)(*it);
               if (unitP->selected && unitP->isBuilder())
                     unitP->setTarget(newBuilding);
            }
      return true;
   }
   //not enough resources
   debug("Insufficient resources to construct building");
   return false;

}

bool GameData::validBuilding(typeNum_t playerID, typeNum_t i) const{
   if (i == NO_TYPE)
      return false;
   const BuildingType &type = core_->buildingTypes[i];
   const Player &player = players[playerID];
   return
      //prerequisites
      player.hasTech(type.getPrereqTech()) &&
      player.hasBuilding(type.getPrereqBuilding());
}

bool GameData::validUnit(typeNum_t playerID, typeNum_t i,
                         typeNum_t buildingType) const{
   if (i == NO_TYPE)
      return false;
   const UnitType &type = core_->unitTypes[i];
   return
      //unit comes from the selected building
      buildingType != NO_TYPE &&
      buildingType == type.getOriginBuilding() &&
      //prerequisites
      players[playerID].hasTech(type.getPrereqTech());
}

bool GameData::validTech(typeNum_t playerID, typeNum_t i,
                         typeNum_t buildingType) const{
   if (i == NO_TYPE)
      return false;
   typeNum_t techIndex = i;
   const Tech &tech = core_->techs[techIndex];
   const Player &player = players[playerID];
   return
      //tech comes from the selected building
      buildingType != NO_TYPE &&
      buildingType == tech.getOriginBuilding() &&
      //prerequisites
      player.hasTech(tech.getPrereqTech1()) &&
      player.hasTech(tech.getPrereqTech2()) &&
      //player hasn't researched it yet
      !player.hasTech(techIndex);
}

void GameData::addEntity(Entity *entity){
   entities.insert(std::lower_bound(entities.begin(),
                                    entities.end(), entity,
                                    dereferenceLessThan),
                   entity);
}