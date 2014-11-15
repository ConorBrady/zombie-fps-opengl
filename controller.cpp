#include "controller.hpp"

const static float ANGULAR_SPEED = 5.0;
const static float STRAIGHT_SPEED = 10.0;

static Controller* _singleton = NULL;

static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
static void cursorPosCallback(GLFWwindow * window, double x, double y);

Controller* Controller::getController() {
	if(_singleton == NULL) {
		_singleton = new Controller();
		_singleton->_camera = NULL;
	}

	return _singleton;
}

void Controller::setCamera(Camera* camera) {
	_camera = camera;
}

Camera* Controller::getCamera() {
	return _camera;
}

void Controller::setWindow(GLFWwindow* window) {
	glfwSetKeyCallback(window, keyCallback);
	glfwSetCursorPosCallback(window, cursorPosCallback);
}

static void cursorPosCallback(GLFWwindow * window, double x, double y) {

	Camera* camera = Controller::getController()->getCamera();

	int width, height;
	glfwGetWindowSize(window, &width, &height);

	float cursor_x, cursor_y;

	if( x > 0 && x < width ) {
		cursor_x = x;

		if( y > 0 && y < height ){
			cursor_y = y;
		}
		else {
			cursor_x = width/2;
			cursor_y = height/2;
		}
	}
	else {
		cursor_x = width/2;
		cursor_y = height/2;
	}

	float yawSpeed, pitchSpeed;

	float dyaw = ((cursor_x-width/2)/width);
	if(dyaw > 0.2) {
		yawSpeed += (dyaw-0.2)*1.25*ANGULAR_SPEED;
	}
	if(dyaw < -0.2) {
		yawSpeed += (dyaw+0.2)*1.25*ANGULAR_SPEED;
	}
	camera->setYawSpeed(yawSpeed);

	float dpitch = (((height-cursor_y)-height/2)/height);
	if(dpitch > 0.2){
		pitchSpeed += (dpitch-0.2)*1.25*ANGULAR_SPEED;
	}
	if(dpitch < -0.2) {
		pitchSpeed += (dpitch+0.2)*1.25*ANGULAR_SPEED;
	}
	camera->setPitchSpeed(pitchSpeed);
}

static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	Camera* camera = Controller::getController()->getCamera();

	if (action==GLFW_PRESS){
	    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
	        glfwSetWindowShouldClose(window, GL_TRUE);
		}

		if (key == GLFW_KEY_UP) {
			camera->setStrideSpeed(CAM_STRIDE_FORWARD*STRAIGHT_SPEED);
		}

		if (key == GLFW_KEY_DOWN) {
			camera->setStrideSpeed(CAM_STRIDE_BACKWARD*STRAIGHT_SPEED);
		}

		if(key == GLFW_KEY_LEFT) {
			camera->setStrafeSpeed(CAM_STRAFE_LEFT*STRAIGHT_SPEED);
		}

		if(key == GLFW_KEY_RIGHT) {
			camera->setStrafeSpeed(CAM_STRAFE_RIGHT*STRAIGHT_SPEED);
		}
	}
	if (action==GLFW_RELEASE) {
		if(key==GLFW_KEY_UP && camera->getStrideSpeed() == CAM_STRIDE_FORWARD*STRAIGHT_SPEED) {
			camera->setStrideSpeed(CAM_STRIDE_NONE);
		}
		if(key==GLFW_KEY_DOWN && camera->getStrideSpeed() == CAM_STRIDE_BACKWARD*STRAIGHT_SPEED) {
			camera->setStrideSpeed(CAM_STRIDE_NONE);
		}

		if(key == GLFW_KEY_LEFT && camera->getStrafeSpeed() == CAM_STRAFE_LEFT*STRAIGHT_SPEED) {
			camera->setStrafeSpeed(CAM_STRAFE_NONE);
		}

		if(key == GLFW_KEY_RIGHT && camera->getStrafeSpeed() == CAM_STRAFE_RIGHT*STRAIGHT_SPEED) {
			camera->setStrafeSpeed(CAM_STRAFE_NONE);
		}
	}
}
