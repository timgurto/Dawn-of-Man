// (C) 2010 Tim Gurto

#ifndef RESOURCE_NODE_H
#define RESOURCE_NODE_H

#include <vector>
#include "SDL.h"
#include "types.h"
#include "globals.h"
#include "Entity.h"
#include "Resources.h"
#include "GameData.h"

class EntityType;

//Entities which begin full of resources, which are reduced
//as gatherer units harvest them.  When depleted, the nodes
//disappear
class ResourceNode: public Entity{

   //the amount of resources remaining in this node
   Resources resources_;

public:
   ResourceNode(typeNum_t typeIndex,
                const Point &loc);
   virtual const EntityType &ResourceNode::type() const;
   virtual EntityTypeID classID() const;
   virtual bool targetable() const;

   //remove some resources from the node, and return the
   //amount harvested
   Resources harvest();
};

#endif