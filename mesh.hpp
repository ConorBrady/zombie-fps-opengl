#include <GL/glew.h>

class Mesh {

private:
	GLuint _vao;
	int _polyCount;
	int _textureIndex;

public:
	Mesh(const char* filename);
	void draw(unsigned int shader);
};
