#version 400


layout (location=0) in vec3 vertex_normal;
layout (location=1) in vec2 vertex_tex_coords;
layout (location=2) in ivec4 bone_ids;
layout (location=3) in vec4 bone_weights;
layout (location=4) in vec3 vertex_point_1;
layout (location=5) in vec3 vertex_point_2;
layout (location=6) in vec3 vertex_point_3;

uniform mat4 M; // model matrix
uniform mat4 P;
uniform mat4 V;
uniform float MESH_1_SELECT;
uniform float MESH_2_SELECT;
uniform float MIX;
uniform float EXPLODE;

out vec3 normal_eye;
out vec3 position_eye;
out vec2 tex_coords;

void main () {
	// send normals to fragment shader
	vec3 normal_world = vec3(M*vec4(vertex_normal,0.0));
	normal_eye = vec3(V*vec4(normal_world,0.0));

	vec3 mesh1;
	if(MESH_1_SELECT==1) {
		mesh1 = vertex_point_1;
	} else if(MESH_1_SELECT == 2) {
		mesh1 = vertex_point_2;
	} else if(MESH_1_SELECT == 3) {
		mesh1 = vertex_point_3;
	}

	vec3 mesh2;
	if(MESH_2_SELECT==1) {
		mesh2 = vertex_point_1;
	} else if(MESH_2_SELECT == 2) {
		mesh2 = vertex_point_2;
	} else if(MESH_2_SELECT == 3) {
		mesh2 = vertex_point_3;
	}

	vec3 position_world = vec3(M*vec4(mix(mesh1,mesh2,MIX), 1.0));
	position_world = vec3(
			position_world.x+EXPLODE*normal_world.x*10,
			position_world.y+EXPLODE*normal_world.y*10,
			position_world.z+(EXPLODE*normal_world.z*10+2*EXPLODE*EXPLODE) );

	position_eye = vec3(V*vec4(position_world,1.0));
	tex_coords = vertex_tex_coords;
	gl_Position = P * vec4(position_eye, 1.0);
}
