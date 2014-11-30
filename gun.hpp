#include "mesh.hpp"
#include "controller_interface.hpp"

class Gun : public IControllable {
private:
	Mesh* _mesh;
	float _gunPitch;
	float _gunYaw;
	float _gunRandomPitch;
	float _gunRandomYaw;
	bool _fireGun;
	float _gunShotTime;
	float _lastTick;
public:
	Gun();
	void update(uint shader, float time);
	void draw(uint shader, float time, glm::mat4 viewMatrix);
	void signal(ControlSignal cs, float value);
};
