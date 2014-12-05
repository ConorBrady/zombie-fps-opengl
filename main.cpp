#define UPS 50.0

#include "player.hpp"
#include "score_manager.hpp"
#include "shader_loader.hpp"
#include "window.hpp"
#include "zombie_manager.hpp"

using namespace std;

int main () {


	Window* window = new Window(1200,1000);
	Shader* shader = new Shader("shader.vert","shader.frag");

	Landscape* landscape = Landscape::getLandscape();
	Player* player = new Player(vec3(0,0,0));

	ZombieManager* zombieManager = new ZombieManager(landscape->getBounds());
	zombieManager->addFollowable(player);

	window->addControllable(player);
	window->setShader(*shader);

	double last_tick = window->getTime();
	int update_count = 0;
	while (!window->shouldClose()) {

		player->update(*shader,last_tick);
		zombieManager->update(last_tick);;
		CollisionSpace::sharedCollisionSpace()->checkForCollisions();
		window->pollEvents();

		if(update_count%2 == 0) {
			zombieManager->draw(*shader);;
			landscape->draw(*shader,last_tick);
			player->draw(*shader);
			window->updateScore(ScoreManager::getSharedScoreManager()->getScore());
			window->drawText();
			
			window->presentBuffer();
			update_count = 0;
		}
		
		update_count ++;

		while(window->getTime()<last_tick+1/UPS);
		last_tick = window->getTime();
	}

	return 0;
}
