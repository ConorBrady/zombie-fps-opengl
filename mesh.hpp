#include <GL/glew.h>

class Mesh {
	// The foundation of the game this class represents the interface between
	// CPP code and the OpenGL GPU calls, this is used by the gun, landscape,
	// bullet and zombie for loading and rendering their meshes
private:
	GLuint _vao;
	int _polyCount;
	int _textureIndex;

public:
	Mesh(const char* filename); // load the mesh with a filename
	void draw(unsigned int shader);
};
