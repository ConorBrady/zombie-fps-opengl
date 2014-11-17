
#include <string>
#include <vector>
#include <map>

#define GLM_FORCE_RADIANS

#include <glm/gtc/matrix_transform.hpp>
#include <assimp/scene.h>

class Animation;

using namespace glm;
using namespace std;

class Bone {
private:
	mat4 _offsetMatrix;
	string _name;
	vector<Bone*> _children;
	map<string,Animation*> _animations;
	int _index;

public:
	Bone(aiBone* bone, int _index);
	string getName();
	vector<Bone*> getChildren();
	int getIndex();

	void addChild(Bone* child);
	void addAnimation(string name, aiNodeAnim* anim, float duration);

	mat4 getTransformForAnimationAtTime(string name, float time);
};
