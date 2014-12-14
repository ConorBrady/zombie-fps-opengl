#include "notification_center.hpp"

class ScoreManager : public INotifiable {
	// singleton for getting the score, recieves game updates via the
	// notification center, and attributes scores accordingly
private:
	int _score = 0;
public:
	static ScoreManager* getSharedScoreManager();

	int getScore();
	void reset();

	#pragma mark INotifiable methods

	void respond(string notificationString, int value);
};
