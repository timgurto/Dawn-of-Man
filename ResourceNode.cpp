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

Resources ResourceNode::harvest(){
   const ResourceNodeType &thisType =
      game_->resourceNodeTypes[typeIndex_];
   Resources yield = Resources::empty();
   if (resources_ >= thisType.yield_)
      yield = thisType.yield_;
   else{
      for (size_t i = 0; i != Resources::resourceCount_; ++i){
         yield.vals_[i] = min(thisType.yield_.vals_[i],
                              resources_.vals_[i]);
      }
   }


   
   resources_ -= yield;
   if (resources_.isEmpty())
      kill();
   return yield;
}