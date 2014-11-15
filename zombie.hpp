#include "mesh.hpp"

class Zombie {

private:
	double _x, _y;

public:
	Zombie(double x, double y);
	void draw(GLuint shader, double time);
};
