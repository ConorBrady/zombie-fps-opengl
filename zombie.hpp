#include "follow_interface.hpp"
#include "collision_space.hpp"

#include <vector>

class Zombie : public IFollower, public ICollidable {

private:
	glm::vec3 _location;
	std::vector<IFollowable*> _followables;
	float _lastTick;
	float _yaw;

	int* _series;
	int _seriesIndex;
	float _previousSine;
	int _mode;
	float _mix;
	float _speedFactor;

	float _deadTime;

	IFollowable* _nearestFollowable();



public:
	Zombie(glm::vec3 location);
	void update(float time);
	void draw(unsigned int shader);

	glm::vec3 getLocation();
	float getCollidableHeight();
	float getCollidableRadius();
	bool isCollidable();
	void collided(ICollidable* collided);
	int getCollisionPoisons();

	void addFollowable(IFollowable* followable);
};
