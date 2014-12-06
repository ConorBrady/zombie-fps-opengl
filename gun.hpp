#include "mesh.hpp"
#include "controller_interface.hpp"
#include "camera.hpp"

class Bullet;

class Gun : public IControllable {
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

	glm::mat4 _M(float time);

public:
	Gun(Camera* camera);
	void update(float time);
	void draw(uint shader);
	void signal(ControlSignal cs, float value);
	glm::vec3 getWorldPosition();
	void destroyBullets();
};
