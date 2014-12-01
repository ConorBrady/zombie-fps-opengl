#include "bullet.hpp"

#define BULLET_SPEED 300

Bullet::Bullet(float time, glm::vec3 startPos, float pitch, float yaw) {
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
