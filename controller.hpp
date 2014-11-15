#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "camera.hpp"

class Controller {

private:
	Camera* _camera;

public:
	static Controller* getController();
	void setCamera(Camera* camera);
	Camera* getCamera();
	void setWindow(GLFWwindow* window);
};
