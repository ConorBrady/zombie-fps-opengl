#include "zombie.hpp"

#include "mesh.hpp"

#include <iostream>

#define YAW_SPEED 0.5
#define FOOT_SPEED 0.1

#define GLM_FORCE_RADIANS

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

static Mesh* _mesh = NULL;

IFollowable* Zombie::_nearestFollowable() {

	if(_followables.size()==0) {
		return NULL;
	}

	float minDistance = glm::distance(_followables[0]->getLocation(), _location);
	IFollowable* minFollowable = _followables[0];
	for(int i = 1; i < _followables.size(); i++) {
		float newDistance = glm::distance(_followables[i]->getLocation(), _location);
		if(newDistance < minDistance) {
			minFollowable = _followables[i];
		}
	}
	return minFollowable;
}

Zombie::Zombie(glm::vec3 location) {

	if(_mesh==NULL) {
		_mesh = new Mesh("resources/zombie.dae");
	}
	_location = location;
	_yaw = 0;

	_lastDrawTime = -1;
}


void Zombie::draw(unsigned int shader, float time) {

	if(_lastDrawTime > 0) {
		IFollowable* nearestFollowable = _nearestFollowable();
		float targetYaw = atan((nearestFollowable->getLocation().x-_location.x)/(nearestFollowable->getLocation().y-_location.y));
		if(nearestFollowable->getLocation().y>_location.y) {
			targetYaw = M_PI+targetYaw;
		}
		float dYaw = targetYaw - _yaw;
		dYaw = fmod(dYaw + M_PI,M_PI*2)-M_PI;
		if(dYaw>(time-_lastDrawTime)*YAW_SPEED || dYaw<-(time-_lastDrawTime)*YAW_SPEED) {
			_yaw += (time-_lastDrawTime)*YAW_SPEED*(dYaw>0?1:-1);
		} else {
			_location.x -= sin(_yaw)*FOOT_SPEED;
			_location.y -= cos(_yaw)*FOOT_SPEED;
		}
	}
	_lastDrawTime = time;
	int M_loc = glGetUniformLocation (shader, "M");
 	glm::mat4 M = glm::translate(glm::mat4(1.0),_location);
	M = glm::rotate(M, -_yaw, glm::vec3(0,0,1));
	glUniformMatrix4fv (M_loc, 1, GL_FALSE, glm::value_ptr(M));

	_mesh->draw(shader, time);
}

void Zombie::addFollowable(IFollowable* followable) {
	_followables.push_back(followable);
}
