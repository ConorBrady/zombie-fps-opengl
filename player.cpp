#include "player.hpp"

Player::Player(glm::vec3 location) {

	_camera = new Camera(location,M_PI/2,0);
	_gun = new Mesh("resources/shotgun.dae");
}


void Player::signal(ControlSignal cs, float value) {
	_camera->signal(cs,value);
	switch(cs) {
		case CONTROL_SIGNAL_ALT_X:
			_gunYaw = value;
			break;
		case CONTROL_SIGNAL_ALT_Y:
			_gunPitch = value;
			break;
		default:
			break;
	}
}

void Player::update(unsigned int shader, float time) {
	_camera->update(shader,time);
}

void Player::draw(unsigned int shader, float time) {
	int M_loc = glGetUniformLocation (shader, "M");
	glm::mat4 gunModel = glm::inverse(_camera->getViewMatrix())*
						 glm::rotate(glm::mat4(1.0),-(float)M_PI/3*_gunYaw,glm::vec3(0,1,0))*
						 glm::rotate(glm::mat4(1.0),(float)M_PI/3*_gunPitch,glm::vec3(1,0,0))*
						 glm::translate(glm::mat4(1.0),glm::vec3(0.2,-0.55,-0.2))*
						 glm::rotate(glm::mat4(1.0),(float)M_PI,glm::vec3(0,1,0));
	glUniformMatrix4fv (M_loc, 1, GL_FALSE, glm::value_ptr(gunModel));
	int M1S = glGetUniformLocation(shader,"MESH_1_SELECT");
	int MIX = glGetUniformLocation(shader,"MIX");
	glUniform1f(MIX,0);
	glUniform1f(M1S,1);
	_gun->draw(shader,time);
}

glm::vec3 Player::getLocation() {
	return _camera->getLocation();
}
