#include "glm/glm.hpp"
#include "collision_space.hpp"

class Bullet : public ICollidable {
private:
	float _createdAt;
	glm::vec3 _initialWorldPos;
	float _yaw;
	float _pitch;
	float _lastTick;
	bool _hasCollided;
	int _index;

public:
	Bullet(float time, glm::vec3 startPos, float pitch, float yaw, int index);

	glm::vec3 getLocation();
	float getCollidableHeight();
	float getCollidableRadius();
	bool isCollidable();
	void collided(ICollidable* collided);
	int getCollisionPoisons();

	void update(float time);
	void draw(int shader);
};
