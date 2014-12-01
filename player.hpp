
#include "glm/glm.hpp"

#include "gun.hpp"
#include "controller_interface.hpp"
#include "follow_interface.hpp"

class Player : public IControllable, public IFollowable {

private:
	Camera* _camera;
	Gun* _gun;

public:
	Player(glm::vec3 location);
	glm::vec3 getLocation();
	void update(uint shader, float time);
	void draw(uint shader, float time);
	void signal(ControlSignal cs, float value);
};
