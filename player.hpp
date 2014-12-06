
#include "glm/glm.hpp"

#include "gun.hpp"
#include "controller_interface.hpp"
#include "follow_interface.hpp"
#include "collision_space.hpp"

class Player : public IControllable, public ICollidableCylinder {

private:
	Camera* _camera = nullptr;
	Gun* _gun = nullptr;
	bool _isDead = false;
	glm::vec3 _initialPos;

public:
	Player(glm::vec3 location, Bounds* worldBounds);
	
	bool isDead();
	void reset();
	void update(uint shader, float time);
	void draw(uint shader);

	#pragma mark IControllable methods

	void signal(ControlSignal cs, float value);

	#pragma mark ITrackable methods

	glm::vec3 getLocation();

	#pragma mark ICollidable methods

	bool isCollidable();
	void collided(ICollidable* collided);
	int getCollisionPoisons();

	#pragma mark ICollidableCylinder methods

	float getCollidableHeight();
	float getCollidableRadius();

};
