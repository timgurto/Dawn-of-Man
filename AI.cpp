// (C) 2010 Tim Gurto

#include <cassert>
#include <cstdlib>
#include "util.h"
#include "AI.h"
#include "Resources.h"
#include "CoreData.h"
#include "GameData.h"
#include "BuildingType.h"
#include "UnitType.h"
#include "Tech.h"
#include "Debug.h"
#include "Building.h"

extern Debug debug;

GameData *AI::game_ = 0;
const CoreData *AI::core_ = 0;
const double AI::allocationRatio_ = 1;

void AI::init(const CoreData *core, GameData *game){
   core_ = core;
   game_ = game;
}

void AI::initPlayer(typeNum_t player){
   player_ = player;
}

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
   }  
}

void AI::allocateIncome(const Resources &income){
   //whether to calculate expansion before military
   //random = fair distribution of remainders
   bool expansionFirst = rand() % 2 == 0;

   Resources expansionShare, militaryShare;
   if (expansionFirst){
      expansionShare = income * (allocationRatio_ / (allocationRatio_ + 1));
      militaryShare = income - expansionShare;
   }else{
      militaryShare = income * (1 / (allocationRatio_ + 1));
      expansionShare = income - militaryShare;
   }
   
   //check against caps
   bool
      expansionFull = expansionShare >= (expansionCap_ - expansionSupply_),
      militaryFull = militaryShare >= (militaryCap_ - militarySupply_);

   //normal allocation
   if (lXor(expansionFull, militaryFull)){
      expansionSupply_ += expansionShare;
      militarySupply_ += militaryShare;

   //one is full
   }else
      if (expansionFull){
         Resources difference = expansionShare - (expansionCap_ - expansionSupply_);
         expansionSupply_ += difference;
         militarySupply_ += (militaryShare + expansionShare - difference);
      }else{
         Resources difference = militaryShare - (militaryCap_ - militarySupply_);
         militarySupply_ += difference;
         expansionSupply_ += (expansionShare + militaryShare - difference);
      }
   debug("Military: ", militarySupply_, "    Expansion: ", expansionSupply_);
}

void AI::checkExpansion(){
   
}

void AI::checkMilitary(){
   //assumption: largest index is the best military unit
   //assumption: one type of military building, one instance of building
   
   //find training grounds
   const Building *militaryBuilding = 0;
   for (entities_t::const_iterator it = game_->entities.begin();
        it != game_->entities.end(); ++it){
      if ((*it)->classID() == ENT_BUILDING){
         militaryBuilding = (const Building *)(*it);
         if (militaryBuilding->getPlayer() == player_)
            if (militaryBuilding->isMilitary()){
               break;
            }
      }
   }
   if (!militaryBuilding){
      debug("No military building found");
      return;
   }
   //check affordable military units
}