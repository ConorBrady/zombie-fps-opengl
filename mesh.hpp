#include <GL/glew.h>

#include "bone_animation.hpp"

class Mesh {

private:
	GLuint _vao;
	int _polyCount, _textureIndex;
	BoneAnimation *_boneAnimation;

public:
	Mesh(const char* filename);
	void draw(GLuint shader, double time);
};
