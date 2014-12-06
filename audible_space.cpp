#include "audible_space.hpp"

static AudibleSpace* _singleton = nullptr;

AudibleSpace* AudibleSpace::sharedAudibleSpace() {
	if(_singleton==nullptr) {
		_singleton = new AudibleSpace();
	}
	return _singleton;
}

void AudibleSpace::setEar(ITrackable* ear) {
	_ear = ear;
}

void AudibleSpace::addAudible(IAudible* audible) {
	_audibles.push_back(audible);
}