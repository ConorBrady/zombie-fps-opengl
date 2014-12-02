#include "mesh.hpp"
#include "controller_interface.hpp"
#include "camera.hpp"

class Bullet;

class Gun : public IControllable {
private:
	Camera* _camera;
	Mesh* _mesh;
	float _altY;
	float _altX;

	float _gunRanPitch;
	float _gunRanPitchSpeed;
	float _gunRanPitchAcceleration;
	float _gunRanYaw;
	float _gunRanYawSpeed;
	float _gunRanYawAcceleration;

	bool _fireGun;
	float _gunShotTime;
	Bullet** _bullets;
	int _lastBulletIndex;

	float _lastTick;

	glm::mat4 _M(float time);
public:
	Gun(Camera* camera);
	void update(float time);
	void draw(uint shader);
	void signal(ControlSignal cs, float value);
	glm::vec3 getWorldPosition();
};
