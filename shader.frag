#version 400

in vec3 normal_eye; // normals from vertex shader
in vec3 position_eye;
in vec2 tex_coords;

uniform sampler2D uni_tex;
uniform mat4 V;

// fixed point light properties
vec3 light_position_world = vec3 (0.0, -20.0, 20.0);
vec3 Ls = vec3 (1.0, 0.8, 0.8);
vec3 Ld = vec3 (1.0, 0.6, 0.6);
vec3 La = vec3 (0.2, 0.1, 0.1);

// surface reflectance
vec3 Ks = vec3 (1.0, 1.0, 1.0);
vec3 Kd = vec3 (1.0, 1.0, 1.0);
vec3 Ka = vec3 (1.0, 1.0, 1.0);

float specular_exponent = 100.0; // specular 'power'

out vec4 fragment_colour; // final colour of surface

void main () {

	vec3 light_position_eye = vec3 (V * vec4 (light_position_world, 1.0));
	vec3 position_to_light_dir_eye = normalize (light_position_eye - position_eye);

	float dot_prod = dot (position_to_light_dir_eye, normalize(normal_eye));
	dot_prod = max (dot_prod, 0.0);
	vec3 Id = Ld * Kd * dot_prod; // final diffuse intensity

	vec3 Ia = La * Ka;

	vec3 reflection_of_light_off_position_eye = reflect (-position_to_light_dir_eye, normalize(normal_eye));
	vec3 position_to_viewer_dir_eye = normalize (-position_eye);

	float dot_prod_specular = dot (reflection_of_light_off_position_eye, position_to_viewer_dir_eye);
	dot_prod_specular = max (dot_prod_specular, 0.0);
	float specular_factor = pow (dot_prod_specular, specular_exponent);
	vec3 Is = Ls * Ks * specular_factor; // final specular intensity

	fragment_colour = texture(uni_tex,tex_coords)*vec4 (Is + Id + Ia, 1.0);
}
