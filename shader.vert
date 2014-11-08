#version 400

layout (location=0) in vec3 vertex_point;
layout (location=1) in vec3 vertex_normal;
layout (location=2) in vec2 vertex_tex_coords;
layout (location=3) in ivec4 bone_ids;
layout (location=4) in vec4 bone_weights;

uniform mat4 M; // model matrix
uniform mat4 BONES[128];

out vec2 texture_coord;

void main () {
	// send normals to fragment shader
	mat4 B;
	B  = BONES[bone_ids[0]]*bone_weights[0];
	B += BONES[bone_ids[1]]*bone_weights[1];
	B += BONES[bone_ids[2]]*bone_weights[2];
	B += BONES[bone_ids[3]]*bone_weights[3];
	texture_coord = vertex_tex_coords;
	gl_Position = M * B * vec4 (vertex_point, 1.0);
}
