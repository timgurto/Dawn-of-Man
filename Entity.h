// (C) 2009-2010 Tim Gurto

#ifndef ENTITY_H
#define ENTITY_H

#include <list>
#include "SDL.h"
#include "globals.h"
#include "Point.h"

struct GameData;
class EntityType;

class Entity;
typedef std::list<Entity *> entities_t;

//An "instance" of an EntityType.  Basically, each entity
//is an on-screen, in-game element.
class Entity{
protected: //everything should be a derived class

   //progress points added per state update
   //progress points: proportional to completion time
   static const progress_t PROGRESS_PER_CALC;

   //type; index in vector of related EntityTypes
   typeNum_t typeIndex_;

   //direction of clipping for partial drawing
   Direction direction_;

   //any vertical movement that took place during the
   //last tick()
   VerticalMovement verticalMovement_;

   Point loc_; //location
   static GameData *game_; //static pointer to game
   static SDL_Surface *screen_; // " " to the screen

   //a list of Entity pointers.  Pointers are copied
   //here when marked for deletion, but deleting would
   //invalidate data or iterators.
   //Allows deletion at a later time.
   static entities_t trashCan_;

   //Draws the mask, then the colored sprite, creating
   //and indexing it if necessary
   void colorBlit(int color, SDL_Surface *screen,
                 SDL_Rect &srcLoc,
                 SDL_Rect &dstLoc) const;

   //creates particles indicating construction progress
   void addParticles(int count) const;

   //calculates
   SDL_Rect getSrcClip(pixels_t wPartial,
                       pixels_t hPartial,
                       int xMutiplier = 0) const;

public:

   Entity(typeNum_t typeIndex, const Point &loc);
   virtual ~Entity(){}

   //whether or not the entity is selected by the user
   bool selected;

   //less than; compares y co-ordinates
   bool operator<(const Entity &rhs) const;
   
   //type() rectangles + entity's location
   SDL_Rect getBaseRect() const;
   SDL_Rect getDrawRect() const;

   //Returns the EntityType object that this entity
   //is an "instance" of
   const virtual EntityType &type() const = 0;

   //Draws the entity, as well as a green mask behind
   //it if ENTITY_MASKS is true
   virtual void draw(SDL_Surface *screen = screen_) const;
   
   //Any changes that need to be made to the entity
   //by the game loop
   virtual void tick(double delta);

   //Whether the entity is within the screen frame
   bool onScreen() const;

   //Initializes the class' static pointers
   static void init(GameData *game, SDL_Surface *screen);

   //identitifes which entity subclass this is
   virtual EntityTypeID classID() const = 0;

   //whether these entities can collide with others
   //default: true
   virtual bool collides() const;

   //whether these entities can be selected
   //default: false
   virtual bool selectable() const;

   //toggles the entity's selection status
   inline void toggleSelect(){
      selected = !selected;
   }

   //calculates where to draw the selection indicator
   SDL_Rect getSelectionDest(SDL_Surface *selection) const;

   //whether the entity has moved vertically.
   //if so, the entities list may need to be re-sorted
   VerticalMovement getVerticalMovement() const;

   //whether the entity's location is valid
   bool isLocationOK() const;

   //kills the entity, removing it from the game
   void kill();

   //empties the trashCan_, deallocating memory
   static void emptyTrash();

   //get
   typeNum_t getTypeIndex() const;
   const Point &getLoc() const;

   //default: NO_TYPE
   virtual typeNum_t getPlayer() const;

   //default: FULL (1.0)
   virtual double getDrawPercent() const;

   //default: ENTITY_DEFAULT
   //see enum EntityColor
   virtual int getColor() const;
};

#endif