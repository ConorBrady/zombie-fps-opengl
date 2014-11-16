#define FPS 25.0

#include "camera.hpp"
#include "landscape.hpp"
#include "shader_loader.hpp"
#include "window.hpp"
#include "zombie.hpp"

using namespace std;

int main () {

	Window* window = new Window(800,800);
	Shader* shader = new Shader("shader.vert","shader.frag");
	Zombie* zombies[10][10];
	for(int i = 0; i < 10; i++) {
		for(int j = 0; j < 10; j++) {
			zombies[i][j] = new Zombie(i*10,j*10);
		}
	}

	Landscape* landscape = Landscape::getLandscape();
	Camera* camera = new Camera(vec3(0,-10,10),M_PI/4,0);

	window->addControllable(camera);
	window->setShader(*shader);

	double last_tick = window->getTime();

	while (!window->shouldClose()) {

		camera->update(*shader,last_tick);
		for(int i = 0; i < 10; i++) {
			for(int j = 0; j < 10; j++) {
				zombies[i][j]->draw(*shader,last_tick);;
			}
		}
		landscape->draw(*shader,last_tick);
		window->pollEvents();
		window->presentBuffer();

		while(window->getTime()<last_tick+1/FPS);
		last_tick = window->getTime();
	}

	return 0;
}
