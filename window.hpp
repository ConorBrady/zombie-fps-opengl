#include <vector>
#include <queue>

#include "controller_interface.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

using namespace std;

class Window : public virtual IController{

private:
	vector<IControllable*> _controllables;
	GLFWwindow* _window;
	unsigned int _shaderId;
	float _aspectRatio;
	int _scoreTextId;
	int _toastTextId;
	float _toastDisplayTime = -1;
	queue<const char*> _toastQueue;
public:
	Window(double width, double height);

	void addControllable(IControllable* controllable);

	void dispatchMessage(ControlSignal cs, float value);

	bool shouldClose();
	float getTime();
	float getAspectRatio();

	void setShader(unsigned int shader);
	void didResize();
	void updateScore(int score);
	void drawText();
	void presentBuffer();
	void pollEvents();

	void queueToast(const char* toast);
};
