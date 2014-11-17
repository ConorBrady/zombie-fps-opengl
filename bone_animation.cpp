#include "bone_animation.hpp"

#define GLM_FORCE_RADIANS

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_interpolation.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <map>

using namespace std;
using namespace glm;

void BoneAnimation::_fillTransforms(Bone* bone, double time, glm::mat4 parent_transform) {
    if(bone!=NULL) {
        mat4 transform = parent_transform*bone->getTransformForAnimationAtTime("reach",time);
        _transforms[bone->getIndex()] = transform;

    	for(int i = 0; i < bone->getChildren().size(); i++) {
    	    _fillTransforms(bone->getChildren()[i],time,transform);
    	}
    }
}

Bone* BoneAnimation::_getBoneForName(string name) {
    if(_boneNameToIndex.find(name) != _boneNameToIndex.end()) {
        return _bones[_boneNameToIndex[name]];
    } else {
        return NULL;
    }
}

aiNodeAnim* getNodeAnim(string name, aiAnimation* animation) {
	for(int i = 0; i < animation->mNumChannels; i++) {
		if(name == string(animation->mChannels[i]->mNodeName.C_Str())) {
			return animation->mChannels[i];
		}
	}
	return NULL;
}

void BoneAnimation::_addChildren(aiNode* current) {

    Bone* parent = _getBoneForName(string(current->mName.C_Str()));
    for(int i = 0; i < current->mNumChildren; i++) {

        Bone* bone = _getBoneForName(string(current->mChildren[i]->mName.C_Str()));

        if(parent!=NULL){
            parent->addChild(bone);
        } else if(bone!=NULL) {
            _rootNodes.push_back(bone);
        }

    }

    for(int i = 0; i < current->mNumChildren; i++) {
        _addChildren(current->mChildren[i]);
    }
}

BoneAnimation::BoneAnimation(const aiScene* scene) {

    for(int i = 0; i < scene->mMeshes[0]->mNumBones; i++) {
        _bones.push_back(new Bone(scene->mMeshes[0]->mBones[i],i));

        _boneNameToIndex[_bones[i]->getName()] = i;
        _bones[i]->addAnimation("reach",getNodeAnim(_bones[i]->getName(),scene->mAnimations[0]),scene->mAnimations[0]->mDuration);
    }

    _addChildren(scene->mRootNode);
}

const int BoneAnimation::numBones() {
	return _bones.size();
}

const float * BoneAnimation::getBonesAtTime(double time) {
	_transforms = (glm::mat4*)malloc(sizeof(glm::mat4)*_bones.size());
    for(int i = 0; i < _rootNodes.size(); i++) {
        _fillTransforms(_rootNodes[i],time,mat4(1.0));
    }
	return glm::value_ptr(_transforms[0]);
}
