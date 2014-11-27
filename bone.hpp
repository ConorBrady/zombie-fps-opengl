
#include <string>
#include <vector>
#include <map>

class Animation;


// Represents a single bone complete with children.

class Bone {
private:

	// The offset matrix to translate a vertex to bone space.
	glm::mat4 _offsetMatrix;

	// The name of the bone used for identification.
	std::string _name;

	// The bones children
	std::vector<Bone*> _children;

	// A map of the bones associated animations
	std::map<std::string,Animation*> _animations;

	// The bones index in the shader index array.
	int _index;

public:
	Bone(aiBone* bone, int _index);
	std::string getName();
	std::vector<Bone*> getChildren();
	int getIndex();

	void addChild(Bone* child);
	void addAnimation(std::string name, aiNodeAnim* anim, float duration);

	glm::mat4 getTransformForAnimationAtTime(std::string name, float time);
};
