#version 400

layout (location=0) in vec3 vertex_point;
layout (location=1) in vec3 vertex_normal;
layout (location=2) in vec2 vertex_tex_coords;
layout (location=3) in ivec4 bone_ids;
layout (location=4) in vec4 bone_weights;

uniform mat4 M; // model matrix
uniform mat4 P;
uniform mat4 V;
uniform mat4 BONES[128];

out vec3 normal_eye;
out vec3 position_eye;
out vec2 tex_coords;

void main () {
	// send normals to fragment shader
	mat4 BoneTransform = mat4(1.0);
	if(bone_weights[0]+bone_weights[1]+bone_weights[2]+bone_weights[3]>0.1) {
		BoneTransform  = BONES[bone_ids[0]]*bone_weights[0];
		BoneTransform += BONES[bone_ids[1]]*bone_weights[1];
		BoneTransform += BONES[bone_ids[2]]*bone_weights[2];
		BoneTransform += BONES[bone_ids[3]]*bone_weights[3];
	}

	normal_eye   = vec3(V*M*BoneTransform*vec4(vertex_normal,0.0));
	// normal_eye   = vec3(V*M*vec4(vertex_normal,0.0));
	position_eye = vec3(V*M*BoneTransform*vec4(vertex_point, 1.0));
	// position_eye = vec3(V*M*vec4(vertex_point, 1.0));
	tex_coords = vertex_tex_coords;
	gl_Position = P * vec4(position_eye, 1.0);
}
