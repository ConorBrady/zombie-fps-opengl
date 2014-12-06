#include "glm/glm.hpp"
#include <vector>

#ifndef COLLISION_SPACE
#define COLLISION_SPACE
enum Poison {
	POISON_NONE 	= 0,
	POISON_ZOMBIE 	= 1 << 0,
	POISON_HUMAN 	= 1 << 1,
	POISON_BULLET 	= 1 << 2
};

class ICollidable {

public:
	virtual glm::vec3 getLocation() = 0;
	virtual bool isCollidable() = 0;
	virtual void collided(ICollidable* collided) = 0;
	virtual int getCollisionPoisons() = 0;
};

class ICollidableCylinder : public ICollidable {

public:
	virtual float getCollidableHeight() = 0;
	virtual float getCollidableRadius() = 0;

};

class CollisionSpace {

private:
	std::vector<ICollidableCylinder*> _collidableCylinders;

public:
	static CollisionSpace* sharedCollisionSpace();
	void addCylinder(ICollidableCylinder* collidable);
	void checkForCollisions();
	void clear();
};
#endif