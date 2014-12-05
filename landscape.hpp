class Mesh;
typedef struct {
	float x1;
	float x2;
	float y1;
	float y2;
} Bounds;
class Landscape {
private:
	Mesh* _mesh;

public:
	static Landscape* getLandscape();
	Bounds* getBounds();
	void draw(unsigned int shader, float time);
};
