
#include "glm/glm.hpp"

#include "controller_interface.hpp"

#include "landscape.hpp"

#ifndef CAMERA
#define CAMERA

class Camera : public virtual IControllable{
 	// object representing the player camera view, this is responsible for
	// updating the view matrix in the shaders
private:
	glm::vec3 _xyz;
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
	Camera(glm::vec3 xyz, float pitch, float yaw, Bounds* worldBounds);

	void setPos(glm::vec3 xyz);
	void signal(ControlSignal cs, float value);

	void update(unsigned int shader, float time);

	float getPitch();
	float getYaw();

	glm::mat4 getViewMatrix();
	glm::vec3 getLocation();
};
#endif
