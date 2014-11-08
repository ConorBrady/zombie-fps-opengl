#version 400

in vec2 texture_coord; // normals from vertex shader

uniform sampler2D uni_tex;

out vec4 frag_colour;

void main () {
	frag_colour = texture(uni_tex,texture_coord);
}
