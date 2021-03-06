// (C) 2009, 2010 Tim Gurto

#ifndef PLAYER_H
#define PLAYER_H

#include "types.h"
#include "globals.h"
#include "Resources.h"
#include "TechBonuses.h"
#include "CoreData.h"
#include "AI.h"

struct GameData;
class UIBar;

//TODO consider bitfield
typedef std::vector<bool> checklist_t;

//Represents one player, be it the human or a
//computer opponent
class Player{
   friend class AI;

   //the player's ID number
   typeNum_t id_;

   //the color of this player's entities
   Uint32 color_;

   //the player's resource stockpiles
   Resources resources_;

   //So that it doesn't have to be recalculated constantly
   std::string resourcesString_;

   //bonuses from researched techs
   TechBonuses bonuses_;

   //which techs have been researched
   checklist_t techsResearched_;

   //which buildings have been constructed
   checklist_t buildingsBuilt_;

   //AI - bookkeeping and controlling
   AI ai_;

   //whether an AI update() has occurred yet in the game
   bool initialAIUpdate_;

   static GameData *game_;
   static const CoreData *core_;
   static UIBar *buildingsBar_;

public:
   Player(Uint32 color,
          const Resources &resources,
          const checklist_t &techsResearched,
          const checklist_t &buildingsBuilt,
          const typeNum_t playerID);

   //whether or not the player has any remaining buildings or units
   bool alive;

   //initialize static pointers
   static void init(const CoreData *core = 0, GameData *game = 0,
                    UIBar *buildingsBar = 0);

   //add or remove the player's resources
   void addResources(const Resources &r);
   void subtractResources(const Resources &r);

   //whether the player has at least the specified amount of resources
   bool sufficientResources(const Resources &r) const;

   //registers the player researching a tech
   void registerTech(typeNum_t index);

   //registers the player constructing a building
   void registerBuilding(typeNum_t index, bool recalc = false);

   //tells the AI that a unit has been trained
   void registerUnit(Unit *unit);

   //renders player nigh-invulnerable (toggle)
   void godMode();

   //processing done every game tick
   void tick();

   //clean up after an entity is destroyed
   void unregisterBuilding(const Building *building);

   //get
   Uint32 getColor() const;
   std::string getResources() const;
   const TechBonuses &getBonuses() const;
   bool hasTech(typeNum_t i) const;
   bool hasBuilding(typeNum_t i) const;
};

#endif