#include "follow_interface.hpp"
#include "collision_space.hpp"
#include "audible_space.hpp"

#include <vector>

class Zombie : public IFollower, public ICollidableCylinder, public IAudible {
	// the zombie
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

	Zombie(Zombie* zombie);

public:
	Zombie(glm::vec3 location);
	~Zombie();

	void update(float time);
	void draw(unsigned int shader);

	bool isAlive();

	#pragma mark ITrackable methods

	glm::vec3 getLocation();

	#pragma mark ICollidable methods

	void collided(ICollidable* collided);
	int getCollisionProperties();
	ICollidable* clone();

	#pragma mark ICollidableCylinder methods

	float getCollidableHeight();
	float getCollidableRadius();

	#pragma mark IFollower methods

	void addFollowable(ITrackable* followable);

	#pragma mark IAudible methods

	const char* ambientSound();
	float getAmbientSoundVolume() { return 1; }
	bool shouldPlayAmbientSound();
	bool shouldContinueAmbientSound();

	const char* spawnSound();
	bool shouldContinueSpawnSound();

	const char* destroySound();
	bool shouldPlayDestroySound();
	bool shouldContinueDestroySound();
};
