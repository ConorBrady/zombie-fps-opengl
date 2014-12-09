#include "zombie.hpp"

#include "mesh.hpp"
#include "notification_center.hpp"

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

Zombie::Zombie(Zombie* z) {

	_location = z->_location;
	for(ITrackable* f : z->_followables) {
		_followables.push_back(f);
	}
	_lastTick = z->_lastTick;
	_yaw = z->_yaw;

	_series = (int*)malloc(sizeof(int)*SERIES_SIZE);
	for(int i = 0; i < SERIES_SIZE; i++) {
		_series[i] = z->_series[i];
	}
	_seriesIndex = z->_seriesIndex;
	_previousSine = z->_previousSine;
	_mode = z->_mode;
	_mix = z->_mix;
	_speedFactor = z->_speedFactor;
	_deadTime = z->_deadTime;
}

Zombie::~Zombie() {
	free(_series);
}

glm::vec3 Zombie::getLocation() {
	return _location;
}

float Zombie::getCollidableHeight() {
	return 5;
}

float Zombie::getCollidableRadius() {
	return 1.2;
}

bool Zombie::isAlive() {
	return _deadTime < 0;
}

void Zombie::collided(ICollidable* collided) {
	if(collided->getCollisionProperties() & KILL_ZOMBIE) {
		if(isAlive()) {
			_deadTime = _lastTick;
			NotificationCenter::getNotificationCenter()->notify("ZOMBIE_KILL",1);
		}
	}
}

int Zombie::getCollisionProperties() {
	if(isAlive()) {
		return KILL_HUMAN | STOP_MOVING_BULLET;
	} else {
		return NONE;
	}
}

ICollidable* Zombie::clone() {
	return new Zombie(this);
}

void Zombie::update(float time) {
	if(_lastTick > 0 && _deadTime < 0) {
		ITrackable* nearestFollowable = _nearestFollowable();
		if(nearestFollowable != nullptr) {
			float targetYaw = atan((nearestFollowable->getLocation().x-_location.x)/(nearestFollowable->getLocation().y-_location.y));
			if(nearestFollowable->getLocation().y >_location.y) {
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


	int M_loc = glGetUniformLocation (shader, "M");
 	glm::mat4 M = glm::translate(glm::mat4(1.0),_location);
	if(!isAlive()) {
		M = glm::translate(M,glm::vec3(-sin(sin(_yaw)*(_lastTick-_deadTime))*10, -sin(cos(_yaw)*(_lastTick-_deadTime))*10,0));
	}
	M = glm::rotate(M, -_yaw, glm::vec3(0,0,1));
	if(!isAlive()) {
		M = glm::rotate(M,_lastTick-_deadTime,glm::vec3(0,0,1));

	}
	glUniformMatrix4fv (M_loc, 1, GL_FALSE, glm::value_ptr(M));

	int M1S = glGetUniformLocation(shader,"MESH_1_SELECT");
	int M2S = glGetUniformLocation(shader,"MESH_2_SELECT");
	int MIX = glGetUniformLocation(shader,"MIX");
	int explode = glGetUniformLocation(shader,"EXPLODE");

	glUniform1f(MIX,_mix);
	glUniform1f(M1S,_series[_seriesIndex]);
	glUniform1f(M2S,_series[(_seriesIndex+1)%SERIES_SIZE]);
	if(_deadTime>0) {
		glUniform1f(explode,(_lastTick-_deadTime)/3);
	}

	_mesh->draw(shader);
	glUniform1f(explode,0);

}

void Zombie::addFollowable(ITrackable* followable) {
	_followables.push_back(followable);
}

const char* Zombie::ambientSound() {
	return "resources/sound/zombie_ambient.wav";
}

bool Zombie::shouldPlayAmbientSound() {
	return isAlive() && rand()%50==0;
}

bool Zombie::shouldContinueAmbientSound() {
	return isAlive();
}

const char* Zombie::spawnSound() {
	return "resources/sound/zombie_spawn.wav";
}

bool Zombie::shouldContinueSpawnSound() {
	return isAlive();
}

const char* Zombie::destroySound() {
	return "resources/sound/zombie_destroy.wav";
}

bool Zombie::shouldPlayDestroySound() {
	return !isAlive();
}

bool Zombie::shouldContinueDestroySound() {
	return true;
}
