#include "animation.hpp"

#include <iostream>

using namespace glm;

quat _quatCast(const aiQuaternion &value)
{
	return quat(value.w, value.x, value.y, value.z);
}

Animation::Animation(aiNodeAnim* animation, float duration) {
	_duration = duration;
	_positionKeys.insert(_positionKeys.end(),animation->mPositionKeys,&animation->mPositionKeys[animation->mNumPositionKeys-1]);
	_rotationKeys.insert(_rotationKeys.end(),animation->mRotationKeys,&animation->mRotationKeys[animation->mNumRotationKeys-1]);
	_scalingKeys.insert(_scalingKeys.end(),animation->mScalingKeys,&animation->mScalingKeys[animation->mNumScalingKeys-1]);
}

mat4 Animation::getTransformAtTime(float time) {
	time = fmod(time,_duration);

	glm::mat4 transform = glm::mat4(1.0);

	for(int j = _scalingKeys.size()-1; j >= 0; j--) {
		if(_scalingKeys[j].mTime<time) {
			aiVector3D scale_v;
			scale_v = _scalingKeys[j].mValue;
			mat4 scale_a = scale(mat4(1.0),vec3(scale_v.x,scale_v.y,scale_v.z));
			scale_v = _scalingKeys[j+1].mValue;
			mat4 scale_b = scale(mat4(1.0),vec3(scale_v.x,scale_v.y,scale_v.z));
			float interpolation_factor = (time-_scalingKeys[j].mTime)/
								(_scalingKeys[j+1].mTime-_scalingKeys[j].mTime);
			transform = interpolate(scale_a,scale_b,interpolation_factor) * transform;
			break;
		}
	}

	for(int j = _rotationKeys.size()-1; j >= 0; j--) {
		if(_rotationKeys[j].mTime<=time) {
			glm::quat rotation_a = _quatCast(_rotationKeys[j].mValue);
			glm::quat rotation_b = _quatCast(_rotationKeys[j+1].mValue);
			float interpolation_factor = (time-_rotationKeys[j].mTime)/
								(_rotationKeys[j+1].mTime-_rotationKeys[j].mTime);
			transform = mat4_cast(slerp(rotation_a,rotation_b,interpolation_factor)) * transform;
			break;
		}
	}

	for(int j = _positionKeys.size()-1; j >= 0; j--) {
		if(_positionKeys[j].mTime<time) {
			aiVector3D position;
			position = _positionKeys[j].mValue;
			glm::mat4 position_a = translate(mat4(1.0),vec3(position.x,position.y,position.z));
			position = _positionKeys[j+1].mValue;
			glm::mat4 position_b = translate(mat4(1.0),vec3(position.x,position.y,position.z));
			float interpolation_factor = (time-_positionKeys[j].mTime)/
								(_positionKeys[j+1].mTime-_rotationKeys[j].mTime);
			transform = interpolate(position_a,position_b,interpolation_factor) * transform;
			break;
		}
	}
	return transform;
}
