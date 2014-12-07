#include "player.hpp"

Player::Player(glm::vec3 location, Bounds* worldBounds) {
	_initialPos = location;
	_camera = new Camera(location,M_PI/2,0,worldBounds);
	_gun = new Gun(_camera);
}	

Player::Player(Player* p) {
	_camera = p->_camera;
	_gun = p->_gun;
	_isDead = p->_isDead;
	_initialPos = p->_initialPos;
}

bool Player::isDead() {
	return _isDead;
}

void Player::reset() {
	_camera->setPos(_initialPos);
	_gun->reset();
	_isDead = false;
}

void Player::update(unsigned int shader, float time) {
	_camera->update(shader,time);
	_gun->update(time);
}

void Player::draw(unsigned int shader) {
	_gun->draw(shader);
}

int Player::getRemainingBullets() {
	return _gun->getRemainingBullets();
}

#pragma mark IControllable methods

void Player::signal(ControlSignal cs, float value) {
	_camera->signal(cs,value);
	_gun->signal(cs,value);
}

#pragma mark ITrackable methods

glm::vec3 Player::getLocation() {
	return _camera->getLocation();
}

#pragma mark ICollidable methods

void Player::collided(ICollidable* collided) {
	if(collided->getCollisionProperties() & KILL_HUMAN) {
		_isDead = true;
	}
	if(collided->getCollisionProperties() & ADD_BULLET) {
		_gun->addBullet(1);
	}
}

int Player::getCollisionProperties() {
	return DESTROY_STATIONARY_BULLET;
}

ICollidable* Player::clone() {
	return new Player(this);
}

#pragma mark ICollidableCylinder methods

float Player::getCollidableHeight() {
	return 5.0;
}

float Player::getCollidableRadius() {
	return 1.5;
}
