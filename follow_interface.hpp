
#ifndef FOLLOW_INTERFACE
#define FOLLOW_INTERFACE

#define GLM_FORCE_RADIANS

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

class ITrackable {
	// an object capable of being tracked in the 3D space
public:
	virtual glm::vec3 getLocation() = 0;
};

class IFollower {
	// an object interested in following 1 or more objects
public:
	virtual void addFollowable(ITrackable* followable) = 0;
};

#endif
