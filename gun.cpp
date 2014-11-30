#include "gun.hpp"

#include <random>

#define RECOIL_SPEED 40
#define RANDOM_SPEED 0.01
#define RANDOM_GUN_VARIATION 1.5

static std::default_random_engine generator;
static std::normal_distribution<double> distribution(0,RANDOM_GUN_VARIATION);

Gun::Gun() {
	_mesh = new Mesh("resources/shotgun.dae");
	_fireGun = false;
	_gunShotTime = 0;
	_gunRandomPitch = 0;
	_gunRandomYaw = 0;
	_lastTick = 0;
}

void Gun::update(uint shader, float time) {
	if(_fireGun) {
		_gunShotTime = time;
		_fireGun = false;
	}
}

void Gun::draw(uint shader, float time, glm::mat4 viewMatrix) {

	int M_loc = glGetUniformLocation (shader, "M");

	_gunRandomPitch += (distribution(generator)-_gunRandomPitch)*RANDOM_SPEED*(time-_lastTick);
	_gunRandomYaw += (distribution(generator)-_gunRandomYaw)*RANDOM_SPEED*(time-_lastTick);
	float recoil = 0;
	if((time-_gunShotTime)*RECOIL_SPEED<15.56) {
		recoil = sin((time-_gunShotTime)*RECOIL_SPEED-3)/((time-_gunShotTime)*RECOIL_SPEED-3);
	}


	glm::mat4 gunModel = viewMatrix*
						glm::rotate(glm::mat4(1.0),-(float)M_PI/3*_gunYaw+_gunRandomYaw,glm::vec3(0,1,0))*
						glm::rotate(glm::mat4(1.0),(float)M_PI/3*_gunPitch+recoil/10+_gunRandomPitch,glm::vec3(1,0,0))*
						glm::translate(glm::mat4(1.0),glm::vec3(0,0,recoil))*
						glm::translate(glm::mat4(1.0),glm::vec3(0.2,-0.55,-0.2))*
						glm::rotate(glm::mat4(1.0),(float)M_PI,glm::vec3(0,1,0));

	glUniformMatrix4fv (M_loc, 1, GL_FALSE, glm::value_ptr(gunModel));
	int M1S = glGetUniformLocation(shader,"MESH_1_SELECT");
	int MIX = glGetUniformLocation(shader,"MIX");
	glUniform1f(MIX,0);
	glUniform1f(M1S,1);
	_mesh->draw(shader,time);
	_lastTick = time;
}

void Gun::signal(ControlSignal cs, float value) {
	switch(cs) {
		case CONTROL_SIGNAL_ALT_X:
		_gunYaw = value;
		break;
		case CONTROL_SIGNAL_ALT_Y:
		_gunPitch = value;
		break;
		case CONTROL_SIGNAL_PRIMARY_ACTION:
		_fireGun = true;
		break;
		default:
		break;
	}
}
