#define FPS 25.0

#include "player.hpp"
#include "landscape.hpp"
#include "shader_loader.hpp"
#include "window.hpp"
#include "zombie.hpp"

using namespace std;

int main () {

	Window* window = new Window(800,800);
	Shader* shader = new Shader("shader.vert","shader.frag");


	Landscape* landscape = Landscape::getLandscape();
	Player* player = new Player(vec3(0,-10,0));

	Zombie* zombies[5][5];
	for(int i = 0; i < 5; i++) {
		for(int j = 0; j < 5; j++) {
			zombies[i][j] = new Zombie(glm::vec3(i*10-20,j*10,0));
			zombies[i][j]->addFollowable(player);
		}
	}

	window->addControllable(player);
	window->setShader(*shader);

	double last_tick = window->getTime();

	while (!window->shouldClose()) {

		player->update(*shader,last_tick);
		for(int i = 0; i < 5; i++) {
			for(int j = 0; j < 5; j++) {
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
