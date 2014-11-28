
#include "glm/glm.hpp"

#include "controller_interface.hpp"

using namespace glm;

class Camera : public virtual IControllable{

private:
	vec3 _xyz;
	float _pitch;
	float _yaw;

	float _pitchSpeed;
	float _yawSpeed;

	float _strideSpeed;
	float _strafeSpeed;

	float _lastUpdateTime;

	float _runStart;
public:
	Camera(vec3 xyz, float pitch, float yaw);

	void signal(ControlSignal cs, float value);

	void update(uint shader, float time);

	glm::vec3 getLocation();
};
