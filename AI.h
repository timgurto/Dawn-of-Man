// (C) 2010 Tim Gurto

#ifndef AI_H
#define AI_H

#include "types.h"
#include "globals.h"
#include "Resources.h"

struct GameData;
struct CoreData;

//EntityType, and type index
typedef std::pair<EntityTypeID, typeNum_t> AnyEntityType;
typedef std::vector<AnyEntityType> wishlist_t;
typedef std::queue<AnyEntityType> buildQueue_t;

//Handles an individual player's AI
class AI{

   //allocated stockpiles
   //all incoming resources are divided amongst these two stockpiles
   Resources
      expansionSupply_,
      militarySupply_;

   //stockpile caps
   //not a hard maximum, but any more is considered excess.
   static Resources
      expansionCap_,
      militaryCap_;
   
   //items the player wants to build
   wishlist_t wishlist_;

   //items which have had resources allocated, in order of preference
   buildQueue_t buildQueue_;

   static GameData *game_;
   static const CoreData *core_;

public:

   //updates stockpile caps based on wishlists
   void updateCaps();

   //allocates income into stockpiles
   void allocateIncome(const Resources income);
};

#endif