// (C) 2010 Tim Gurto

#ifndef AI_H
#define AI_H

#include <list>
#include "queue"
#include "types.h"
#include "globals.h"
#include "Resources.h"
#include "Building.h"

struct GameData;
struct CoreData;

//list, for safe removal of arbitrary elements
typedef std::list<typeNum_t> wishlist_t;
typedef std::queue<typeNum_t> buildQueue_t;

//Handles an individual player's AI
class AI{

   //whose AI this is
   typeNum_t player_;

   //allocated stockpiles
   //all incoming resources are divided amongst these two stockpiles
   Resources
      expansionSupply_,
      militarySupply_;

   //stockpile caps
   //not a hard maximum, but any more is considered excess.
   Resources
      expansionCap_,
      militaryCap_;
   
   //expansion items the player wants to build
   wishlist_t
      unitWishlist_,
      buildingWishlist_,
      techWishlist_;

   //items which have had resources allocated, in order of preference
   buildQueue_t buildingsQueue_; //buildings
   buildQueue_t militaryQueue_; //military units
   buildQueue_t expansionQueue_; //other units

   //the buildings which will train units
   const Building *militaryBuilding_, *expansionBuilding_;

   //x:1 expansion:military
   static const double allocationRatio_;

   static GameData *game_;
   static const CoreData *core_;

public:

   AI();

   static void init(const CoreData *core, GameData *game);

   //initialize player ID
   void initPlayer(typeNum_t player);

   //updates stockpile caps based on wishlists
   void updateCaps();

   //allocates income into stockpiles
   void allocateIncome(const Resources &income);

   //check for and execute any newly available actions
   void update();

   //pay for any available expansion items
   void checkExpansion();

   //pay for any available military items
   void checkMilitary();

   //build stuff in the buildQueue
   void buildPossible();
};

#endif