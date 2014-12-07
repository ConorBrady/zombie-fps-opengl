#define FPS 25.0

#include "player.hpp"
#include "score_manager.hpp"
#include "shader_loader.hpp"
#include "zombie_manager.hpp"

using namespace std;

int main () {

	Window* window = new Window(1200,1000);
	Shader* shader = new Shader("shader.vert","shader.frag");

	Landscape* landscape = Landscape::getLandscape();
	Player* player = new Player(vec3(0,0,0),landscape->getBounds());

	ZombieManager* zombieManager = new ZombieManager(landscape->getBounds(),window);

	window->addControllable(player);
	window->setShader(*shader);

	while(!window->shouldClose()) {

		player->reset();
		zombieManager->reset();
		zombieManager->addFollowable(player);
		ScoreManager::getSharedScoreManager()->reset();
		CollisionSpace::sharedCollisionSpace()->clear();
		CollisionSpace::sharedCollisionSpace()->addCylinder(player);
		
		double last_tick = window->getTime();

		while (!window->shouldClose() && !player->isDead()) {

			player->update(*shader,last_tick);
			int t = glGetUniformLocation(*shader,"time");
			glUniform1f(t,last_tick);
			zombieManager->update(last_tick);;
			zombieManager->draw(*shader);;

			landscape->draw(*shader,last_tick);
			player->draw(*shader);

			CollisionSpace::sharedCollisionSpace()->checkForCollisions();

			window->updateScore(ScoreManager::getSharedScoreManager()->getScore());
			window->updateBulletCount(player->getRemainingBullets());
			window->drawText();
			window->pollEvents();
			window->presentBuffer();

			while(window->getTime()<last_tick+1/FPS);
			last_tick = window->getTime();
		}
		
	}
	return 0;
}
