#include "follow_interface.hpp"
#include "collision_space.hpp"

#include <vector>

class Zombie : public IFollower, public ICollidableCylinder {

private:
	glm::vec3 _location;
	std::vector<ITrackable*> _followables;
	float _lastTick = -1;
	float _yaw = 0;

	int* _series;
	int _seriesIndex = 0;
	float _previousSine = 0;
	int _mode = 0;
	float _mix = 0;
	float _speedFactor = 0;

	float _deadTime = -1;

	ITrackable* _nearestFollowable();

public:
	Zombie(glm::vec3 location);
	void update(float time);
	void draw(unsigned int shader);

	glm::vec3 getLocation();
	float getCollidableHeight();
	float getCollidableRadius();
	bool isCollidable();
	bool isAlive();
	void collided(ICollidable* collided);
	int getCollisionPoisons();

	void addFollowable(ITrackable* followable);
};
