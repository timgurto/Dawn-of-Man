// (C) 2010 Tim Gurto

#ifndef TECH_H
#define TECH_H

#include <string>
#include "globals.h"
#include "TechBonuses.h"
#include "Resources.h"
#include "Surface.h"

//An improvement each player can employ once,
//at a cost, to yield defined permanent bonuses
class Tech{
   friend struct CoreData;

   typeNum_t index_;
   std::string name_;
   std::string description_;
   Surface icon_;

   //What bonuses this tech grants
   TechBonuses bonuses_;

   //Which building can research this tech
   typeNum_t originBuilding_;

   //How much the tech costs
   Resources cost_;

   //prerequisites
   typeNum_t
      prereqTech1_,
      prereqTech2_;

public:
   Tech(typeNum_t index,
        const std::string &name,
        const std::string &description,
        const TechBonuses &bonuses,
        typeNum_t originBuilding,
        const Resources &cost,
        typeNum_t prereqTech1 = NO_TYPE,
        typeNum_t prereqTech2 = NO_TYPE);
   Tech (const Tech &rhs);

   //get
   const TechBonuses &getBonuses() const;
   typeNum_t getOriginBuilding()const;
   typeNum_t getIndex() const;
   const Surface &getIcon() const;
   const Resources &getCost() const;
   std::string getCostString() const;
   std::string getName() const;
   typeNum_t getPrereqTech1() const;
   typeNum_t getPrereqTech2() const;
   std::string getDescription() const;
};

#endif