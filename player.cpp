#include "player.hpp"


Player::Player(glm::vec3 location) {
	_camera = new Camera(location,M_PI/2,0);
	_gun = new Gun(_camera);
}


void Player::signal(ControlSignal cs, float value) {
	_camera->signal(cs,value);
	_gun->signal(cs,value);
}

void Player::update(unsigned int shader, float time) {
	_camera->update(shader,time);
	_gun->update(time);
}

void Player::draw(unsigned int shader) {
	_gun->draw(shader);
}

glm::vec3 Player::getLocation() {
	return _camera->getLocation();
}
