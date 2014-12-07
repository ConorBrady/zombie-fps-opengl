#include "bullet.hpp"

#include "mesh.hpp"

#define BULLET_SPEED 50

#include <iostream>
using namespace std;

#define GLM_FORCE_RADIANS

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

static Mesh* _mesh;

static std::vector<glm::mat3> lights {  glm::mat3(  1.0,  0.4, 0.4,	// Ls
													1.0,  0.4, 0.4, 	// Ld
													0.02, 0.04, 0.0 ),
										glm::mat3(  0.8,  0.8, 0.2, 	// Ls
													0.8,  0.8, 0.2, 	// Ld
													0.02, 0.04, 0.0 ),
										glm::mat3(  0.4,  1.0, 0.4,  	// Ls
													0.4,  1.0, 0.4, 	// Ld
													0.02, 0.04, 0.0 ),
										glm::mat3(  0.2,  0.8, 0.8, 	// Ls
													0.2,  0.8, 0.8, 	// Ld
													0.02, 0.04, 0.0 ),
										glm::mat3(  0.4,  0.4, 1.0, 	// Ls
													0.4,  0.4, 1.0, 	// Ld
													0.02, 0.04, 0.0 ) }; // La


Bullet::Bullet(float time, glm::vec3 startPos, float pitch, float yaw, int index) {
	if(_mesh==NULL) {
		_mesh = new Mesh("resources/bullet.dae");
	}
	_createdAt = time;
	_lastTick = time;
	_initialWorldPos = startPos;
	_lastPos = _initialWorldPos;
	_yaw = yaw;
	_pitch = pitch;
	_index = index;
}

Bullet::Bullet(Bullet* b) {
	_createdAt = b->_createdAt;
	_initialWorldPos = b->_initialWorldPos;
	_yaw = b->_yaw;
	_pitch = b->_pitch;
	_index = b->_index;
	_isVisible = b->_isVisible;
	_lastTick = b->_lastTick;
	_isStationary = b->_isStationary;
}

glm::vec3 Bullet::getLocation() {
	float bullet_x, bullet_y, bullet_z;
	bullet_x = _initialWorldPos.x+(_lastTick-_createdAt)*sin(_pitch)*sin(_yaw)*BULLET_SPEED;
	bullet_y = _initialWorldPos.y+(_lastTick-_createdAt)*sin(_pitch)*cos(_yaw)*BULLET_SPEED;
	bullet_z = _initialWorldPos.z-(_lastTick-_createdAt)*cos(_pitch)*BULLET_SPEED;
	return glm::vec3(bullet_x,bullet_y,bullet_z);
}

LineSegment* Bullet::getLineSegment() {
	LineSegment* s = (LineSegment*)malloc(sizeof(LineSegment));
	s->start = _lastPos;
	s->end = getLocation();
	return s;
}

void Bullet::collided(ICollidable* collided) {
	if(_isStationary) {
		if(collided->getCollisionProperties() & DESTROY_STATIONARY_BULLET) {
			_isVisible = false;
		}
	} else {
		if(collided->getCollisionProperties() & STOP_MOVING_BULLET) {
			_isStationary = true;
		}
	}
}

int Bullet::getCollisionProperties() {

	if(_isStationary) {
		if(_isVisible) {
			return ADD_BULLET;
		} else {
			return NONE;
		}
	} else {
		return KILL_ZOMBIE;	
	}
}

ICollidable* Bullet::clone() {
	return new Bullet(this);
}

void Bullet::update(float time) {
	if(!_isStationary) {
		_lastPos = getLocation();
		_lastTick = time;
	}
}

void Bullet::draw(int shader) {
	if(_isVisible) {
		char buffer [50];
		sprintf(buffer,"light_position_world[%d]",_index);
		glUniform3fv(glGetUniformLocation(shader,buffer),1,glm::value_ptr(getLocation()));
		sprintf(buffer,"light_properties[%d]",_index);
		glUniformMatrix3fv(glGetUniformLocation(shader,buffer),1,GL_FALSE,glm::value_ptr(lights[_index]));

		int M_loc = glGetUniformLocation (shader, "M");
		glUniformMatrix4fv (M_loc, 1, GL_FALSE, glm::value_ptr(	glm::translate(glm::mat4(1.0),getLocation())*
															  	glm::rotate(glm::mat4(1.0),-_yaw,glm::vec3(0,0,1))*
																glm::rotate(glm::mat4(1.0),(float)(_pitch+M_PI/2),glm::vec3(1,0,0))));
		int M1S = glGetUniformLocation(shader,"MESH_1_SELECT");
		int MIX = glGetUniformLocation(shader,"MIX");
		glUniform1f(MIX,0);
		glUniform1f(M1S,1);
		_mesh->draw(shader);
	} else {
		char buffer [50];
		sprintf(buffer,"light_properties[%d]",_index);
		glUniformMatrix3fv(glGetUniformLocation(shader,buffer),1,GL_FALSE,glm::value_ptr(glm::mat3(0)));
	}
	
}
