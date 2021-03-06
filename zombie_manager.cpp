#include "zombie_manager.hpp"
#include "score_manager.hpp"
#include "notification_center.hpp"
#include "audible_space.hpp"

#include <iostream>

#define WAVE_MULTIPLIER 8

using namespace std;

#include <random>

static std::default_random_engine generator;
static std::normal_distribution<double> distribution(0.0,40.0);

ZombieManager::ZombieManager(Bounds* spawnBounds, Window* window) {
	_window = window;
	_spawnBounds = spawnBounds;
	_waveNumber = 0;
}

void ZombieManager::reset() {
	_waveNumber = 0;
	_zombies.clear();
	_followables.clear();
}

void ZombieManager::update(float time) {
	for(Zombie* zombie : _zombies) {
		zombie->update(time);
	}
	if(_zombies.empty()) {

		while(_zombies.size() < _waveNumber*WAVE_MULTIPLIER) {
			double randx = distribution(generator);
			double randy = distribution(generator);
			if(fabs(randx) < fabs(randy)) {
				if(randy<0) {
					randy = _spawnBounds->y1;
				} else {
					randy = _spawnBounds->y2;
				}
			} else {
				if(randx<0) {
					randx = _spawnBounds->x1;
				} else {
					randx = _spawnBounds->x2;
				}
			}
			Zombie* newZombie = new Zombie(glm::vec3(randx,randy,0));
			CollisionSpace::sharedCollisionSpace()->addCylinder(newZombie);
			AudibleSpace::sharedAudibleSpace()->addAudible(newZombie);
			for(ITrackable* f : _followables) {
				newZombie->addFollowable(f);
			}
			_zombies.push_back(newZombie);

			NotificationCenter::getNotificationCenter()->notify("CREATED_WAVE",_waveNumber);
		}
	}
	if(!_anyAlive()) {
		_zombies.clear();
		_waveNumber++;
		char* temp = (char*)malloc(sizeof(char)*10);
		sprintf (temp, "Wave %d", _waveNumber);
		_window->queueToast(temp);
	}
}

bool ZombieManager::_anyAlive() {
	for(Zombie* z : _zombies) {
		if(z->isAlive()) {
			return true;
		}
	}
	return false;
}

void ZombieManager::draw(unsigned int shader) {
	for(Zombie* zombie : _zombies) {
		zombie->draw(shader);
	}
}

void ZombieManager::addFollowable(ITrackable* followable) {
	for(Zombie* z : _zombies) {
		z->addFollowable(followable);
	}
	_followables.push_back(followable);
}
