class ScoreManager {
private:
	int _score = 0;
public:
	static ScoreManager* getSharedScoreManager();

	int getScore();
	void reset();
	void registerZombieKill();
};
