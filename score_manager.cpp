#include "score_manager.hpp"
#include <math.h>

static ScoreManager* _singleton = nullptr;

ScoreManager* ScoreManager::getSharedScoreManager() {
	if(_singleton == nullptr) {
		_singleton = new ScoreManager();
	}
	return _singleton;
}

int ScoreManager::getScore() {
	return _score*100*sqrt(_score);
}

void ScoreManager::reset() {
	_score = 0;
}

void ScoreManager::registerZombieKill() {
	_score ++;
}
