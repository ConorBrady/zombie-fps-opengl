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
	// An object that makes sound in the 3D space
public:

	virtual glm::vec3 getLocation() = 0; // Needed for 3D sound positioning

	virtual const char* ambientSound() = 0; // filename for the Ambient sound
	virtual float getAmbientSoundVolume() = 0; // Volume at this moment in time
	virtual bool shouldPlayAmbientSound() = 0;
	virtual bool shouldContinueAmbientSound() = 0;

	virtual const char* spawnSound() = 0;
	virtual bool shouldContinueSpawnSound() = 0; // possibility to stop if dead

	virtual const char* destroySound() = 0;
	virtual bool shouldPlayDestroySound() = 0;
	virtual bool shouldContinueDestroySound() = 0;
};

class AudibleSpace {
	// The singleton object that takes care of all the audio in the game in 3D
	// space
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
	static AudibleSpace* sharedAudibleSpace(); // singleton access

	void update(float time); // stop or start sounds if needed with time
	void setCamera(Camera* camera); // camera to orientate 3D sound
	void addAudible(IAudible* audible); // add audible object to the space

	void setSoundTrack(const char* filename); // Looped background audio
};

#endif
