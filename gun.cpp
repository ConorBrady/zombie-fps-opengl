#include "gun.hpp"
#include "bullet.hpp"

#include <random>
#include <iostream>

#define RECOIL_SPEED 30
#define RANDOM_SPEED 1
#define RANDOM_GUN_VARIATION 0.01
#define BULLET_COUNT 5
#define COCKING_TIME 0.5

#define GLM_FORCE_RADIANS

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "audible_space.hpp"

static std::default_random_engine generator;
static std::normal_distribution<double> distribution(0,RANDOM_GUN_VARIATION);

Gun::Gun(Camera* camera) {
	_camera = camera;
	_bullets = (Bullet**)calloc(BULLET_COUNT,sizeof(Bullet*));
	NotificationCenter::getNotificationCenter()->registerForNotifications(this);
}

void Gun::update(float time) {
	if(_fireGun) {
		_fireGun = false;
		if( time-_gunShotTime > COCKING_TIME && _remainingBullets > 0 ) {
			_remainingBullets--;
			_gunShotTime = time;
			int index = (_lastBulletIndex+1)%BULLET_COUNT;
			Bullet* bullet = new Bullet(time, getWorldPosition(), _camera->getPitch()+_altY, _camera->getYaw()+_altX, index);
			CollisionSpace::sharedCollisionSpace()->addLine(bullet);
			AudibleSpace::sharedAudibleSpace()->addAudible(bullet);
			_bullets[index] = bullet;
			_lastBulletIndex = index;
		}
	}

	for(int i = 0; i < BULLET_COUNT; i++) {
		if(_bullets[i]!=NULL){
			_bullets[i]->update(time);
		}
	}

	_lastTick = time;
}

int Gun::getRemainingBullets() {
	return _remainingBullets;
}

void Gun::addBullet(int count) {
	_remainingBullets += count;
}

void Gun::draw(uint shader) {

	for(int i = 0; i < BULLET_COUNT; i++) {
		if(_bullets[i]!=NULL ){
			_bullets[i]->draw(shader);
		} else {
			char buffer[50];
			sprintf(buffer,"light_properties[%d]",i);
			glUniformMatrix3fv(glGetUniformLocation(shader,buffer),1,GL_FALSE,glm::value_ptr(glm::mat3(0.0)));
		}
	}

	int M_loc = glGetUniformLocation (shader, "M");

	glUniformMatrix4fv (M_loc, 1, GL_FALSE, glm::value_ptr(_M(_lastTick)));
	int M1S = glGetUniformLocation(shader,"MESH_1_SELECT");
	int MIX = glGetUniformLocation(shader,"MIX");
	glUniform1f(MIX,0);
	glUniform1f(M1S,1);
	_mesh->draw(shader);

}

void Gun::reset() {
	for(int i = 0; i < BULLET_COUNT; i++) {
		_bullets[i] = nullptr;
	}
	_currentWave = 0;
	_remainingBullets = 0;
}

glm::mat4 Gun::_M(float time) {

	float recoil = 0;
	if((time-_gunShotTime)*RECOIL_SPEED<9.3) {
		recoil = sin((time-_gunShotTime)*RECOIL_SPEED-3)/((time-_gunShotTime)*RECOIL_SPEED-3);
	}

	return  glm::inverse(_camera->getViewMatrix())*
			glm::rotate(glm::mat4(1.0),-_altX,glm::vec3(0,1,0))*
			glm::rotate(glm::mat4(1.0),_altY+recoil/20,glm::vec3(1,0,0))*
			glm::translate(glm::mat4(1.0),glm::vec3(0,0,recoil))*
			glm::translate(glm::mat4(1.0),glm::vec3(0.2,-0.55,-0.2))*
			glm::rotate(glm::mat4(1.0),(float)M_PI,glm::vec3(0,1,0));
}

glm::vec3 Gun::getWorldPosition(){
	return glm::vec3(_M(0)*glm::vec4(-0.2,0.1,0,1));
}

void Gun::signal(ControlSignal cs, float value) {
	switch(cs) {
		case CONTROL_SIGNAL_ALT_X:
			_altX = value*1.25;
			break;
		case CONTROL_SIGNAL_ALT_Y:
			_altY = value*1.25;
			break;
		case CONTROL_SIGNAL_PRIMARY_ACTION:
			_fireGun = true;
			break;
		default:
			break;
	}
}


void Gun::respond(string notificationString, int value) {
	if(notificationString == "CREATED_WAVE") {
		_remainingBullets += (value-_currentWave)*20;
		_currentWave = value;
	}
}
