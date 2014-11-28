#include "follow_interface.hpp"

#include <vector>

class Zombie : public IFollower {

private:
	glm::vec3 _location;
	std::vector<IFollowable*> _followables;
	float _yaw;
	float _lastDrawTime;

	int* _series;
	int _seriesIndex;
	float _previousSine;
	int _mode;
	float _speedFactor;

	IFollowable* _nearestFollowable();



public:
	Zombie(glm::vec3 location);
	void draw(unsigned int shader, float time);

	void addFollowable(IFollowable* followable);
};
