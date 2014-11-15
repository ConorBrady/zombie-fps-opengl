#include "glm/glm.hpp"

using namespace glm;

const int CAM_STRAFE_LEFT = -1;
const int CAM_STRAFE_RIGHT = 1;
const int CAM_STRAFE_NONE = 0;

const int CAM_STRIDE_FORWARD = 1;
const int CAM_STRIDE_BACKWARD = -1;
const int CAM_STRIDE_NONE = 0;

class Camera {
private:
	vec3 _xyz;
	float _pitch;
	float _yaw;

	float _pitchSpeed;
	float _yawSpeed;

	float _strideSpeed;
	float _strafeSpeed;

	float _lastUpdateTime;
public:
	Camera(vec3 xyz, float pitch, float yaw);

	void setPitchSpeed(float radPerSec);
	void setYawSpeed(float radPerSec);

	void setStrideSpeed(float stride);
	float getStrideSpeed();

	void setStrafeSpeed(float strafe);
	float getStrafeSpeed();

	void update(uint shader, float time);
};
