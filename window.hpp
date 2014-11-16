#include <vector>

#include "controller_interface.hpp"

class GLFWwindow;

using namespace std;

class Window : public virtual IController{

private:
	vector<IControllable*> _controllables;
	GLFWwindow* _window;
	unsigned int _shaderId;
public:
	Window(double width, double height);

	void addControllable(IControllable* controllable);

	void dispatchMessage(ControlSignal cs, float value);

	bool shouldClose();
	float getTime();

	void setShader(unsigned int shader);
	void didResize();
	void presentBuffer();
	void pollEvents();

};
