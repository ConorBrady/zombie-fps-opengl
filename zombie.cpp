#include "zombie.hpp"

#include "mesh.hpp"
#include "score_manager.hpp"

#include <iostream>

#include <random>

#define YAW_SPEED 0.5
#define FOOT_SPEED 0.1

#define INCREASING_POSITIVE 1
#define DECREASING_POSITIVE 2
#define DECREASING_NEGATIVE 3
#define INCREASING_NEGATIVE 4

#define GLM_FORCE_RADIANS

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#define SERIES_SIZE 20
#define KEY_FRAMES 3

static Mesh* _mesh = NULL;

static std::default_random_engine generator;
static std::normal_distribution<double> distribution(1.0,0.4);

ITrackable* Zombie::_nearestFollowable() {

	if(_followables.size()==0) {
		return NULL;
	}

	float minDistance = glm::distance(_followables[0]->getLocation(), _location);
	ITrackable* minFollowable = _followables[0];
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

	_speedFactor = distribution(generator);

	_series = (int*)malloc(sizeof(int)*SERIES_SIZE);
	for(int i = 0; i < SERIES_SIZE; i++) {
		_series[i] = (i+rand())%KEY_FRAMES+1;
		while(i>0&&_series[i]==_series[i-1]) {
			_series[i] = (i+rand())%KEY_FRAMES+1;
		}
	}
}

glm::vec3 Zombie::getLocation() {
	return _location;
}

float Zombie::getCollidableHeight() {
	return 5;
}

float Zombie::getCollidableRadius() {
	return 1.3;
}

bool Zombie::isCollidable() {
	return isAlive();
}

bool Zombie::isAlive() {
	return _deadTime < 0;
}

void Zombie::collided(ICollidable* collided) {
	if(collided->getCollisionPoisons() & POISON_ZOMBIE) {
		_deadTime = _lastTick;
		ScoreManager::getSharedScoreManager()->registerZombieKill();
	}
}

int Zombie::getCollisionPoisons() {
	return POISON_HUMAN | POISON_BULLET;
}

void Zombie::update(float time) {
	if(_lastTick > 0 && _deadTime < 0) {
		ITrackable* nearestFollowable = _nearestFollowable();
		if(nearestFollowable != nullptr) {
			float targetYaw = atan((nearestFollowable->getLocation().x-_location.x)/(nearestFollowable->getLocation().y-_location.y));
			if(nearestFollowable->getLocation().y>_location.y) {
				targetYaw = M_PI+targetYaw;
			}
			float dYaw = targetYaw - _yaw;
			dYaw = fmod(dYaw + M_PI,M_PI*2)-M_PI;
			if(dYaw>(time-_lastTick)*YAW_SPEED || dYaw<-(time-_lastTick)*YAW_SPEED) {
				_yaw += (time-_lastTick)*YAW_SPEED*(dYaw>0?1:-1);
			} else {
				_location.x -= sin(_yaw)*FOOT_SPEED*_speedFactor;
				_location.y -= cos(_yaw)*FOOT_SPEED*_speedFactor;
			}
		}

		int nextIndex = (_seriesIndex+1)%SERIES_SIZE;
		float currentSine = sin(_lastTick*_speedFactor);
		int newMode;

		if(_previousSine < currentSine && currentSine > 0) {
			newMode = INCREASING_POSITIVE;
		} else if (_previousSine < currentSine && currentSine < 0) {
			newMode = INCREASING_NEGATIVE;
		} else if (_previousSine > currentSine && currentSine > 0) {
			newMode = DECREASING_POSITIVE;
		} else {
			newMode = DECREASING_NEGATIVE;
		}

		if(newMode != _mode) {
			_seriesIndex = nextIndex;
			nextIndex = (_seriesIndex+1)%SERIES_SIZE;
			_mode = newMode;
		}

		switch(_mode) {
			case INCREASING_POSITIVE:
			_mix = currentSine;
			break;
			case INCREASING_NEGATIVE:
			_mix = 1+currentSine;
			break;
			case DECREASING_POSITIVE:
			_mix = 1-currentSine;
			break;
			case DECREASING_NEGATIVE:
			_mix = fabs(currentSine);
			break;
		}
		_previousSine = currentSine;

	}
	_lastTick = time;
}

void Zombie::draw(unsigned int shader) {

	if(_deadTime < 0) {
		int M_loc = glGetUniformLocation (shader, "M");
	 	glm::mat4 M = glm::translate(glm::mat4(1.0),_location);
		M = glm::rotate(M, -_yaw, glm::vec3(0,0,1));
		glUniformMatrix4fv (M_loc, 1, GL_FALSE, glm::value_ptr(M));

		int M1S = glGetUniformLocation(shader,"MESH_1_SELECT");
		int M2S = glGetUniformLocation(shader,"MESH_2_SELECT");
		int MIX = glGetUniformLocation(shader,"MIX");

		glUniform1f(MIX,_mix);
		glUniform1f(M1S,_series[_seriesIndex]);
		glUniform1f(M2S,_series[(_seriesIndex+1)%SERIES_SIZE]);

		_mesh->draw(shader);
	}

}

void Zombie::addFollowable(ITrackable* followable) {
	_followables.push_back(followable);
}
