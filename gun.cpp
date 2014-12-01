#include "gun.hpp"

#include <random>
#include <iostream>

#define RECOIL_SPEED 30
#define RANDOM_SPEED 1
#define RANDOM_GUN_VARIATION 0.01
#define BULLET_COUNT 5

static std::default_random_engine generator;
static std::normal_distribution<double> distribution(0,RANDOM_GUN_VARIATION);

Gun::Gun(Camera* camera) {
	_camera = camera;
	_mesh = new Mesh("resources/shotgun.dae");
	_fireGun = false;
	_bullets = (Bullet**)calloc(BULLET_COUNT,sizeof(Bullet*));
	_lastBulletIndex = 0;
	_lastTick = 0;

}

void Gun::update(uint shader, float time) {
	if(_fireGun) {
		_fireGun = false;
		_gunShotTime = time;

		_bullets[(_lastBulletIndex+1)%BULLET_COUNT] = new Bullet(time, getWorldPosition(), _camera->getPitch()+_altY, _camera->getYaw()+_altX);
		_lastBulletIndex = (_lastBulletIndex+1)%BULLET_COUNT;
	}
	_gunRanPitchAcceleration = distribution(generator)-_gunRanPitch/3;
	_gunRanYawAcceleration = distribution(generator)-_gunRanYaw/3;

	_gunRanPitch += _gunRanPitchSpeed*(time-_lastTick)*RANDOM_SPEED + _gunRanPitchAcceleration*pow(time-_lastTick,2)*RANDOM_SPEED/2;
	_gunRanYaw += _gunRanYawSpeed*(time-_lastTick)*RANDOM_SPEED + _gunRanYawAcceleration*pow(time-_lastTick,2)*RANDOM_SPEED/2;

	_gunRanPitchSpeed += _gunRanPitchAcceleration*(time-_lastTick)*RANDOM_SPEED;
	_gunRanYawSpeed += _gunRanYawAcceleration*(time-_lastTick)*RANDOM_SPEED;

	if( _gunRanPitch > M_PI/3 ) {
		_gunRanPitch = 0;
	}

	if( _gunRanPitch < -M_PI/3 ) {
		_gunRanPitch = 0;
	}

	if( _gunRanYaw > M_PI/3 ) {
		_gunRanYaw = 0;
	}

	if( _gunRanYaw < -M_PI/3 ) {
		_gunRanYaw = 0;
	}

	std::vector<glm::mat3> lights {  glm::mat3(  1.0,  0.8, 0.8, 	// Ls
												1.0,  0.0, 0.0, 	// Ld
												0.02, 0.04, 0.0 ),
									glm::mat3(  1.0,  0.8, 0.8, 	// Ls
												0.0,  0.6, 0.0, 	// Ld
												0.02, 0.04, 0.0 ),
									glm::mat3(  1.0,  0.8, 0.8, 	// Ls
												0.0,  0.0, 0.6, 	// Ld
												0.02, 0.04, 0.0 ),
									glm::mat3(  1.0,  0.8, 0.8, 	// Ls
												1.0,  1.0, 0.0, 	// Ld
												0.02, 0.04, 0.0 ),
									glm::mat3(  1.0,  0.8, 0.8, 	// Ls
												0.0,  1.0, 1.0, 	// Ld
												0.02, 0.04, 0.0 ) }; // La

	for(int i = 0; i < BULLET_COUNT; i++) {
		if(_bullets[i]!=NULL){
			char buffer [50];
			sprintf(buffer,"light_position_world[%d]",i);
			glUniform3fv(glGetUniformLocation(shader,buffer),1,glm::value_ptr(_bullets[i]->getWorldPosAtTime(time)));
			sprintf(buffer,"light_properties[%d]",i);
			glUniformMatrix3fv(glGetUniformLocation(shader,buffer),1,GL_FALSE,glm::value_ptr(lights[i]));
		}
	}

	_lastTick = time;
}

void Gun::draw(uint shader, float time) {


	int M_loc = glGetUniformLocation (shader, "M");

	glUniformMatrix4fv (M_loc, 1, GL_FALSE, glm::value_ptr(_M(time)));
	int M1S = glGetUniformLocation(shader,"MESH_1_SELECT");
	int MIX = glGetUniformLocation(shader,"MIX");
	glUniform1f(MIX,0);
	glUniform1f(M1S,1);
	_mesh->draw(shader,time);

}

glm::mat4 Gun::_M(float time) {

	float recoil = 0;
	if((time-_gunShotTime)*RECOIL_SPEED<9.3) {
		recoil = sin((time-_gunShotTime)*RECOIL_SPEED-3)/((time-_gunShotTime)*RECOIL_SPEED-3);
	}

	return  glm::inverse(_camera->getViewMatrix())*
			glm::rotate(glm::mat4(1.0),-_altX+_gunRanYaw*2,glm::vec3(0,1,0))*
			glm::rotate(glm::mat4(1.0),_altY+recoil/20+_gunRanPitch,glm::vec3(1,0,0))*
			glm::translate(glm::mat4(1.0),glm::vec3(0,0,recoil))*
			glm::translate(glm::mat4(1.0),glm::vec3(0.2,-0.55,-0.2))*
			glm::rotate(glm::mat4(1.0),(float)M_PI,glm::vec3(0,1,0));
}

glm::vec3 Gun::getWorldPosition(){
	return glm::vec3(_camera->getLocation().x,_camera->getLocation().y,_camera->getLocation().z+5);
}

void Gun::signal(ControlSignal cs, float value) {
	switch(cs) {
		case CONTROL_SIGNAL_ALT_X:
			_altX = value;
			break;
		case CONTROL_SIGNAL_ALT_Y:
			_altY = value;
			break;
		case CONTROL_SIGNAL_PRIMARY_ACTION:
			_fireGun = true;
			break;
		default:
			break;
	}
}
