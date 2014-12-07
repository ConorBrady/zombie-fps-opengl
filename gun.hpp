#include "mesh.hpp"
#include "controller_interface.hpp"
#include "camera.hpp"
#include "notification_center.hpp"

class Bullet;

class Gun : public IControllable, public INotifiable {
private:
	Camera* _camera;
	Mesh* _mesh = new Mesh("resources/shotgun.dae");
	float _altY = 0;
	float _altX = 0;
	
	bool _fireGun = false;
	float _gunShotTime = -10;
	Bullet** _bullets = nullptr;
	int _lastBulletIndex = 0;

	float _lastTick = -1;
	int _currentWave = 0;

	int _remainingBullets = 0;

	glm::mat4 _M(float time);

public:
	Gun(Camera* camera);
	void update(float time);
	void draw(uint shader);
	glm::vec3 getWorldPosition();
	void reset();
	void addBullet(int count);
	int getRemainingBullets();

	#pragma mark IControllable methods

	void signal(ControlSignal cs, float value);

	#pragma mark INotifiable methods

	void respond(string notificationString, int value);
};
