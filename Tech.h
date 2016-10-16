// (C) 2010 Tim Gurto

#ifndef TECH_H
#define TECH_H

#include <string>
#include "globals.h"
#include "TechBonuses.h"
#include "Resources.h"

//TODO comment
class Tech{
   friend struct CoreData;

   typeNum_t index_;
   std::string name_;
   SDL_Surface *icon_;

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
        const TechBonuses &bonuses,
        typeNum_t originBuilding,
        const Resources &cost,
        typeNum_t prereqTech1 = NO_TYPE,
        typeNum_t prereqTech2 = NO_TYPE);
   Tech (const Tech &rhs);
   ~Tech();

   //get
   const TechBonuses &getBonuses() const;
   typeNum_t getOriginBuilding()const;
   typeNum_t getIndex() const;
   SDL_Surface *getIcon() const;
   const Resources &getCost() const;
   std::string getCostString() const;
   std::string getName() const;
   typeNum_t getPrereqTech1() const;
   typeNum_t getPrereqTech2() const;
};

#endif