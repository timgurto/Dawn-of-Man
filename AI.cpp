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
#include "Player.h"

extern Debug debug;

GameData *AI::game_ = 0;
const CoreData *AI::core_ = 0;
const double AI::allocationRatio_ = 3; //x:1 expansion:military

AI::AI():
militaryBuilding_(0),
expansionBuilding_(0),
player_(NO_TYPE){}

void AI::init(const CoreData *core, GameData *game){
   core_ = core;
   game_ = game;
   
   //initialize military caps; only needs to be done once
   Resources militaryCap;
   ITERATE(unitTypes_t::const_iterator, core_->unitTypes, it)
      if (!(it->isBuilder() || it->isGatherer())){
         const Resources &cost = it->getCost();
         if (militaryCap < cost)
            militaryCap += (cost - militaryCap);
      }

   ITERATE(players_t::iterator, game_->players, it)
      it->ai_.militaryCap_ = militaryCap;
}

void AI::initPlayer(typeNum_t player){
   player_ = player;
}

void AI::updateCaps(){
   ITERATE(wishlist_t::const_iterator, unitWishlist_, it){
      const Resources &cost = core_->unitTypes[*it].getCost();
      if (expansionCap_ < cost)
         expansionCap_ += (cost - expansionCap_);
   }

   ITERATE(wishlist_t::const_iterator, buildingWishlist_, it){
      const Resources &cost = core_->buildingTypes[*it].getCost();
      if (expansionCap_ < cost)
         expansionCap_ += (cost - expansionCap_);
   }
   
   ITERATE(wishlist_t::const_iterator, techWishlist_, it){
      const Resources &cost = core_->techs[*it].getCost();
      if (expansionCap_ < cost)
         expansionCap_ += (cost - expansionCap_);
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
   //assumption: one type of expansion unit building, one instance
   //TODO maintain vector, and allow multiple buliding types

   //find expansion building
   //TODO set to 0 if the building is destroyed
   if (!expansionBuilding_)
      ITERATE(entities_t::const_iterator, game_->entities, it){
         if ((*it)->classID() == ENT_BUILDING){
            expansionBuilding_ = (const Building *)(*it);
            if (expansionBuilding_->getPlayer() == player_ &&
                expansionBuilding_->isExpansion() &&
                expansionBuilding_->isFinished())
               break;
            expansionBuilding_ = 0;
         }
      }
   if (!expansionBuilding_){
      debug("No expansion building found");
      return;
   }
   typeNum_t expBuildingType = expansionBuilding_->getTypeIndex();
   Player &player = game_->players[player_];

   //check any expansion units
   ITERATE(wishlist_t::iterator, unitWishlist_, it){
      typeNum_t index = *it;
      const Resources &cost = core_->unitTypes[index].getCost();
      if (!player.sufficientResources(cost))
         break;
      
      expansionSupply_ -= cost;
      expansionQueue_.push(index);
      unitWishlist_.erase(it);
      debug("Player ", player_, "'s AI is training a ",
            core_->unitTypes[index].getName());
   }
}

void AI::checkMilitary(){
   //assumption: largest index is the best military unit
   //assumption: one type of military building, one instance of building
   //if these assumptions are wrong, the AI will be less effective.
   //TODO maintain vector of military buildings, possibly of multiple types, instead
   
   //find military building (only necessary once)
   //TODO set to 0 if the building is destroyed
   if (!militaryBuilding_)
      ITERATE(entities_t::const_iterator, game_->entities, it){
         if ((*it)->classID() == ENT_BUILDING){
            militaryBuilding_ = (const Building *)(*it);
            if (militaryBuilding_->getPlayer() == player_ &&
                militaryBuilding_->isMilitary() &&
                militaryBuilding_->isFinished())
                     break;
            militaryBuilding_ = 0;
         }
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
                  militarySupply_ -= cost;
                  debug("Player ", player_, "'s AI is training a ",
                        it->getName());
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
      if (trained)
         militaryQueue_.pop();
      else
         break; //no room to place unit
   }
}