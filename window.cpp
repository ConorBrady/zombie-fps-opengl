#include "window.hpp"

#include <map>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "controller_interface.hpp"

static map<GLFWwindow*,Window*> windows;

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {

	Window* windowControl = windows[window];

	if (action==GLFW_PRESS){
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
			glfwSetWindowShouldClose(window, GL_TRUE);
		}

		if (key == GLFW_KEY_UP) {
			windowControl->dispatchMessage(CONTROL_SIGNAL_PRIMARY_Y,1.0f);
		}

		if (key == GLFW_KEY_DOWN) {
			windowControl->dispatchMessage(CONTROL_SIGNAL_PRIMARY_Y,-1.0f);
		}

		if(key == GLFW_KEY_LEFT) {
			windowControl->dispatchMessage(CONTROL_SIGNAL_PRIMARY_X,-1.0f);
		}

		if(key == GLFW_KEY_RIGHT) {
			windowControl->dispatchMessage(CONTROL_SIGNAL_PRIMARY_X,1.0f);
		}
	}

	if (action == GLFW_RELEASE) {
		if(key == GLFW_KEY_UP || key == GLFW_KEY_DOWN) {
			windowControl->dispatchMessage(CONTROL_SIGNAL_PRIMARY_Y,0.0f);
		}

		if(key == GLFW_KEY_LEFT || key == GLFW_KEY_RIGHT) {
			windowControl->dispatchMessage(CONTROL_SIGNAL_PRIMARY_X,0.0f);
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

	windowControl->dispatchMessage(CONTROL_SIGNAL_ALT_X,dyaw);

	float dpitch = (((height-cursor_y)-height/2)/height);

	windowControl->dispatchMessage(CONTROL_SIGNAL_ALT_Y,dpitch);
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
	windows[_window] = this;

	glewExperimental = GL_TRUE;
	glewInit ();

	glEnable (GL_DEPTH_TEST);
	glDepthFunc (GL_LESS);
	glClearColor (0.0, 0.0, 0.0, 1.0);
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

void Window::setShader(unsigned int shader) {
	_shaderId = shader;
	didResize();
}

void Window::didResize() {
	int width, height;
	glfwGetWindowSize(_window, &width, &height);

	glm::mat4 P = glm::perspective(70.0f,(float)width/(float)height,0.1f,1000.0f);
	int P_loc = glGetUniformLocation (_shaderId, "P");
	glUniformMatrix4fv (P_loc, 1, GL_FALSE, glm::value_ptr(P));
}

void Window::presentBuffer() {
	glfwSwapBuffers(_window);
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
void Window::pollEvents() {
	glfwPollEvents();
}
