#include "glm/glm.hpp"
#include "follow_interface.hpp"
#include <vector>

#ifndef COLLISION_SPACE
#define COLLISION_SPACE

enum CollisionProperty {
	// objects will exhibit various properties that effect the objects they
	// collide with, for example if a bullet hits a zombie, it should die.
	// Therefore the bullet should give a "KILL_ZOMBIE" property, so once the
	// bullet is passed to the zombie on collision it can deduce how it should
	// react without caring if it is a bullet, so any object then can easily
	// kill a zombie. This is applied to all situations.

	NONE 						= 0,
	KILL_ZOMBIE 				= 1 << 0, // kill zombies on impact
	KILL_HUMAN 					= 1 << 1, // kill humans on impact
	STOP_MOVING_BULLET			= 1 << 2, // stop a moving bullet
	ADD_BULLET					= 1 << 3, // gift a bullet on impact
	DESTROY_STATIONARY_BULLET 	= 1 << 4  // destroy a bullet if stationary
};

class ICollidable : public ITrackable {
	// an object than may be involved in a collision in the 3D space, note at
	// this point it is only defined to be a point based on it being Trackable
	// This interface should not be directly implemented as the collision space
	// will only accept Lines and Cylinders
public:

	// method to call when a collision happens, each objects collide method is
	// called with the other object being passed as a parameter
	virtual void collided(ICollidable* collided) = 0;

	// see "enum CollisionProperty"
	virtual int getCollisionProperties() = 0;

	// required as the object collide one after another, resulting in the state
	// of the first object changing before it can be "collided" the second,
	// therefore a copy must be made of each before the collision
	virtual ICollidable* clone() = 0;
};

class ICollidableCylinder : public ICollidable {
	// A collidable object with a cylindrical shape examples include zombies and
	// the player
public:
	virtual float getCollidableHeight() = 0;
	virtual float getCollidableRadius() = 0;
};

typedef struct {
	glm::vec3 start;
	glm::vec3 end;
} LineSegment;

class ICollidableLine : public ICollidable {
	// A line segment that can be checked for intersection with a 3D shape such
	// as a cyclinder, an example is the bullets the player shoots. Their
	// trajectories are broken into discrete line segments.
public:
	virtual LineSegment* getLineSegment() = 0;
};

class CollisionSpace {
	// singleton collision space object, manages the collidable objects in the
	// game and alerts them when they have collided and to what they collided
	// into
private:
	std::vector<ICollidableCylinder*> _collidableCylinders;
	std::vector<ICollidableLine*> _collidableLines;

public:
	static CollisionSpace* sharedCollisionSpace();
	void addCylinder(ICollidableCylinder* collidable);
	void addLine(ICollidableLine* line);
	void checkForCollisions();
	void clear();
};
#endif
