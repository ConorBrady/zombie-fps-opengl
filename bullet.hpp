#include "glm/glm.hpp"

class Bullet {
private:
	float _createdAt;
	glm::vec3 _initialWorldPos;
	float _yaw;
	float _pitch;
public:
	Bullet(float time, glm::vec3 startPos, float pitch, float yaw);
	glm::vec3 getWorldPosAtTime(float time);
};
