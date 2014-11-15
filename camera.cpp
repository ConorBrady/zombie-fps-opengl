#include "camera.hpp"

#include <GL/glew.h>

#define GLM_FORCE_RADIANS

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

using namespace glm;

Camera::Camera(vec3 xyz, float pitch, float yaw) {
	_xyz = xyz;
	_pitch = pitch;
	_yaw = yaw;

	_pitchSpeed = 0;
	_yawSpeed = 0;

	_strideSpeed = CAM_STRIDE_NONE;
	_strafeSpeed = CAM_STRAFE_NONE;

	_lastUpdateTime = -1;
}

void Camera::setPitchSpeed(float radPerSec) {
	_pitchSpeed = radPerSec;
}

void Camera::setYawSpeed(float radPerSec) {
	_yawSpeed = radPerSec;
}

void Camera::setStrideSpeed(float stride) {
	_strideSpeed = stride;
}

float Camera::getStrideSpeed() {
	return _strideSpeed;
}

void Camera::setStrafeSpeed(float strafe) {
	_strafeSpeed = strafe;
}

float Camera::getStrafeSpeed() {
	return _strafeSpeed;
}

void Camera::update(uint shader, float time) {

	if( _lastUpdateTime > 0 ) {

		float secondsPassed = time - _lastUpdateTime;

		_xyz.z -= secondsPassed*_strideSpeed*cos(_pitch);
		_xyz.x += secondsPassed*_strideSpeed*sin(_pitch)*sin(_yaw);
		_xyz.y += secondsPassed*_strideSpeed*sin(_pitch)*cos(_yaw);

		_xyz.x += secondsPassed*abs(_strafeSpeed)*sin(_yaw+_strafeSpeed*M_PI/2);
		_xyz.y += secondsPassed*abs(_strafeSpeed)*cos(_yaw+_strafeSpeed*M_PI/2);

		_pitch += secondsPassed * _pitchSpeed;
		_yaw += secondsPassed * _yawSpeed;

		if(_pitch>M_PI) {
			_pitch=M_PI;
		}
		if(_pitch<0) {
			_pitch = 0;
		}
	}

	mat4 V;

	V = rotate(mat4(1.0),_pitch,vec3(-1,0,0));
	V = rotate(V,_yaw,vec3(0,0,1));
	V = translate(V,vec3(-_xyz.x,-_xyz.y,-_xyz.z));

	int V_loc = glGetUniformLocation (shader, "V");
	glUniformMatrix4fv (V_loc, 1, GL_FALSE, glm::value_ptr(V));

	_lastUpdateTime = time;

}
