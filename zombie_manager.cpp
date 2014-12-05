#include "zombie_manager.hpp"
#include "score_manager.hpp"

#include <random>

static std::default_random_engine generator;
static std::normal_distribution<double> distribution(0.0,40.0);

ZombieManager::ZombieManager(Bounds* spawnBounds) {
	_spawnBounds = spawnBounds;
}

void ZombieManager::update(float time) {
	for(Zombie* zombie : _zombies) {
		zombie->update(time);
	}
	if(_zombies.size() < 8) {
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
		CollisionSpace::sharedCollisionSpace()->add(newZombie);
		for(IFollowable* f : _followables) {
			newZombie->addFollowable(f);
		}
		_zombies.push_back(newZombie);
	}
}

void ZombieManager::draw(unsigned int shader) {
	for(Zombie* zombie : _zombies) {
		zombie->draw(shader);
	}
}

void ZombieManager::addFollowable(IFollowable* followable) {
	for(Zombie* z : _zombies) {
		z->addFollowable(followable);
	}
	_followables.push_back(followable);
}
