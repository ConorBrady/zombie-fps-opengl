#include "camera.hpp"

#include <GL/glew.h>

#define DEAD_ZONE 0.2
#define ANGULAR_SPEED 5
#define LINEAR_SPEED 10
#define BOUNCE_HEIGHT 0.25
#define BOUNCE_SPEED 7

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

	_strideSpeed = 0;
	_strafeSpeed = 0;

	_lastUpdateTime = -1;
	_runStart = 0;
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

void Camera::update(uint shader, float time) {

	float bounce = 0;
	if( _lastUpdateTime > 0 ) {

		float secondsPassed = time - _lastUpdateTime;

		if(_runStart==0 && _strideSpeed != 0) {
			_runStart = time;
		}
		if(_strideSpeed != 0 ) {
			bounce = fabs(sin((time-_runStart)*BOUNCE_SPEED)*BOUNCE_HEIGHT);
		}

		//_xyz.z -= secondsPassed*_strideSpeed*cos(_pitch); // NO FLYING
		_xyz.x += secondsPassed*_strideSpeed*sin(_pitch)*sin(_yaw);
		_xyz.y += secondsPassed*_strideSpeed*sin(_pitch)*cos(_yaw);

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

	mat4 V;

	V = rotate(mat4(1.0),_pitch,vec3(-1,0,0));
	V = rotate(V,_yaw,vec3(0,0,1));
	V = translate(V,vec3(-_xyz.x,-_xyz.y,-_xyz.z-5-bounce));

	int V_loc = glGetUniformLocation (shader, "V");
	glUniformMatrix4fv (V_loc, 1, GL_FALSE, glm::value_ptr(V));

	_lastUpdateTime = time;

}

glm::vec3 Camera::getLocation() {
	return _xyz;
}
