#include "collision_space.hpp"

#include <iostream>

static CollisionSpace* _singleton = NULL;

CollisionSpace* CollisionSpace::sharedCollisionSpace() {
	if(_singleton == NULL) {
		_singleton = new CollisionSpace();
	}
	return _singleton;
}

void CollisionSpace::add(ICollidable* collidable) {
	_collidables.push_back(collidable);
}

void CollisionSpace::checkForCollisions() {
	for(int i = 0; i < _collidables.size(); i++){
		ICollidable* obj1 = _collidables[i];
		if(obj1->isCollidable()) {
			for(int j = i+1; j < _collidables.size(); j++) {

				ICollidable* obj2 = _collidables[j];
				if(obj2->isCollidable()) {

					float z1 = obj1->getLocation().z;
					float z2 = obj2->getLocation().z;
					float t1 = z1+obj1->getCollidableHeight();
					float t2 = z2+obj2->getCollidableHeight();

					if(
							pow(obj1->getLocation().x-obj2->getLocation().x,2) +
							pow(obj1->getLocation().y-obj2->getLocation().y,2) <
							pow(obj1->getCollidableRadius()+obj2->getCollidableRadius(),2)
					){
					    if(
							(z2 >= z1 && z2 <= t1) ||
							(t2 >= z1 && t2 <= t1) ||
							(t2 >= t1 && z2 <= z1)
						) {
							obj1->collided(obj2);
							obj2->collided(obj1);
						}
					}
				}
			}
		}
	}
}
