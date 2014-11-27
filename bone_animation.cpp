#include "bone_animation.hpp"

#define GLM_FORCE_RADIANS

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_interpolation.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <map>


glm::mat4 _mat4Cast(const aiMatrix4x4 &value)
{
    glm::mat4 result;
    result[0] = glm::vec4(value[0][0], value[1][0], value[2][0], value[3][0]);
    result[1] = glm::vec4(value[0][1], value[1][1], value[2][1], value[3][1]);
    result[2] = glm::vec4(value[0][2], value[1][2], value[2][2], value[3][2]);
    result[3] = glm::vec4(value[0][3], value[1][3], value[2][3], value[3][3]);

    return result;
}


Bone* _getBoneForName(std::string name, std::vector<Bone*> bones) {
    for(int i = 0; i < bones.size(); i++) {
        if(bones[i]->getName() == name) {
            return bones[i];
        }
    }
    return NULL;
}

aiNodeAnim* getNodeAnim(std::string name, aiAnimation* animation) {
	for(int i = 0; i < animation->mNumChannels; i++) {
		if(name == std::string(animation->mChannels[i]->mNodeName.C_Str())) {
			return animation->mChannels[i];
		}
	}
	return NULL;
}


void BoneAnimation::_fillTransforms(Bone* bone, double time, glm::mat4 parent_transform) {
    if(bone!=NULL) {
        glm::mat4 transform = _globalInverseTransform*parent_transform*bone->getTransformForAnimationAtTime("reach",time);
        _transforms[bone->getIndex()] = transform;

        for(int i = 0; i < bone->getChildren().size(); i++) {
            _fillTransforms(bone->getChildren()[i],time,transform);
        }
    }
}

void BoneAnimation::_addChildren(aiNode* current) {

    Bone* parent = _getBoneForName(std::string(current->mName.C_Str()),_bones);
    for(int i = 0; i < current->mNumChildren; i++) {

        Bone* bone = _getBoneForName(std::string(current->mChildren[i]->mName.C_Str()),_bones);
        if(parent!=NULL && bone != NULL){
            parent->addChild(bone);
        } else if(bone!=NULL) {
            _rootNodes.push_back(bone->getIndex());
        }

    }

    for(int i = 0; i < current->mNumChildren; i++) {
        _addChildren(current->mChildren[i]);
    }
}

BoneAnimation::BoneAnimation(const aiScene* scene) {

    for(int i = 0; i < scene->mMeshes[0]->mNumBones; i++) {
        _bones.push_back(new Bone(scene->mMeshes[0]->mBones[i],i));
        _bones[i]->addAnimation("reach",getNodeAnim(_bones[i]->getName(),scene->mAnimations[0]),scene->mAnimations[0]->mDuration);
        _globalInverseTransform = glm::inverse(_mat4Cast(scene->mRootNode->mTransformation));
    }

    _addChildren(scene->mRootNode);
}

const int BoneAnimation::numBones() {
	return _bones.size();
}

const float * BoneAnimation::getBonesAtTime(double time) {

	_transforms = (glm::mat4*)malloc(sizeof(glm::mat4)*_bones.size());
    for(int i = 0; i < _rootNodes.size(); i++) {
        _fillTransforms(_bones[_rootNodes[i]],time,glm::mat4(1.0));
    }
	return glm::value_ptr(_transforms[0]);
}
