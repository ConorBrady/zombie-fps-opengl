#include "camera.hpp"

#include <GL/glew.h>

#define DEAD_ZONE 0.2
#define ANGULAR_SPEED 10
#define LINEAR_SPEED 15
#define BOUNCE_HEIGHT 0.50
#define BOUNCE_SPEED 7

#include "landscape.hpp"

#define GLM_FORCE_RADIANS

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

using namespace glm;

Camera::Camera(vec3 xyz, float pitch, float yaw, Bounds* worldBounds) {
	_xyz = xyz;
	_pitch = pitch;
	_yaw = yaw;

	_worldBounds = worldBounds;
}

void Camera::setPos(vec3 xyz) {
	_xyz = xyz;
}

void Camera::signal(ControlSignal cs, float value) {

	value = fmin(1.0f,value);
	value = fmax(-1.0f,value);

	switch(cs) {
		case CONTROL_SIGNAL_PRIMARY_X:
			_strafeSpeed = value*LINEAR_SPEED;
			break;
		case CONTROL_SIGNAL_PRIMARY_Y:
			_strideSpeed = value*LINEAR_SPEED;
			if(value == 0) {
				_runStart = 0;
			}
			break;
		case CONTROL_SIGNAL_ALT_X:
			if(value > DEAD_ZONE) {
				value = (value-DEAD_ZONE);
			}else if(value < -DEAD_ZONE) {
				value = (value+DEAD_ZONE);
			} else {
				value = 0.0f;
			}
			_yawSpeed = value*ANGULAR_SPEED;
			break;
		case CONTROL_SIGNAL_ALT_Y:
			if(value > DEAD_ZONE) {
				value = (value-DEAD_ZONE);
			} else if(value < -DEAD_ZONE) {
				value = (value+DEAD_ZONE);
			} else {
				value = 0.0f;
			}
			_pitchSpeed = value*ANGULAR_SPEED;
			break;
		default:
			break;
	}
}

void Camera::update(unsigned int shader, float time) {

	_bounce = 0;
	if( _lastUpdateTime > 0 ) {

		float secondsPassed = time - _lastUpdateTime;

		if(_runStart==0 && _strideSpeed != 0) {
			_runStart = time;
		}
		if(_strideSpeed != 0 ) {
			_bounce = fabs(sin((time-_runStart)*BOUNCE_SPEED)*BOUNCE_HEIGHT);
		}

		//_xyz.z -= secondsPassed*_strideSpeed*cos(_pitch); // NO FLYING
		_xyz.x += secondsPassed*_strideSpeed*sin(_pitch)*sin(_yaw);
		_xyz.x = fmax(fmin(_xyz.x,_worldBounds->x2),_worldBounds->x1);
		_xyz.y += secondsPassed*_strideSpeed*sin(_pitch)*cos(_yaw);
		_xyz.y = fmax(fmin(_xyz.y,_worldBounds->y2),_worldBounds->y1);

		int strafeSpeedSign = (_strafeSpeed > 0) - (_strafeSpeed < 0);
		_xyz.x += secondsPassed*fabs(_strafeSpeed)*sin(_yaw+strafeSpeedSign*M_PI/2);
		_xyz.y += secondsPassed*fabs(_strafeSpeed)*cos(_yaw+strafeSpeedSign*M_PI/2);

		_pitch += secondsPassed * _pitchSpeed;
		_yaw += secondsPassed * _yawSpeed;

		if(_pitch>M_PI) {
			_pitch=M_PI;
		}
		if(_pitch<0) {
			_pitch = 0;
		}
	}

	int V_loc = glGetUniformLocation (shader, "V");
	glUniformMatrix4fv (V_loc, 1, GL_FALSE, glm::value_ptr(getViewMatrix()));

	_lastUpdateTime = time;

}

float Camera::getPitch() {
	return _pitch;
}

float Camera::getYaw() {
	return _yaw;
}

glm::mat4 Camera::getViewMatrix() {
	mat4 V;

	V = rotate(mat4(1.0),_pitch,vec3(-1,0,0));
	V = rotate(V,_yaw,vec3(0,0,1));
	V = translate(V,vec3(-_xyz.x,-_xyz.y,-_xyz.z-5-_bounce));
	return V;
}

glm::vec3 Camera::getLocation() {
	return _xyz;
}
