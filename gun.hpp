#include "mesh.hpp"
#include "controller_interface.hpp"
#include "camera.hpp"
#include "bullet.hpp"

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
	void update(uint shader, float time);
	void draw(uint shader, float time);
	void signal(ControlSignal cs, float value);
	glm::vec3 getWorldPosition();
};
