

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>

#define FPS 25.0

#define GLM_FORCE_RADIANS

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "zombie.hpp"

#include "controller.hpp"

#include "shader_loader.hpp"

using namespace std;


int gl_width = 800;
int gl_height = 800;


GLFWwindow* window_init() {
	GLFWwindow* window = NULL;
	const GLubyte* renderer;
	const GLubyte* version;
	if (!glfwInit ()) {
		fprintf (stderr, "ERROR: could not start GLFW3\n");
		return window;
	}

	glfwWindowHint (GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint (GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint (GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow (gl_width, gl_height, "Hello Triangle", NULL, NULL);
	if (!window) {
		fprintf (stderr, "ERROR: opening OS window\n");
		return window;
	}
	glfwMakeContextCurrent (window);

	glewExperimental = GL_TRUE;
	glewInit ();

	renderer = glGetString (GL_RENDERER);
	version = glGetString (GL_VERSION);
	printf ("Renderer: %s\n", renderer);
	printf ("OpenGL version supported %s\n", version);
	glEnable (GL_DEPTH_TEST);
	glDepthFunc (GL_LESS);
	glClearColor (0.5, 0.5, 0.5, 1.0);



	return window;
}



int main () {

	GLFWwindow* window = window_init();

	GLuint shader_programme = ShaderLoader::getProgramId("shader.vert","shader.frag");

	Zombie* zombie = new Zombie(0,0);
	Camera* camera = new Camera(vec3(0,-5,5),M_PI/4,0);

	Controller* controller = Controller::getController();
	controller->setCamera(camera);
	controller->setWindow(window);

	double last_tick = glfwGetTime();

	glUseProgram (shader_programme);

	glm::mat4 P = glm::perspective(70.0f,1.0f,0.1f,100.0f);
	int P_loc = glGetUniformLocation (shader_programme, "P");
	glUniformMatrix4fv (P_loc, 1, GL_FALSE, glm::value_ptr(P));

	while (!glfwWindowShouldClose (window)) {

		glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		camera->update(shader_programme,last_tick);
		zombie->draw(shader_programme,last_tick);

		glfwPollEvents();
		glfwSwapBuffers(window);

		while(glfwGetTime()<last_tick+1/FPS);
		last_tick = glfwGetTime();
	}

	return 0;
}
