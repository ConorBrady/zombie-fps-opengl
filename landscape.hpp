
#ifndef LANDSCAPE
#define LANDSCAPE
class Mesh;
typedef struct {
	float x1;
	float x2;
	float y1;
	float y2;
} Bounds;
class Landscape {
	// the game level, not much going on here as it is a box
private:
	Mesh* _mesh = nullptr;

public:
	static Landscape* getLandscape();
	Bounds* getBounds();
	void draw(unsigned int shader, float time);
};
#endif
