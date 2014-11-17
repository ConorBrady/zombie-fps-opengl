#include <vector>
#include <assimp/scene.h>

#define GLM_FORCE_RADIANS

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_interpolation.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;
using namespace glm;

class Animation {
private:
	float _duration;
	vector<aiVectorKey> _positionKeys;
	vector<aiQuatKey> _rotationKeys;
	vector<aiVectorKey> _scalingKeys;
public:
	Animation(aiNodeAnim* animation, float _duration);
	mat4 getTransformAtTime(float time);
};
