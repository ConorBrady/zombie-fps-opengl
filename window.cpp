#include "window.hpp"

#include <map>
#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS

#define FIELD_OF_VIEW (M_PI/3)
#define TOAST_TIME 3

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "text/text.h"

#include "controller_interface.hpp"

using namespace std;

static map<GLFWwindow*,Window*> windows;

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {

	Window* windowControl = windows[window];

	if (action==GLFW_PRESS){
		switch(key) {
			case GLFW_KEY_ESCAPE:
				glfwSetWindowShouldClose(window, GL_TRUE);
				break;
		 	case GLFW_KEY_UP:
			case GLFW_KEY_W:
				windowControl->dispatchMessage(CONTROL_SIGNAL_PRIMARY_Y,1.0f);
				break;

		 	case GLFW_KEY_DOWN:
			case GLFW_KEY_S:
				windowControl->dispatchMessage(CONTROL_SIGNAL_PRIMARY_Y,-1.0f);
				break;

		  	case GLFW_KEY_LEFT:
			case GLFW_KEY_A:
				windowControl->dispatchMessage(CONTROL_SIGNAL_PRIMARY_X,-1.0f);
				break;

			case GLFW_KEY_RIGHT:
			case GLFW_KEY_D:
				windowControl->dispatchMessage(CONTROL_SIGNAL_PRIMARY_X,1.0f);
				break;

			case GLFW_KEY_SPACE:
				windowControl->dispatchMessage(CONTROL_SIGNAL_PRIMARY_ACTION,1);
				break;
			default:
				break;
		}
	}

	if (action == GLFW_RELEASE) {
		switch(key) {
			case GLFW_KEY_UP:
			case GLFW_KEY_W:
			case GLFW_KEY_DOWN:
			case GLFW_KEY_S:
				windowControl->dispatchMessage(CONTROL_SIGNAL_PRIMARY_Y,0.0f);
				break;
			case GLFW_KEY_LEFT:
			case GLFW_KEY_A:
			case GLFW_KEY_RIGHT:
			case GLFW_KEY_D:
				windowControl->dispatchMessage(CONTROL_SIGNAL_PRIMARY_X,0.0f);
				break;
			default:
				break;
		}
	}
}

void cursorPositionCallback(GLFWwindow * window, double x, double y) {

	Window* windowControl = windows[window];

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

	float dyaw = ((cursor_x-width/2)/width);
	windowControl->dispatchMessage(CONTROL_SIGNAL_ALT_X,dyaw*FIELD_OF_VIEW*windowControl->getAspectRatio());

	float dpitch = (((height-cursor_y)-height/2)/height);
	windowControl->dispatchMessage(CONTROL_SIGNAL_ALT_Y,dpitch*FIELD_OF_VIEW);
}

void mouseButtonCallback(GLFWwindow * window, int button, int action, int mods) {

	Window* windowControl = windows[window];

	if(button==GLFW_MOUSE_BUTTON_1 && action == GLFW_PRESS) {
		windowControl->dispatchMessage(CONTROL_SIGNAL_PRIMARY_ACTION,1);
	}
}


void windowSizeCallback(GLFWwindow* window, int width, int height) {
	windows[window]->didResize();
}

Window::Window(double width, double height) {

	if (!glfwInit ()) {
		fprintf (stderr, "ERROR: could not start GLFW3\n");
	}

	glfwWindowHint (GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint (GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint (GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	_window = glfwCreateWindow (width, height, "Hello Triangle", NULL, NULL);
	if (!_window) {
		fprintf (stderr, "ERROR: opening OS window\n");
	}
	glfwMakeContextCurrent (_window);

	glfwSetKeyCallback(_window, keyCallback);

	glfwSetCursorPosCallback(_window, cursorPositionCallback);
	glfwSetWindowSizeCallback(_window, windowSizeCallback);
	glfwSetMouseButtonCallback(_window, mouseButtonCallback);

	windows[_window] = this;

	glewExperimental = GL_TRUE;
	glewInit ();

	glEnable (GL_DEPTH_TEST);
	glDepthFunc (GL_LESS);
	glClearColor (0.0, 0.0, 0.0, 1.0);

	init_text_rendering ("resources/freemono.png", "resources/freemono.meta", width, height);
	_scoreTextId = add_text("",0.4f, -0.8f, 100.0f, 1.0f, 1.0f, 1.0f, 1.0f);
	_toastTextId = add_text("",0.4f, -0.7f, 100.0f, 1.0f, 1.0f, 1.0f, 1.0f);
}

void Window::addControllable(IControllable* controllable) {
	_controllables.push_back(controllable);
}

void Window::dispatchMessage(ControlSignal cs, float value) {
	for(int i = 0; i < _controllables.size(); i++) {
		_controllables[i]->signal(cs,value);
	}
}

bool Window::shouldClose() {
	return glfwWindowShouldClose(_window);
}
float Window::getTime() {
	return glfwGetTime();
}

float Window::getAspectRatio() {
	return _aspectRatio;
}
void Window::setShader(unsigned int shader) {
	_shaderId = shader;
	didResize();
}

void Window::didResize() {
	int width, height;
	glfwGetWindowSize(_window, &width, &height);
	_aspectRatio = (float)width/(float)height;
	glm::mat4 P = glm::perspective((float)FIELD_OF_VIEW,_aspectRatio,0.1f,1000.0f);
	int P_loc = glGetUniformLocation (_shaderId, "P");
	glUniformMatrix4fv (P_loc, 1, GL_FALSE, glm::value_ptr(P));
}

void Window::updateScore(int score) {
	char tmp[7];
	sprintf (tmp, "Score: %06d\n", score);
	update_text (_scoreTextId, tmp);
}

void Window::queueToast(const char* toast) {
	_toastQueue.push(toast);
}

void Window::drawText() {
	if(_toastQueue.size() > 0 ) {
		if(_toastDisplayTime < 0 ) {
			update_text (_toastTextId, _toastQueue.front());
			_toastDisplayTime = getTime();
		} else if( getTime()-_toastDisplayTime > TOAST_TIME) {
			_toastDisplayTime = -1;
			_toastQueue.pop();
		}
	} else {
		update_text(_toastTextId, "");
	}
	draw_texts();
	glUseProgram(_shaderId); // Undo sideeffect of draw_texts()
}

void Window::presentBuffer() {

	glfwSwapBuffers(_window);
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
void Window::pollEvents() {
	glfwPollEvents();
}
