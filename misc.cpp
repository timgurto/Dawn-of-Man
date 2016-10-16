// (C) 2009, 2010 Tim Gurto

#include <cassert>
#include <string>
#include <sstream>
#include <cstdlib>
#include <cmath>
#include <sstream>
#include <queue>
#include "util.h"
#include "globals.h"
#include "misc.h"
#include "Entity.h"
#include "Point.h"
#include "GameData.h"
#include "Debug.h"
#include "EntityType.h"
#include "Screen.h"

extern Debug debug;
extern int
   surfacesLoaded,
   soundsLoaded;


//Constructs a file path to load graphics for entities
std::string makePath(EntityTypeID type, typeNum_t imageNumber,
                     ImageModifier modifier){
   std::ostringstream path;
   switch (type){
   case ENT_BUILDING:
      path << BUILDINGS_IMAGE_PATH;
      break;
   case ENT_DECORATION:
      path << DECORATIONS_IMAGE_PATH;
      break;
   case ENT_UNIT:
      path << UNITS_IMAGE_PATH;
      break;
   case ENT_RESOURCE_NODE:
      path << RESOURCE_NODES_IMAGE_PATH;
      break;
   case ENT_TECH:
      path << TECHS_IMAGE_PATH;
      break;
   }

   switch (modifier){
   case IMG_ICON:
      path << 'p'; // p
      break;
   case IMG_MASK:
      path << 'm'; // m
      break;
   }

   assert (imageNumber < 1000);
   if (imageNumber < 100) // 0
      path << '0';
   if (imageNumber < 10) // 0
      path << '0';
   path << imageNumber; // 5
   
   path << IMAGE_SUFFIX; // .png
   return path.str();
}

//Dereferences Entity pointers and performs <
//(used in sorting the Entities list)
bool dereferenceLessThan(Entity *p1, Entity *p2){
   assert (p1);
   assert (p2);
   return *p1 < *p2;
}

//Checks for collisions against every entity in the game:
//a potential entity, of given type and location
//ignore: an entity to skip checking, i.e. the current unit
bool noCollision(const GameData &game, const EntityType &type,
                 const Point &loc, const Entity *ignore1,
                 const Entity *ignore2){
   SDL_Rect rect = loc + type.getBaseRect();

   //check that it's inside map
   if (!inside(rect, dimRect(game.map)))
      return false;

   //TODO * only check nearby entities
   //check against entities
   for (entities_t::const_iterator it = game.entities.begin();
        it != game.entities.end(); ++it){
      if (*it != ignore1 &&
          *it != ignore2 &&
          (*it)->collides() &&
          collision(rect, (*it)->getBaseRect()))
         return false;
   }
   return true;
}

//looks up the relevant color based on index input
Uint32 getEntityColor(const GameData &game, int color){
   //if a player color
   if (color < MAX_PLAYERS)
      return game.players[color].getColor();
   
   //otherwise
   switch(color){
   case CLR_BLACK:
      return BLACK_UINT;
   case CLR_RESOURCE_WOOD:
      return RESOURCE_WOOD_COLOR;
   case CLR_RESOURCE_FOOD:
      return RESOURCE_FOOD_COLOR;
   case CLR_RESOURCE_STONE:
      return RESOURCE_STONE_COLOR;
   case CLR_RESOURCE_METAL:
      return RESOURCE_METAL_COLOR;
   case CLR_DECORATION:
      return DECORATION_COLOR;
   case CLR_CORPSE:
      return CORPSE_COLOR;
   default:
      return ENTITY_DEFAULT_COLOR;

   }
}

//Centers the map on the provided co-ordinates.
//Map edges are taken into account automatically
void centerMap(GameData &game, const Point &center){
   const Point &res = Screen::getScreenRes();
   Point extendedDim = Point(game.map.w + SCROLL_MARGIN,
                             game.map.h + SCROLL_MARGIN);
   if (extendedDim.x <= res.x){
      game.map.x = (res.x - game.map.w) / 2;
      game.scrollLockX = true;
   }else{
      game.map.x = res.x / 2 - center.x;
      if (game.map.x > SCROLL_MARGIN)
         game.map.x = SCROLL_MARGIN;
      else if (game.map.x + extendedDim.x < res.x)
         game.map.x = res.x - extendedDim.x;
   }

   if (extendedDim.y <= res.y){
      game.map.y = (res.y - game.map.h) / 2;
      game.scrollLockY = true;
   }else{
      game.map.y = res.y / 2 - center.y;
      if (game.map.y > SCROLL_MARGIN)
         game.map.y = SCROLL_MARGIN;
      else if (game.map.y + extendedDim.y < res.y)
         game.map.y = res.y - extendedDim.y;
   }
}

//Determines whether the specified entity is
//able to move freely in a straight line to
//its target, without hitting anything
bool isPathClear(const Point &start,
                 const Point &end,
                 const GameData &game,
                 const Entity &entity){
   const EntityType &thisType = entity.type();


   //calculate angle
   double angle;
   if (end.x == start.x)
      angle =
         end.y < start.y ?
            1.5 * PI :
            0.5 * PI;
   else{
      double gradient =
         1.0 *
         (end.y - start.y) /
         (end.x - start.x);
      angle = atan(gradient);
      if (end.x < start.x){
         if (end.y > start.y)
            angle += PI;
         else
            angle -= PI;
      }
   }

   double
      xDelta = cos(angle) * PATH_CHECK_DISTANCE,
      yDelta = sin(angle) * PATH_CHECK_DISTANCE;

   double x = start.x, y = start.y;
   bool finished = false;
   while (!finished){
      //debug("Checking ", x, ",", y);
      if (!noCollision(game,
                       thisType,
                       Point(pixels_t(x),
                             pixels_t(y)),
                       &entity))
         return false;

      x += xDelta;
      y += yDelta;
      finished =
         (start.x < end.x ? x >= end.x : x <= end.x) &&
         (start.y < end.y ? y >= end.y : y <= end.y);
   }
   return true;
}

//Adds a leading space if x < 100 (fps display)
std::string format3(double x){
   std::ostringstream oss;
   if (x < 100)
      oss << ' ';
   oss << x;
   return oss.str();
}

//Adds a leading zero if x < 10 (game file names)
std::string format2(int x){
   std::ostringstream oss;
   if (x < 10)
      oss << '0';
   oss << x;
   debug (oss.str());
   return oss.str();
}

//Parse the next token from a data file
std::string parseToken(std::ifstream &data){
   std::ostringstream os;
   while (data){
      bool skip = false;
      char c;
      data >> c;
      //end if punctuation
      if (c == '{' ||
          c == '}' ||
          c == '=' ||
          c == ';'){
         os << c;
         break;
      }
      //skip rest of line if comment
      else if (c == '#'){
         do
            data.ignore(1);
         while (data.peek() != '\n'); //(c != '#');
         skip = true;
      }
      else if (c == '\"'){
         os << c;
         do{
            //stop >> from ignoring spaces
            if (data.peek() == ' '){
               c = ' ';
               data.ignore(1);
            }else
               data >> c;
            os << c;
         }while(c != '\"');
         data >> c;
         os << c;
         break;
      }
      if (!skip)
         os << c;
   }
   std::string ret = os.str();
   //debug(ret);
   return ret;
}

//if an index is invalid, set it to NO_TYPE
//so that nothing breaks
void checkTypeIndex(typeNum_t &i, size_t max){
   if (i != NO_TYPE && (i < 0 ||
                        i >= max))
      i = NO_TYPE;
}

//push a mouse-move event onto the queue, to refresh the
//calculations handled there
void pushMouseMove(){
   SDL_Event fakeMouseMove;
   fakeMouseMove.type = SDL_MOUSEMOTION;
   fakeMouseMove.motion.x = Screen::mousePos.x;
   fakeMouseMove.motion.y = Screen::mousePos.y;
   SDL_PushEvent(&fakeMouseMove);
}