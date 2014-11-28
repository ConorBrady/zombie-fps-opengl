#include "player.hpp"

Player::Player(glm::vec3 location) {

	_camera = new Camera(location,M_PI/2,0);
}


void Player::signal(ControlSignal cs, float value) {
	_camera->signal(cs,value);
}

void Player::update(unsigned int shader, float time) {
	_camera->update(shader,time);
}

glm::vec3 Player::getLocation() {
	return _camera->getLocation();
}
