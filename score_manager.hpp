#include "notification_center.hpp"

class ScoreManager : public INotifiable {
private:
	int _score = 0;
public:
	static ScoreManager* getSharedScoreManager();

	int getScore();
	void reset();

	#pragma mark INotifiable methods

	void respond(string notificationString, int value);
};
