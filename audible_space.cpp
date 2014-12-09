#include "audible_space.hpp"

#define GLM_FORCE_RADIANS

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <iostream>

using namespace glm;
using namespace std;

static AudibleSpace* _singleton = nullptr;

AudibleSpace* AudibleSpace::sharedAudibleSpace() {
	if(_singleton==nullptr) {
		_singleton = new AudibleSpace();
	}
	return _singleton;
}

AudibleSpace::AudibleSpace() {
	_engine = createIrrKlangDevice();
}

void AudibleSpace::setCamera(Camera* camera) {
	_camera = camera;
}

void AudibleSpace::addAudible(IAudible* audible) {
	_audibles.push_back(audible);

	_ambientSounds.push_back(nullptr);
	_spawnSounds.push_back(nullptr);
	_destroySounds.push_back(nullptr);
}

void AudibleSpace::update(float time) {
	vec3 pos = _camera->getLocation();
	float pitch = _camera->getPitch();
	float yaw = _camera->getYaw();

	vec3df position(pos.x,pos.y,pos.z);
	vec3df lookDirection(sin(pitch)*sin(yaw),sin(pitch)*cos(yaw),cos(pitch));
	vec3df velPerSecond(0,0,0);
	vec3df upVector(0,0,1);

	_engine->setListenerPosition(position, lookDirection, velPerSecond, upVector);

	for(int i = 0; i < _audibles.size(); i++) {
		if(_ambientSounds[i] == nullptr || _ambientSounds[i]->isFinished()){
			if(_audibles[i]->shouldPlayAmbientSound()) {
				_ambientSounds[i] = _engine->play3D(_audibles[i]->ambientSound(),position,false,true,true,ESM_AUTO_DETECT,true);
				_ambientSounds[i]->setVolume(_audibles[i]->getAmbientSoundVolume());
				_ambientSounds[i]->setIsPaused(false);
			}
		} else if(!_audibles[i]->shouldContinueAmbientSound()){
			_ambientSounds[i]->stop();
		}
		if(_ambientSounds[i] != nullptr && !_ambientSounds[i]->isFinished()){
			vec3 pos = _audibles[i]->getLocation();
			vec3df position(pos.x,pos.y,pos.z);
			_ambientSounds[i]->setPosition(position);
		}

		if(_spawnSounds[i] == nullptr ) {
			if(_audibles[i]->spawnSound() != nullptr){
				_spawnSounds[i] = _engine->play3D(_audibles[i]->spawnSound(),position,false,true,true,ESM_AUTO_DETECT,true);
				_spawnSounds[i]->setIsPaused(false);
			}
		} else if(!_audibles[i]->shouldContinueSpawnSound()){
			_spawnSounds[i]->stop();
		}
		if(_spawnSounds[i] != nullptr && !_spawnSounds[i]->isFinished()){
			vec3 pos = _audibles[i]->getLocation();
			vec3df position(pos.x,pos.y,pos.z);
			_spawnSounds[i]->setPosition(position);
		}

		if(_destroySounds[i] == nullptr){
			if(_audibles[i]->shouldPlayDestroySound()) {
				_destroySounds[i] = _engine->play3D(_audibles[i]->destroySound(),position,false,true,true,ESM_AUTO_DETECT,true);
				_destroySounds[i]->setIsPaused(false);
			}
		} else if(!_audibles[i]->shouldContinueDestroySound()){
			_destroySounds[i]->stop();
		}
		if(_destroySounds[i] != nullptr && !_destroySounds[i]->isFinished()){
			vec3 pos = _audibles[i]->getLocation();
			vec3df position(pos.x,pos.y,pos.z);
			_destroySounds[i]->setPosition(position);
		}
	}
}

void AudibleSpace::setSoundTrack(const char* filename) {
	_soundTrack = _engine->play2D(filename,true,true,true,ESM_AUTO_DETECT,true);
	_soundTrack->setIsPaused(false);
	_soundTrack->setVolume(0.3);
}
