#include "glm/glm.hpp"
#include <vector>

enum Poison {
	POISON_NONE = 0,
	POISON_ZOMBIE = 1,
	POISON_HUMAN = 2
};

class ICollidable {

public:
	virtual glm::vec3 getLocation() = 0;
	virtual float getCollidableHeight() = 0;
	virtual float getCollidableRadius() = 0;
	virtual bool isCollidable() = 0;
	virtual void collided(ICollidable* collided) = 0;
	virtual int getCollisionPoisons() = 0;
};

class CollisionSpace {

private:
	std::vector<ICollidable*> _collidables;

public:
	static CollisionSpace* sharedCollisionSpace();
	void add(ICollidable* collidable);
	void checkForCollisions();
};
