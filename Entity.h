// (C) 2009, 2010 Tim Gurto

#ifndef ENTITY_H
#define ENTITY_H

#include <list>
#include "globals.h"
#include "Point.h"
#include "Surface.h"

struct GameData;
struct CoreData;
class EntityType;

class Entity;
typedef std::list<Entity *> entities_t;

extern Surface screenBuf;

//An "instance" of an EntityType.  Basically, each entity
//is an on-screen, in-game element.
class Entity{
   friend struct GameData;

protected: //everything should be a derived class

   //progress points added per state update
   //progress points: proportional to completion time
   static const progress_t PROGRESS_PER_CALC;

   //type; index in vector of related EntityTypes
   typeNum_t typeIndex_;

   //direction of clipping for partial drawing
   Direction direction_;

   //whether the entity has moved vertically.
   //if so, the entities list may need to be re-sorted
   VerticalMovement verticalMovement_;

   Point loc_; //location
   static GameData *game_; //static pointer to game
   static const CoreData *core_; //static pointer to core data
   
   //stark base for invisible entities
   static Surface *diagGreen_;

   //a list of Entity pointers.  Pointers are copied
   //here when marked for deletion, but deleting would
   //invalidate data or iterators.
   //Allows deletion at a later time.
   //It's best to empty it at the same place you use it,
   //so that you know what's going on.
   static entities_t trashCan_;

   //Draws the mask, then the colored sprite, creating
   //and indexing it if necessary
   //partial: whether the entity is only partially drawn
   //black: whether the entity should be darkened
   void colorBlit(int color, Surface &screen,
                 SDL_Rect &srcLoc,
                 SDL_Rect &dstLoc,
                 bool partial = false) const;

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
   virtual const EntityType &type() const = 0;

   //Draws the entity.  Should draw with colorBlit(), to
   //take advantage of surface indexing and entity masks
   virtual void draw(Surface &screen= screenBuf) const;
   
   //Any changes that need to be made to the entity
   //by the game loop
   virtual void tick(double delta);

   //Whether the entity is within the screen frame
   bool onScreen() const;

   //Initializes the class' static pointers
   static void init(const CoreData *core,
                    GameData *game = 0,
                    Surface *diagGreen = 0);

   //identitifes which entity subclass this is
   virtual EntityTypeID classID() const = 0;

   //whether the entity can collide with others
   //default: true
   virtual bool collides() const;

   //whether the entity can be selected
   //default: false
   virtual bool selectable() const;

   //whether the entity can be targeted
   //default: false
   virtual bool targetable() const;

   //toggles the entity's selection status
   inline void toggleSelect(){
      selected = !selected;
   }

   //calculates where to draw the selection indicator
   SDL_Rect getSelectionDest(Surface &selection) const;

   //whether the entity's location is valid
   bool isLocationOK() const;

   //kills the entity, removing it from the game
   void kill();

   //empties the trashCan_, deallocating memory
   static void emptyTrash();

   //how the entity determines whether or not it should be
   //blackened when drawn
   //default: false
   virtual bool drawBlack() const;

   //the entity's context help
   virtual std::string getHelp() const;

   //get
   typeNum_t getTypeIndex() const;
   const Point &getLoc() const;
   VerticalMovement getVerticalMovement() const;

   //default: NO_TYPE
   virtual typeNum_t getPlayer() const;

   //default: FULL (1.0)
   virtual double getDrawPercent() const;

   //default: ENTITY_DEFAULT
   //see enum EntityColor
   virtual int getColor() const;
};

#endif