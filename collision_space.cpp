#include "collision_space.hpp"

#include <iostream>

using namespace glm;
using namespace std;

static CollisionSpace* _singleton = NULL;

float shortestDistanceFromPointToLine(vec2 point, vec2 lineStart, vec2 lineEnd) {

	vec2 c = point - lineStart;	
	vec2 v = normalize(lineEnd - lineStart);
	v *= dot(v,c);

	return distance( lineStart+v, point);
}

CollisionSpace* CollisionSpace::sharedCollisionSpace() {
	if(_singleton == NULL) {
		_singleton = new CollisionSpace();
	}
	return _singleton;
}

void CollisionSpace::addCylinder(ICollidableCylinder* collidable) {
	_collidableCylinders.push_back(collidable);
}

void CollisionSpace::addLine(ICollidableLine* collidable) {
	_collidableLines.push_back(collidable);
}

void CollisionSpace::checkForCollisions() {

	for(int i = 0; i < _collidableCylinders.size(); i++) {

		ICollidableCylinder* obj1 = _collidableCylinders[i];

		for(int j = i+1; j < _collidableCylinders.size(); j++) {

			ICollidableCylinder* obj2 = _collidableCylinders[j];

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
					ICollidable* obj1Clone = obj1->clone();
					ICollidable* obj2Clone = obj2->clone();
					obj1->collided(obj2Clone);
					obj2->collided(obj1Clone);
				}
			}
		}

		for( ICollidableLine* obj2 : _collidableLines ) {

			LineSegment* seg = obj2->getLineSegment();

			float z1 = obj1->getLocation().z;
			float z2 = seg->start.z;
			float t1 = z1+obj1->getCollidableHeight();
			float t2 = seg->end.z;

			if( dot(vec2(seg->end)-vec2(seg->start),vec2(obj1->getLocation())-vec2(seg->start)) > 0 && // Is the start in front of it
			    dot(vec2(seg->start)-vec2(seg->end),vec2(obj1->getLocation())-vec2(seg->end)) > 0  && // Is the end behind it
			    shortestDistanceFromPointToLine(vec2(obj1->getLocation()),vec2(seg->start),vec2(seg->end)) < obj1->getCollidableRadius()
			){
				if(
					(z2 >= z1 && z2 <= t1) ||
					(t2 >= z1 && t2 <= t1) ||
					(t2 >= t1 && z2 <= z1)
				) {
					ICollidable* obj1Clone = obj1->clone();
					ICollidable* obj2Clone = obj2->clone();
					obj1->collided(obj2Clone);
					obj2->collided(obj1Clone);
				}
			}
		}
	}
}

void CollisionSpace::clear() {
	_collidableCylinders.clear();
	_collidableLines.clear();
}
