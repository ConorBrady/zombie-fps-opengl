#include "landscape.hpp"

#include "mesh.hpp"

#define GLM_FORCE_RADIANS

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

static Landscape* singleton;

Landscape* Landscape::getLandscape() {
	if(singleton == NULL) {
		singleton = new Landscape();
		singleton->_mesh = new Mesh("resources/ground.dae");
	}
	return singleton;
}

Bounds* Landscape::getBounds() {
	Bounds* b = (Bounds*)malloc(sizeof(Bounds));
	b->x1 = -49;
	b->x2 = 49;
	b->y1 = -49;
	b->y2 = 49;
	return b;
}

void Landscape::draw(unsigned int shader, float time) {
	int M_loc = glGetUniformLocation (shader, "M");

	glUniformMatrix4fv (M_loc, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0)));

	int M1S = glGetUniformLocation(shader,"MESH_1_SELECT");
	int MIX = glGetUniformLocation(shader,"MIX");
	glUniform1f(MIX,0);
	glUniform1f(M1S,1);
	_mesh->draw(shader);
}
