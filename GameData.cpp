// (C) 2010 Tim Gurto

#include <fstream>
#include <cassert>
#include <string>
#include <map>
#include "types.h"
#include "update.h"
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

const CoreData *GameData::core_ = 0;

GameData::GameData(std::string filename):
//initialize data that can be initialized at this point
//(including stuff which should be replaced by file data,
//just in case it isn't)
rightMouse(),
mapX(1),
mapY(1),
paused(false),
toBuild(NO_TYPE),
mode(MODE_NORMAL),
cursorColor(CLR_MAX), //no color
recalcBars(false),
selectionChanged(false),
buildingSelected(0),
outcome(IN_PROGRESS){
   //map
   Uint16 w = mapX * MAP_TILE_SIZE;
   Uint16 h = mapY * MAP_TILE_SIZE;
   //center map
   map = makeRect((Screen::screenRes.x - w) / 2,
                  (Screen::screenRes.y - h) / 2,
                  w, h);


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
         techsResearched(core_->techs.size(), false);
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
         w = mapX * MAP_TILE_SIZE;
         h = mapY * MAP_TILE_SIZE;
         //center map
         map = makeRect((Screen::screenRes.x - w) / 2,
                        (Screen::screenRes.y - h) / 2,
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
               addEntity(*this, new Building(type, loc, player, progress));
            }
         else
            addEntity(*this, new Building(type, loc, player, progress));
         if (centerScreen)
            centerMap(*this, loc);
         players[player].buildBuilding(type, false);

      }else if (object == "decoration"){
         if (random != 0)
            for (int i = 0; i != random; ++i){
               do{
                  loc.x = rand() % map.w;
                  loc.y = rand() % map.h;
               }while (!noCollision(*this, core_->decorationTypes[type], loc));
               addEntity(*this, new Decoration(type, loc));
            }
         else
            addEntity(*this, new Decoration(type, loc));
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
               addEntity(*this, new Unit(type, loc, player, progress));
            }
         else
            addEntity(*this, new Unit(type, loc, player, progress));
         if (centerScreen)
            centerMap(*this, loc);
      }else if (object == "resourceNode"){
         if (random != 0)
            for (int i = 0; i != random; ++i){
               do{
                  loc.x = rand() % map.w;
                  loc.y = rand() % map.h;
               }while (!noCollision(*this, core_->resourceNodeTypes[type], loc));
               addEntity(*this, new ResourceNode(type, loc));
            }
         else
            addEntity(*this, new ResourceNode(type, loc));
         if (centerScreen)
            centerMap(*this, loc);
      }else if (object == "player"){
         assert (index != NO_TYPE); //an index must be specified
         players.push_back(Player(color, resources, techsResearched));
      }

   }
   data.close();

   for (entities_t::const_iterator it = entities.begin();
        it != entities.end(); ++it){

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
   for (entities_t::iterator it = entities.begin();
        it != entities.end(); ++it){
      delete(*it);
   }

}

void GameData::init(const CoreData *core){
   core_ = core;
}