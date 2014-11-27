#include <map>
#include <vector>

#include <assimp/scene.h>

#include "bone.hpp"


class BoneAnimation {

private:
	std::vector<Bone*> _bones;
	std::vector<int> _rootNodes;
	glm::mat4* _transforms;
	glm::mat4 _globalInverseTransform;
	void _fillTransforms(Bone* parent, double time, glm::mat4 parent_transform);
	void _addChildren(aiNode* current);

public:
	BoneAnimation(const aiScene* scene);
	const int numBones();
	const float * getBonesAtTime(double time);
};
