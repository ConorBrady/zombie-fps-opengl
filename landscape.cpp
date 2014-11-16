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

void Landscape::draw(unsigned int shader, float time) {
	int M_loc = glGetUniformLocation (shader, "M");
	glUniformMatrix4fv (M_loc, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0)));
	_mesh->draw(shader, time);
}
