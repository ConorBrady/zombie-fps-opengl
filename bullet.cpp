#include "bullet.hpp"

#include "mesh.hpp"

#define BULLET_SPEED 50

static Mesh* _mesh;

Bullet::Bullet(float time, glm::vec3 startPos, float pitch, float yaw) {
	if(_mesh==NULL) {
		_mesh = new Mesh("resources/shotgun.dae");
	}
	_createdAt = time;
	_initialWorldPos = startPos;
	_yaw = yaw;
	_pitch = pitch;
}

glm::vec3 Bullet::getWorldPosAtTime(float time) {
	float bullet_x, bullet_y, bullet_z;
	bullet_x = _initialWorldPos.x+(time-_createdAt)*sin(_pitch)*sin(_yaw)*BULLET_SPEED;
	bullet_y = _initialWorldPos.y+(time-_createdAt)*sin(_pitch)*cos(_yaw)*BULLET_SPEED;
	bullet_z = _initialWorldPos.z-(time-_createdAt)*cos(_pitch)*BULLET_SPEED;
	return glm::vec3(bullet_x,bullet_y,bullet_z);
}


void Bullet::draw(int shader, float time) {
	int M_loc = glGetUniformLocation (shader, "M");
	glUniformMatrix4fv (M_loc, 1, GL_FALSE, glm::value_ptr(glm::translate(glm::mat4(1.0),getWorldPosAtTime(time))*glm::rotate(glm::mat4(1.0),-_yaw,glm::vec3(0,0,1))*glm::rotate(glm::mat4(1.0),_pitch,glm::vec3(1,0,0))));
	int M1S = glGetUniformLocation(shader,"MESH_1_SELECT");
	int MIX = glGetUniformLocation(shader,"MIX");
	glUniform1f(MIX,0);
	glUniform1f(M1S,1);
	_mesh->draw(shader,time);
}
