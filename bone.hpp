
#include <string>
#include <vector>
#include <map>

#define GLM_FORCE_RADIANS

#include <glm/gtc/matrix_transform.hpp>
#include <assimp/scene.h>

class Animation;

using namespace glm;
using namespace std;

// Represents a single bone complete with children.

class Bone {
private:

	// The offset matrix to translate a vertex to bone space.
	mat4 _offsetMatrix

	// The name of the bone used for identification.
	string _name;

	// The bones children
	vector<Bone*> _children;

	// A map of the bones associated animations
	map<string,Animation*> _animations;

	// The bones index in the shader index array.
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
