// (C) 2010 Tim Gurto

#include <cassert>
#include <cstdlib>
#include <vector>
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
const double AI::allocationRatio_ = 3; //x:1 expansion:military

AI::AI():
militaryBuilding_(0),
player_(NO_TYPE){}

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

void AI::update(){
   checkExpansion();
   checkMilitary();
   buildPossible();
}

void AI::checkExpansion(){
   
}

void AI::checkMilitary(){
   //assumption: largest index is the best military unit
   //assumption: one type of military building, one instance of building
   //if these assumptions are wrong, the AI will be less effective.
   //TODO maintain vector of military buildings, possibly of multiple types, instead

   Player &player = game_->players[player_];
   
   //find training grounds (only necessary once)
   //TODO set to 0 if the building is destroyed
   if (!militaryBuilding_)
      for (entities_t::const_iterator it = game_->entities.begin();
           it != game_->entities.end(); ++it){
         if ((*it)->classID() == ENT_BUILDING){
            militaryBuilding_ = (const Building *)(*it);
            if (militaryBuilding_->getPlayer() == player_)
               if (militaryBuilding_->isMilitary())
                  if (militaryBuilding_->isFinished())
                     break;
         }
         militaryBuilding_ = 0;
      }
   if (!militaryBuilding_){
      debug("No military building found");
      return;
   }
   typeNum_t militaryBuildingType = militaryBuilding_->getTypeIndex();

   //check affordable military units
   //as many highest-level units as possible, then as many of the next level, etc.
   for (unitTypes_t::const_reverse_iterator it = core_->unitTypes.rbegin();
        it != core_->unitTypes.rend(); ++it){
      typeNum_t index = it->getIndex();
      //valid for this player
      if (game_->validUnit(player_, index, militaryBuildingType))
         //created at the military building found above
         if (it->getOriginBuilding() == militaryBuildingType)
            //military unit
            if (!(it->isBuilder() || it->isGatherer())){
               //while affordable
               const Resources &cost = it->getCost();
               while (militarySupply_ >= cost){
                  //pay for and queue up the unit
                  militaryQueue_.push(index);
                  player.subtractResources(cost);
                  militarySupply_ -= cost;
                  debug("Player ", player_, "'s AI is training a ", it->getName());
               }
            }//if military unit        
   }//for unitTypes
}

//build anything in the buildQueue which can be built
void AI::buildPossible(){

   //military units
   while (!militaryQueue_.empty()){
      bool trained = game_->trainUnit(militaryQueue_.front(),
                                      *militaryBuilding_,
                                      player_);
      if (trained){
         militarySupply_ -= core_->unitTypes
                               [militaryQueue_.front()].getCost();
         militaryQueue_.pop();
      }else
         break;
   }
}