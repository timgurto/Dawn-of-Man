// (C) 2010 Tim Gurto

#include <cassert>
#include "AI.h"
#include "Resources.h"
#include "CoreData.h"
#include "GameData.h"
#include "BuildingType.h"
#include "UnitType.h"
#include "Tech.h"

struct CoreData;
struct GameData;

GameData *AI::game_ = 0;
const CoreData *AI::core_ = 0;

void AI::updateCaps(){
   for (wishlist_t::const_iterator it = wishlist_.begin();
        it != wishlist_.end(); ++it){
      Resources cost;
      bool military = false; //whether this item is a military expense
      switch(it->first){
      case ENT_BUILDING:
         cost = core_->buildingTypes[it->second].getCost();
         break;
      case ENT_TECH:
         cost = core_->techs[it->second].getCost();
         break;
      case ENT_UNIT:
         { //new scope for unitType
            const UnitType &unitType = core_->unitTypes[it->second];
            cost = unitType.getCost();
            if (!(unitType.isBuilder() || unitType.isGatherer()))
               military = true;
         }
      default:
         assert(false);
      }
      //which cap to add to
      Resources &cap = military ?
                       militarySupply_ :
                       expansionSupply_;
      if (cap < cost)
         cap += (cost - cap);
      military;
      cap;
   }
   
}

void AI::allocateIncome(const Resources income){
   
}