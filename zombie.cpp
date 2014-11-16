#include "zombie.hpp"

#include "mesh.hpp"

#include <iostream>

#define GLM_FORCE_RADIANS

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

static Mesh* _mesh = NULL;

Zombie::Zombie(double x, double y) {

	if(_mesh==NULL) {
		_mesh = new Mesh("resources/zombie.dae");
	}
	_x = x;
	_y = y;
}

void Zombie::draw(unsigned int shader, float time) {
	int M_loc = glGetUniformLocation (shader, "M");
	glm::mat4 M = glm::translate(glm::mat4(1.0),glm::vec3(_x,_y,0));
	glUniformMatrix4fv (M_loc, 1, GL_FALSE, glm::value_ptr(M));
	_mesh->draw(shader, time);
}
