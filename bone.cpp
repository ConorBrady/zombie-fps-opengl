#include "bone.hpp"

#include <iostream>

#include "animation.hpp"

glm::mat4 _matCast(const aiMatrix4x4 &value)
{
	glm::mat4 result;
	result[0] = glm::vec4(value[0][0], value[1][0], value[2][0], value[3][0]);
	result[1] = glm::vec4(value[0][1], value[1][1], value[2][1], value[3][1]);
	result[2] = glm::vec4(value[0][2], value[1][2], value[2][2], value[3][2]);
	result[3] = glm::vec4(value[0][3], value[1][3], value[2][3], value[3][3]);

	return result;
}

Bone::Bone(aiBone* bone, int index) {
	_offsetMatrix = _matCast(bone->mOffsetMatrix);
	_name = std::string(std::string(bone->mName.C_Str()));
	_index = index;
}

void Bone::addChild(Bone* bone) {
	_children.push_back(bone);
}

std::string Bone::getName() {
	return _name;
}

std::vector<Bone*> Bone::getChildren() {
	return _children;
}

int Bone::getIndex() {
	return _index;
}

void Bone::addAnimation(std::string name, aiNodeAnim* anim, float duration) {

	_animations[name] = new Animation(anim,duration);
}

glm::mat4 Bone::getTransformForAnimationAtTime(std::string name, float time) {

	glm::mat4 transform = glm::mat4(1.0);
	if(!_animations.empty()) {
		transform = _animations[name]->getTransformAtTime(time);
	}
	return transform*_offsetMatrix;
}
