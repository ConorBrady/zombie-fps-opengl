#define GLM_FORCE_RADIANS

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

#include <iostream>
#include <map>

using namespace std;

class BoneAnimation {

private:
	aiBone** _bones;
	aiAnimation* _animation;
	aiNode* _rootNode;
	int _numBones;
	map<string,int> _boneNameToIndex;
	glm::mat4* _transforms;
	glm::mat4 _globalInverseTransform;

public:
	BoneAnimation(aiBone ** bones, int numBones, aiAnimation * animation, aiNode * rootNode) {

		_bones = bones;
		_animation = animation;
		_rootNode = rootNode;
		_numBones = numBones;
		_globalInverseTransform = _mat4Cast(rootNode->mTransformation.Inverse());


		for(int i = 0; i < numBones; i++) {
			_boneNameToIndex[string(bones[i]->mName.C_Str())] = i;
		}
		std::cout << "Called constructor, with "<< _numBones << "." << std::endl;
	}

	const float * getBonesAtTime(double time) {
		free(_transforms);
		_transforms = (glm::mat4*)malloc(sizeof(glm::mat4)*_numBones);
		_fillChildrenTransforms(_rootNode,fmod(time,_animation->mDuration));
		return glm::value_ptr(_transforms[0]);
	}

	glm::mat4 _mat4Cast(const aiMatrix4x4 &value)
    {
        glm::mat4 result;
        result[0] = glm::vec4(value[0][0], value[1][0], value[2][0], value[3][0]);
        result[1] = glm::vec4(value[0][1], value[1][1], value[2][1], value[3][1]);
        result[2] = glm::vec4(value[0][2], value[1][2], value[2][2], value[3][2]);
        result[3] = glm::vec4(value[0][3], value[1][3], value[2][3], value[3][3]);

        return result;
    }


	glm::quat _quatCast(const aiQuaternion &value)
    {
        return glm::quat(value.w, value.x, value.y, value.z);
    }

	void _fillChildrenTransforms(aiNode* parent, double time) {

		glm::mat4 parent_transform;
		if(_boneNameToIndex.find(string(parent->mName.C_Str())) != _boneNameToIndex.end()) {
			parent_transform = _transforms[_boneNameToIndex[string(parent->mName.C_Str())]];
		} else {
			parent_transform = glm::mat4(1.0);
		}

		for(int i = 0; i < parent->mNumChildren; i++) {

			aiNode* node = parent->mChildren[i];


			aiNodeAnim* anim = _getNodeAnim(node->mName);
			glm::mat4 transform = glm::mat4(1.0);

			if(anim!=NULL) {
				for(int j = 0; j < anim->mNumScalingKeys; j++) {
					if(anim->mScalingKeys[j].mTime<time) {
						aiVector3D scale = anim->mScalingKeys[j].mValue;
						transform = glm::scale(transform,glm::vec3(scale[0],scale[1],scale[2]));
						break;
					}
				}

				for(int j = 0; j < anim->mNumRotationKeys; j++) {
					if(anim->mRotationKeys[j].mTime<time) {
						aiQuaternion rotation = anim->mRotationKeys[j].mValue;
						transform = glm::mat4_cast(_quatCast(rotation))*transform;
						break;
					}
				}

				for(int j = 0; j < anim->mNumPositionKeys; j++) {
					if(anim->mPositionKeys[j].mTime<time) {
						aiVector3D position = anim->mPositionKeys[j].mValue;
						transform = glm::translate(transform,glm::vec3(position[0],position[1],position[2]));
						break;
					}
				}
			}

			int boneIndex = _boneNameToIndex[string(node->mName.C_Str())];
			transform = _globalInverseTransform*parent_transform*transform*_mat4Cast(_bones[boneIndex]->mOffsetMatrix);
			_transforms[boneIndex] = transform;
			_fillChildrenTransforms(node,time);
		}
	}

	aiNodeAnim* _getNodeAnim(aiString name) {
		for(int i = 0; i < _animation->mNumChannels; i++) {
			if(name == _animation->mChannels[i]->mNodeName) {
				return _animation->mChannels[i];
			}
		}
		return NULL;
	}
};
