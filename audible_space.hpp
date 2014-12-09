#include "irrKlang/irrKlang.h"
#include "follow_interface.hpp"
#include "camera.hpp"

#include <string>
#include <vector>

using namespace std;
using namespace irrklang;


#ifndef AUDIBLE
#define AUDIBLE

class IAudible {
public:

	virtual glm::vec3 getLocation() = 0;

	virtual const char* ambientSound() = 0;
	virtual float getAmbientSoundVolume() = 0;
	virtual bool shouldPlayAmbientSound() = 0;
	virtual bool shouldContinueAmbientSound() = 0;

	virtual const char* spawnSound() = 0;
	virtual bool shouldContinueSpawnSound() = 0;

	virtual const char* destroySound() = 0;
	virtual bool shouldPlayDestroySound() = 0;
	virtual bool shouldContinueDestroySound() = 0;
};

class AudibleSpace {
private:
	vector<IAudible*> _audibles;

	vector<ISound*> _ambientSounds;
	vector<ISound*> _spawnSounds;
	vector<ISound*> _destroySounds;

	ISoundEngine* _engine;
	ISound* _soundTrack;
	Camera* _camera;
	AudibleSpace();
public:
	static AudibleSpace* sharedAudibleSpace();

	void update(float time);
	void setCamera(Camera* camera);
	void addAudible(IAudible* audible);

	void setSoundTrack(const char* filename);
};

#endif
