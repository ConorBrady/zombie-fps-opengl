#include "glm/glm.hpp"
#include "collision_space.hpp"

class Bullet : public ICollidableLine {
private:
	float _createdAt;
	glm::vec3 _initialWorldPos;
	glm::vec3 _lastPos;
	float _yaw;
	float _pitch;
	int _index;
	bool _isVisible = true;

	float _lastTick = -1;
	bool _isStationary = false;

	Bullet(Bullet* b);

public:
	Bullet(float time, glm::vec3 startPos, float pitch, float yaw, int index);
	void update(float time);
	void draw(int shader);
	
	#pragma mark ITrackable methods

	glm::vec3 getLocation();

	#pragma mark ICollidable methods

	void collided(ICollidable* collided);
	int getCollisionProperties();	
	ICollidable* clone();

	#pragma mark ICollidableLine methods

	LineSegment* getLineSegment();
};
