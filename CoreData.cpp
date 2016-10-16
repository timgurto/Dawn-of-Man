// (C) 2010 Tim Gurto

#include <cassert>
#include <vector>
#include <map>

#include "CoreData.h"
#include "misc.h"
#include "Debug.h"

extern Debug debug;

CoreData::CoreData(const std::string filename){
   std::ifstream data(filename.c_str());

   typeNum_t resourceCount = 0;
   std::map<std::string, typeNum_t> resourceIndices;
   std::vector<std::string> resourceNames;

   debug("");
   while (data){
      
      //initialize synthesized data
      std::string
         name = "",
         soundFile = "",
         deathSoundFile = "",
         hitSoundFile = "";
      bool
         collides = false,
         builder = false,
         gatherer = false;
      typeNum_t
         index = NO_TYPE,
         prereqBuilding = NO_TYPE,
         prereqTech = NO_TYPE,
         prereqTech2 = NO_TYPE,
         originBuilding = NO_TYPE,
         resourceAtDeath = NO_TYPE;
      damage_t
         maxHealth = 1,
         armor = 0,
         attack = 0;
      pixels_t
         speed = 8;
      progress_t
         progressCost = 0;
      frames_t
         maxFrameCounter = 0,
         frameCount = 1,
         maxCombatFrameCounter = 0,
         combatFrameCount = 1,
         combatWait = 1;
      int
         color = 0;
      SDL_Rect
         drawRect = makeRect(),
         baseRect = makeRect();
      Point
         selectionCenter = Point();
      TechBonuses
         bonus;
      resources_t
         cost(resourceCount, 0),
         maxResources(resourceCount, 0),
         yield(resourceCount, 0);

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
         std::string val = parseToken(data);
         if (val[val.size() - 1] == ',')
            removeLast(val);
         double numVal = 0;
         bool isNum = val[0] != '\"';
         if (isNum)
            numVal = atod(val);
         else
            val = val.substr(1, val.length()-3);

         if (isNum)
            debug(attr + " = #", numVal);
         else
            debug(attr + " = " +  val);

         //special case: keeping track of resources
         if (object == "resource" && attr == "name"){
            resourceIndices[val] = resourceCount;
            ++resourceCount;
         }

         //read attribute into relevant var
         if (attr == "name")
            name = val;
         else if (attr == "soundFile")
            soundFile = val;
         else if (attr == "deathSoundFile")
            deathSoundFile = val;
         else if (attr == "hitSoundFile")
            hitSoundFile = val;
         else if (attr == "collides")
            collides = numVal != 0;
         else if (attr == "builder")
            builder = numVal != 0;
         else if (attr == "gatherer")
            gatherer = numVal != 0;
         else if (attr == "index")
            index = typeNum_t(numVal);
         else if (attr == "prereqBuilding")
            prereqBuilding = typeNum_t(numVal);
         else if (attr == "originBuilding")
            originBuilding = typeNum_t(numVal);
         else if (attr == "resourceAtDeath")
            resourceAtDeath = typeNum_t(numVal);
         else if (attr == "maxHealth")
            maxHealth = damage_t(numVal);
         else if (attr == "armor")
            armor = damage_t(numVal);
         else if (attr == "attack")
            attack = damage_t(numVal);
         else if (attr == "speed")
            speed = pixels_t(numVal);
         else if (attr == "progressCost")
            progressCost = progress_t(numVal);
         else if (attr == "maxFrameCounter")
            maxFrameCounter = frames_t(numVal);
         else if (attr == "frameCount")
            frameCount = frames_t(numVal);
         else if (attr == "maxCombatFrameCounter")
            maxCombatFrameCounter = frames_t(numVal);
         else if (attr == "combatFrameCount")
            combatFrameCount = frames_t(numVal);
         else if (attr == "combatWait")
            combatWait = frames_t(numVal);
         else if (attr == "color")
            color = EntityColor(int(numVal + CLR_DEFAULT));
         else if (attr.substr(0, 9) == "drawRect."){
            char c = attr[9];
            if (c == 'x')
               drawRect.x = pixels_t(numVal);
            else if (c == 'y')
               drawRect.y = pixels_t(numVal);
            else if (c == 'w')
               drawRect.w = pixels_t(numVal);
            else if (c == 'h')
               drawRect.h = pixels_t(numVal);
         }else if (attr.substr(0, 9) == "baseRect."){
            char c = attr[9];
            if (c == 'x')
               baseRect.x = pixels_t(numVal);
            else if (c == 'y')
               baseRect.y = pixels_t(numVal);
            else if (c == 'w')
               baseRect.w = pixels_t(numVal);
            else if (c == 'h')
               baseRect.h = pixels_t(numVal);
         }else if (attr.substr(0, 16) == "selectionCenter."){
            char c = attr[16];
            if (c == 'x')
               selectionCenter.x = pixels_t(numVal);
            else if (c == 'y')
               selectionCenter.y = pixels_t(numVal);
         }else if (attr.substr(0, 16) == "bonus.gathering."){
            typeNum_t i = resourceIndices[attr.substr(16)];
            //ensure gathering bonus is big enough
            while (bonus.gathering.size() < i)
               bonus.gathering.push_back(0);
            bonus.gathering[i] = resource_t(numVal);
         }else if (attr.substr(0, 6) == "bonus."){
            std::string sub = attr.substr(6);
            if (sub == "unitHealth")
               bonus.unitHealth = damage_t(numVal);
            else if (sub == "unitArmor")
               bonus.unitArmor = damage_t(numVal);
            else if (sub == "unitAttack")
               bonus.attack = damage_t(numVal);
            else if (sub == "buildingHealth")
               bonus.buildingHealth = damage_t(numVal);
            else if (sub == "buildingArmor")
               bonus.buildingArmor = damage_t(numVal);
            else if (sub == "unitSpeed")
               bonus.speed = pixels_t(numVal);
            else if (sub == "trainingSpeed")
               bonus.trainingSpeed = progress_t(numVal);
            else if (sub == "buildingSpeed")
               bonus.buildingSpeed = damage_t(numVal);
            else if (sub == "unitArmor")
               bonus.unitArmor = damage_t(numVal);
            else if (sub == "unitArmor")
               bonus.unitArmor = damage_t(numVal);
            else if (sub == "unitArmor")
               bonus.unitArmor = damage_t(numVal);
            else if (sub == "unitArmor")
               bonus.unitArmor = damage_t(numVal);
            else if (sub == "unitArmor")
               bonus.unitArmor = damage_t(numVal);
         }else if (attr.substr(0, 5) == "cost."){
            typeNum_t i = resourceIndices[attr.substr(5)];
            //ensure cost is big enough
            while (cost.size() < i)
               cost.push_back(0);
            cost[i] = resource_t(numVal);
         }else if (attr.substr(0, 13) == "maxResources."){
            typeNum_t i = resourceIndices[attr.substr(13)];
            //ensure maxResources is big enough
            while (maxResources.size() < i)
               maxResources.push_back(0);
            maxResources[i] = resource_t(numVal);
         }else if (attr.substr(0, 6) == "yield."){
            typeNum_t i = resourceIndices[attr.substr(6)];
            //ensure yield is big enough
            while (yield.size() < i)
               yield.push_back(0);
            yield[i] = resource_t(numVal);
         }

      }
      debug("");

      //initialize object
      if (object == "resource"){
         resourceNames.push_back(name);
         while (cost.size() < resourceNames.size())
            cost.push_back(0);
      }else if (object == "buildingType"){
         BuildingType temp(index, name, drawRect, baseRect,
                           selectionCenter, cost, maxHealth, armor,
                           progressCost, prereqBuilding, prereqTech,
                           soundFile, deathSoundFile);
         buildingTypes.push_back(temp);
      }else if (object == "decorationType"){
         DecorationType temp(index, name, drawRect, baseRect, collides);
         decorationTypes.push_back(temp);
      }else if (object == "unitType"){
         UnitType temp(index, name, drawRect, baseRect, selectionCenter,
                       cost, speed, maxFrameCounter, frameCount,
                       maxCombatFrameCounter, combatFrameCount, combatWait,
                       maxHealth, attack, armor, builder, gatherer,
                       originBuilding, progressCost, resourceAtDeath,
                       prereqTech, soundFile, deathSoundFile, hitSoundFile);
         unitTypes.push_back(temp);
      }else if (object == "resourceNodeType"){
         ResourceNodeType temp(index, name, drawRect, baseRect,
                               selectionCenter, maxResources, yield,
                               EntityColor(color));
         resourceNodeTypes.push_back(temp);
      }else if (object == "technology"){
         Tech temp(index, name, bonus, originBuilding, cost, prereqTech,
                   prereqTech2);
         techs.push_back(temp);
      }

   }
   data.close();

   Resources::init(resourceNames);

   //check data
   //indices are ordered
   for (typeNum_t i = 0; i != buildingTypes.size(); ++i)
      assert (buildingTypes[i].getIndex() == i);
   for (typeNum_t i = 0; i != decorationTypes.size(); ++i)
      assert (decorationTypes[i].getIndex() == i);
   for (typeNum_t i = 0; i != unitTypes.size(); ++i)
      assert (unitTypes[i].getIndex() == i);
   for (typeNum_t i = 0; i != resourceNodeTypes.size(); ++i)
      assert (resourceNodeTypes[i].getIndex() == i);
   for (typeNum_t i = 0; i != techs.size(); ++i)
      assert (techs[i].getIndex() == i);
   
//TODO check all prereqs, originBuildings etc. are in-bounds or NO_TYPE
}