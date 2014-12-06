
class IAudible : public ITrackable {
	virtual string spawnSound() = 0;
	virtual string ambientSound() = 0;
};

class AudibleSpace {
private:
	ITrackable* _ear;
	vector<IAudible*> _audibles;
public:
	static AudibleSpace* sharedAudibleSpace();
	
	void setEar(ITrackable* ear);
	void addAudible(IAudible* audible);
};