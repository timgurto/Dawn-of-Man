// (C) 2010 Tim Gurto

#include "ResourceNode.h"
#include "GameData.h"

ResourceNode::ResourceNode(typeNum_t typeIndex,
                           const Point &loc):
Entity(typeIndex, loc),
resources_(game_->resourceNodeTypes[typeIndex].maxResources_){}

const EntityType &ResourceNode::type() const{
   return game_->resourceNodeTypes[typeIndex_];
}

EntityTypeID ResourceNode::classID() const{
   return ENT_RESOURCE_NODE;
}

bool ResourceNode::targetable() const{
   return true;
}

bool ResourceNode::drawBlack() const{
   return
      1.0 * resources_.sum() /
      game_->resourceNodeTypes[typeIndex_].maxResources_.sum() <
      ENTITY_BLACK_HEALTH;
}

Resources ResourceNode::harvest(Resources bonus){
   const ResourceNodeType &thisType =
      game_->resourceNodeTypes[typeIndex_];
   Resources yield = Resources::empty();
   Resources maxYield = thisType.yield_ + bonus;
   if (resources_ >= maxYield)
      yield = maxYield;
   else{
      for (size_t i = 0; i != Resources::resourceCount_; ++i){
         yield.vals_[i] = min(maxYield.vals_[i],
                              resources_.vals_[i]);
      }
   }


   
   resources_ -= yield;
   if (resources_.isEmpty())
      kill();
   return yield;
}

int ResourceNode::getColor() const{
   return game_->resourceNodeTypes[typeIndex_].color_;
}