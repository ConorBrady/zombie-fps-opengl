#include "zombie.hpp"
#include "landscape.hpp"
#include "follow_interface.hpp"

#include <vector>

using namespace std;

class ZombieManager {
private:
	vector<Zombie*> _zombies;
	Bounds* _spawnBounds;
	vector<IFollowable*> _followables;
public:
	ZombieManager(Bounds* spawnBounds);
	void addFollowable(IFollowable* followable);
	void update(float time);
	void draw(unsigned int shader);
};
