#include "player.hpp"

Player::Player(glm::vec3 location, Bounds* worldBounds) {
	_initialPos = location;
	_camera = new Camera(location,M_PI/2,0,worldBounds);
	_gun = new Gun(_camera);
}	

bool Player::isDead() {
	return _isDead;
}

void Player::reset() {
	_camera->setPos(_initialPos);
	_gun->destroyBullets();
	_isDead = false;
}

void Player::update(unsigned int shader, float time) {
	_camera->update(shader,time);
	_gun->update(time);
}

void Player::draw(unsigned int shader) {
	_gun->draw(shader);
}

#pragma mark IControllable methods

void Player::signal(ControlSignal cs, float value) {
	_camera->signal(cs,value);
	_gun->signal(cs,value);
}

#pragma mark ICollidable and IFollowable methods

glm::vec3 Player::getLocation() {
	return _camera->getLocation();
}

#pragma mark ICollidable methods

bool Player::isCollidable() {
	return true;
}

void Player::collided(ICollidable* collided) {
	if(collided->getCollisionPoisons() & POISON_HUMAN) {
		_isDead = true;
	}
}

int Player::getCollisionPoisons() {
	return POISON_NONE;
}

#pragma mark ICollidableCylinder methods

float Player::getCollidableHeight() {
	return 5.0;
}

float Player::getCollidableRadius() {
	return 1.3;
}
