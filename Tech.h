// (C) 2010 Tim Gurto

#ifndef TECH_H
#define TECH_H

#include <string>
#include "TechBonuses.h"
#include "Resources.h"

//TODO comment
class Tech{
   typeNum_t index_;
   std::string name_;
   SDL_Surface *icon_;

   //What bonuses this tech grants
   TechBonuses bonuses_;

   //Which building can research this tech
   typeNum_t originBuilding_;

   //How much the tech costs
   Resources cost_;

   //Any prerequisite tech
   typeNum_t prereqTech_;

public:
   Tech(typeNum_t index,
        const std::string &name,
        const TechBonuses &bonuses,
        typeNum_t originBuilding,
        const Resources &cost,
        typeNum_t prereqTech);
   Tech (const Tech &rhs);
   ~Tech();
};

#endif