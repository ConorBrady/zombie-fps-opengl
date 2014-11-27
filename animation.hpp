#include <vector>
#include <assimp/scene.h>

#define GLM_FORCE_RADIANS

// Encapsulation of an interpolatable animation of a set of key framed matrices.
// Allows for easy access to an interpolated transform matrix given a time in
// the animation.

class Animation {
private:

	// The duration of the animation
	float _duration;

	// The position keys of the animation
	std::vector<aiVectorKey> _positionKeys;

	// The rotation keys of the animation
	std::vector<aiQuatKey> _rotationKeys;

	// The scaling keys of the animation
	std::vector<aiVectorKey> _scalingKeys;
public:

	Animation(aiNodeAnim* animation, float _duration);
	glm::mat4 getTransformAtTime(float time);
};
