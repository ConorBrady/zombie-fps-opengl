#include <map>
#include <vector>

#include <assimp/scene.h>
#include <glm/glm.hpp>

#include "bone.hpp"

using namespace std;
using namespace glm;

class BoneAnimation {

private:
	vector<Bone*> _bones;
	vector<Bone*> _rootNodes;
	map<string,int> _boneNameToIndex;
	mat4* _transforms;

	Bone* _getBoneForName(string name);
	void _fillTransforms(Bone* parent, double time, mat4 parent_transform);
	void _addChildren(aiNode* current);

public:
	BoneAnimation(const aiScene* scene);
	const int numBones();
	const float * getBonesAtTime(double time);
};
