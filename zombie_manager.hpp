#include "zombie.hpp"
#include "landscape.hpp"
#include "follow_interface.hpp"
#include "window.hpp"

#include <vector>

using namespace std;

class ZombieManager {
	// spawns and manages the waves of zombies
private:
	vector<Zombie*> _zombies;
	Bounds* _spawnBounds;
	vector<ITrackable*> _followables;
	Window* _window;
	int _waveNumber = 1;
	bool _anyAlive();
public:
	ZombieManager(Bounds* spawnBounds,Window* window);
	void addFollowable(ITrackable* followable);
	void update(float time);
	void draw(unsigned int shader);
	void reset();
};
