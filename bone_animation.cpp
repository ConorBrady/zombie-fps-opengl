#include "bone_animation.hpp"

#define GLM_FORCE_RADIANS

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_interpolation.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <map>

using namespace std;

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

void BoneAnimation::_fillChildrenTransforms(aiNode* parent, double time, glm::mat4 parent_transform) {

	for(int i = 0; i < parent->mNumChildren; i++) {

		aiNode* node = parent->mChildren[i];

		aiNodeAnim* anim = _getNodeAnim(node->mName);
		glm::mat4 transform = glm::mat4(1.0);

		if(anim!=NULL) {
			for(int j = anim->mNumScalingKeys-1; j >= 0; j--) {
				if(anim->mScalingKeys[j].mTime<time) {
					aiVector3D scale;
					scale = anim->mScalingKeys[j].mValue;
					glm::mat4 scale_a = glm::scale(glm::mat4(1.0),glm::vec3(scale.x,scale.y,scale.z));
					scale = anim->mScalingKeys[j+1].mValue;
					glm::mat4 scale_b = glm::scale(glm::mat4(1.0),glm::vec3(scale.x,scale.y,scale.z));
					float interpolation_factor = (time-anim->mScalingKeys[j].mTime)/
										(anim->mScalingKeys[j+1].mTime-anim->mScalingKeys[j].mTime);
					transform = glm::interpolate(scale_a,scale_b,interpolation_factor) * transform;
					break;
				}
			}

			for(int j = anim->mNumRotationKeys-1; j >= 0; j--) {
				if(anim->mRotationKeys[j].mTime<=time) {
					glm::quat rotation_a = _quatCast(anim->mRotationKeys[j].mValue);
					glm::quat rotation_b = _quatCast(anim->mRotationKeys[j+1].mValue);
					float interpolation_factor = (time-anim->mRotationKeys[j].mTime)/
										(anim->mRotationKeys[j+1].mTime-anim->mRotationKeys[j].mTime);
					transform = glm::mat4_cast(glm::slerp(rotation_a,rotation_b,interpolation_factor)) * transform;
					break;
				}
			}

			for(int j = anim->mNumPositionKeys-1; j >= 0; j--) {
				if(anim->mPositionKeys[j].mTime<time) {
					aiVector3D position;
					position = anim->mPositionKeys[j].mValue;
					glm::mat4 position_a = glm::translate(glm::mat4(1.0),glm::vec3(position.x,position.y,position.z));
					position = anim->mPositionKeys[j+1].mValue;
					glm::mat4 position_b = glm::translate(glm::mat4(1.0),glm::vec3(position.x,position.y,position.z));
					float interpolation_factor = (time-anim->mPositionKeys[j].mTime)/
										(anim->mPositionKeys[j+1].mTime-anim->mRotationKeys[j].mTime);
					transform = glm::interpolate(position_a,position_b,interpolation_factor) * transform;
					break;
				}
			}

		}

		glm::mat4 offset = glm::mat4(1.0);

		if(_boneNameToIndex.find(string(node->mName.C_Str())) != _boneNameToIndex.end()) {
			int boneIndex = _boneNameToIndex[string(node->mName.C_Str())];
			glm::mat4 offset = _mat4Cast(_bones[boneIndex]->mOffsetMatrix);

			transform = parent_transform*glm::inverse(offset)*transform*offset;

			_transforms[boneIndex] = transform;
		}

		_fillChildrenTransforms(node,time,transform);
	}
}

aiNodeAnim* BoneAnimation::_getNodeAnim(aiString name) {
	for(int i = 0; i < _animation->mNumChannels; i++) {
		if(name == _animation->mChannels[i]->mNodeName) {
			return _animation->mChannels[i];
		}
	}
	return NULL;
}

BoneAnimation::BoneAnimation(aiBone ** bones, int numBones, aiAnimation * animation, aiNode * rootNode) {

	_bones = bones;
	_animation = animation;
	_rootNode = rootNode;
	_numBones = numBones;
	_globalInverseTransform = _mat4Cast(rootNode->mTransformation.Inverse());


	for(int i = 0; i < numBones; i++) {
		_boneNameToIndex[string(bones[i]->mName.C_Str())] = i;
	}
}

const int BoneAnimation::numBones() {
	return _numBones;
}

const float * BoneAnimation::getBonesAtTime(double time) {
	free(_transforms);
	_transforms = (glm::mat4*)malloc(sizeof(glm::mat4)*_numBones);
	_fillChildrenTransforms(_rootNode,fmod(time,_animation->mDuration),glm::mat4(1.0));
	return glm::value_ptr(_transforms[0]);
}
