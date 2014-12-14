class Shader {
	// class for loading shaders
private:
	unsigned int _shaderId;

public:
	Shader(const char* vertex_shader, const char* fragment_shader);

	operator int() { return _shaderId; }
};
