
#include "glm/glm.hpp"

#include "controller_interface.hpp"

#include "landscape.hpp"

using namespace glm;

class Camera : public virtual IControllable{

private:
	vec3 _xyz;
	float _pitch;
	float _yaw;

	float _pitchSpeed = 0;
	float _yawSpeed = 0;

	float _strideSpeed = 0;
	float _strafeSpeed = 0;

	float _lastUpdateTime = -1;

	float _runStart = 0;
	float _bounce = 0;

	Bounds* _worldBounds;

public:
	Camera(vec3 xyz, float pitch, float yaw, Bounds* worldBounds);

	void setPos(vec3 xyz);
	void signal(ControlSignal cs, float value);

	void update(uint shader, float time);

	float getPitch();
	float getYaw();
	
	glm::mat4 getViewMatrix();
	glm::vec3 getLocation();
};
