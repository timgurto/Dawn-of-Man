// (C) 2010 Tim Gurto

#include "globals.h"
#include "GameData.h"

GameData::GameData():
mousePos(SCREEN_WIDTH/2, SCREEN_HEIGHT/2),
toBuild(NO_TYPE){
   for (typeNum_t i = 0; i != ENTITY_MAX; ++i)
      for (typeNum_t j = 0; j != MAX_TYPES; ++j)
         for (typeNum_t k = 0; k != MAX_ENTITY_TYPE; ++k)
            surfaceIndex[i][j][k] = 0;
}

GameData::~GameData(){

   //entities (delete)
   for (entities_t::iterator it = entities.begin();
        it != entities.end(); ++it){
      delete(*it);
   }

   //surface index (SDL_FreeSurface)
   for (typeNum_t i = 0; i != ENTITY_MAX; ++i)
      for (typeNum_t j = 0; j != MAX_TYPES; ++j)
         for (typeNum_t k = 0; k != MAX_ENTITY_TYPE; ++k)
            SDL_FreeSurface(surfaceIndex[i][j][k]);
}