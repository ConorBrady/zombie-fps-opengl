class Mesh;

class Landscape {
private:
	Mesh* _mesh;

public:
	static Landscape* getLandscape();
	void draw(unsigned int shader, float time);
};
