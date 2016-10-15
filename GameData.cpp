// (C) 2010 Tim Gurto

#include "types.h"
#include "GameData.h"

GameData::GameData(int mapSizeX, int mapSizeY):
rightMouse(),
mapX(mapSizeX),
mapY(mapSizeY),
mousePos(SCREEN_WIDTH/2, SCREEN_HEIGHT/2),
toBuild(NO_TYPE),
buildingSelected(0){
   for (typeNum_t i = 0; i != ENTITY_MAX; ++i)
      for (typeNum_t j = 0; j != MAX_TYPES; ++j)
         for (typeNum_t k = 0; k != MAX_ENTITY_TYPE; ++k)
            surfaceIndex[i][j][k] = 0;
   
   //map
   Uint16 w = mapX * MAP_TILE_SIZE;
   Uint16 h = mapY * MAP_TILE_SIZE;
   //center map
   map = makeRect((SCREEN_WIDTH - w) / 2,
                  (SCREEN_HEIGHT - h) / 2,
                  w,
                  h);
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
            freeSurface(surfaceIndex[i][j][k]);
}