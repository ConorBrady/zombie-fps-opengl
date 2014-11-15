#include <map>

#include <assimp/scene.h>
#include <glm/glm.hpp>

using namespace std;
using namespace glm;

class BoneAnimation {

private:
	aiBone** _bones;
	aiAnimation* _animation;
	aiNode* _rootNode;
	int _numBones;
	map<string,int> _boneNameToIndex;
	mat4* _transforms;
	mat4 _globalInverseTransform;

	void _fillChildrenTransforms(aiNode* parent, double time, mat4 parent_transform);
	aiNodeAnim* _getNodeAnim(aiString name);

public:
	BoneAnimation(aiBone ** bones, int numBones, aiAnimation * animation, aiNode * rootNode);
	const int numBones();
	const float * getBonesAtTime(double time);
};
