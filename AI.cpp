// (C) 2010 Tim Gurto

#include <cassert>
#include <cstdlib>
#include <vector>
#include "util.h"
#include "misc.h"
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
#include "Unit.h"
#include "ResourceNode.h"

extern Debug debug;

GameData *AI::game_ = 0;
const CoreData *AI::core_ = 0;
typeNum_t AI::expansionUnitType_ = NO_TYPE;

const double AI::allocationRatio_ = 3; //x:1 expansion:military
const unsigned AI::expansionUnits_ = 4;

AI::AI():
militaryBuilding_(0),
expansionBuilding_(0),
player_(NO_TYPE),
expansionUnitsAdded_(false){}

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

   //assumption: one kind of expansion unit
   ITERATE(unitTypes_t::const_iterator, core_->unitTypes, it)
      if (it->isBuilder() || it->isGatherer()){
         expansionUnitType_ = it->getIndex();
         break;
      }
   assert(expansionUnitType_ != NO_TYPE);
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

   ITERATE(buildingTypes_t::const_iterator, core_->buildingTypes, it){
      const Resources &cost = it->getCost();
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
   //debug("Military: ", militarySupply_, "    Expansion: ", expansionSupply_);
}

void AI::update(){
   if (game_){
      checkExpansion();
      checkMilitary();
      checkBuildings();
      buildPossible();
      dispatchAllUnits();
   }
}

void AI::checkExpansion(){
   //assumption: one type of expansion unit building, one instance
   //TODO maintain vector, and allow multiple buliding types

   //add initial units to the wishlist, only once
   if (!expansionUnitsAdded_)
      for (int i = 0; i != expansionUnits_; ++i)
         unitWishlist_.push_back(expansionUnitType_);

   //find expansion building
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
      //debug("No expansion building found");
      return;
   }
   typeNum_t expBuildingType = expansionBuilding_->getTypeIndex();
   Player &player = game_->players[player_];

   //check any expansion units
   std::vector<wishlist_t::iterator> trash; //iterators to erase
   ITERATE(wishlist_t::iterator, unitWishlist_, it){
      typeNum_t index = *it;
      const Resources &cost = core_->unitTypes[index].getCost();
      if (!player.sufficientResources(cost))
         break;
      
      expansionSupply_ -= cost;
      expansionQueue_.push(index);

      trash.push_back(it);
      //debug("Player ", player_, "'s AI is training a ",
      //      core_->unitTypes[index].getName());
   }
   ITERATE(std::vector<wishlist_t::iterator>::iterator, trash, it)
      unitWishlist_.erase(*it);
}

void AI::checkMilitary(){
   //assumption: largest index is the best military unit
   //assumption: one type of military building, one instance of building
   //if these assumptions are wrong, the AI will be less effective.
   //TODO maintain vector of military buildings, possibly of multiple types, instead
   
   //find military building (only necessary once)
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
      //debug("No military building found");
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
                  //debug("Player ", player_, "'s AI is training a ",
                  //      it->getName());
               }
            }//if military unit        
   }//for unitTypes
}

void AI::checkBuildings(){
   const Player &player = game_->players[player_];
   for (typeNum_t i = 0; i != core_->buildingTypes.size(); ++i){
      const BuildingType &type = core_->buildingTypes[i];
      if (player.hasBuilding(i))
         continue;
      if (!game_->validBuilding(player_, i))
         continue;
      const Resources &cost = type.getCost();
      if (expansionSupply_ < cost)
         continue;
      
      expansionSupply_ -= cost;
      buildingsQueue_.push(i);
   }
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

   //expansion units
   while (!expansionQueue_.empty()){
      bool trained = game_->trainUnit(expansionQueue_.front(),
                                      *expansionBuilding_,
                                      player_);
      if (trained)
         expansionQueue_.pop();
      else
         break; //no room to place unit
   }

   //buildings
   while (!buildingsQueue_.empty()){
      typeNum_t index = buildingsQueue_.front();
      const BuildingType &type = core_->buildingTypes[index];
      Point loc(-1, -1);
      int tries = 100;
      bool built;
      do{
         loc = Point(pixels_t(rand() % game_->map.w),
                     pixels_t(rand() % game_->map.h));
         if (!inside(type.getBaseRect() + loc,
             dimRect(game_->map)))
            continue;
         if (!noCollision(*game_, type, loc))
            continue;
         built = game_->constructBuilding(index, loc, player_);
         if (!built)
            break;
      }while(--tries >= 0);
      if (built)
         buildingsQueue_.pop();
      else
         break;
   }
}

void AI::dispatchUnit(Unit *unit, const ResourceNode *ignore){

   //non-gatherer builders are dispatched separately
   if (unit->isBuilder())
      ITERATE(entities_t::const_iterator, game_->entities, it)
         if ((*it)->classID() == ENT_BUILDING){
            const Building &building = (const Building &)(**it);
            if (!building.isFinished()){
               unit->setTarget(*it);
               return;
            }
         }
   //fall-through to gathering if nothing to build

   //1. gatherers --> resource sites
   if (unit->isGatherer()){
      ResourceNode *target = unit->findNearbyResource(ignore);
      if (target)
         unit->setTarget(target);
      //else no nodes on map, and so stay idle
      return;
   }

   //2. military --> attack nearest enemy
   //TODO surround base first
   Entity *target = 0;
   double dist = -1;
   ITERATE(entities_t::const_iterator, game_->entities, it){
      EntityTypeID classID = (*it)->classID();
      if (classID == ENT_UNIT){
         const Unit &enemy = (const Unit &)(**it);
         if (enemy.getPlayer() == player_)
            continue;
      }else if (classID == ENT_BUILDING){
         const Building &enemy = (const Building &)(**it);
         if (enemy.getPlayer() == player_)
            continue;
      }else
         continue; //not a unit or building
      //enemy unit: find shortest
      double tempDist = distance(unit->getLoc(), (*it)->getLoc());
      if (dist == -1 || tempDist < dist){
         dist = tempDist;
         target = *it;
      }
   }
   if (target)
      unit->setTarget(target);
}

void AI::dispatchAllUnits(){
   ITERATE(entities_t::iterator, game_->entities, it)
      if ((*it)->classID() == ENT_UNIT){
         Unit *unit = (Unit *)(*it);
         if (unit->getPlayer() == player_ &&
             unit->isIdle())
            dispatchUnit(unit);
      }
}