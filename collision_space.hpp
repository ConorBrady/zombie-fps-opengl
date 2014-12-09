#include "glm/glm.hpp"
#include "follow_interface.hpp"
#include <vector>

#ifndef COLLISION_SPACE
#define COLLISION_SPACE

enum CollisionProperty {
	NONE 						= 0,
	KILL_ZOMBIE 				= 1 << 0,
	KILL_HUMAN 					= 1 << 1,
	STOP_MOVING_BULLET			= 1 << 2,
	ADD_BULLET					= 1 << 3,
	DESTROY_STATIONARY_BULLET 	= 1 << 4
};

typedef struct {
	glm::vec3 start;
	glm::vec3 end;
} LineSegment;

class ICollidable : public ITrackable {

public:
	virtual void collided(ICollidable* collided) = 0;
	virtual int getCollisionProperties() = 0;
	virtual ICollidable* clone() = 0;
};

class ICollidableCylinder : public ICollidable {

public:
	virtual float getCollidableHeight() = 0;
	virtual float getCollidableRadius() = 0;
};

class ICollidableLine : public ICollidable {
public:
	virtual LineSegment* getLineSegment() = 0;
};

class CollisionSpace {

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
