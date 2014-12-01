#version 400

in vec3 normal_eye; // normals from vertex shader
in vec3 position_eye;
in vec2 tex_coords;

uniform sampler2D uni_tex;
uniform mat4 V;
uniform vec3 light_position_world[5];
uniform mat3 light_properties[5];

// surface reflectance
vec3 Ks = vec3 (0.5, 0.5, 0.5);
vec3 Kd = vec3 (1.0, 1.0, 1.0);
vec3 Ka = vec3 (1.0, 1.0, 1.0);

float specular_exponent = 100.0; // specular 'power'

out vec4 fragment_colour; // final colour of surface

void main () {
	vec3 light = vec3(0,0,0);

	for(int i = 0; i < 5; i ++){

		vec3 Ls = light_properties[i][0];
		vec3 Ld = light_properties[i][1];
		vec3 La = light_properties[i][2];

		vec3 light_position_eye = vec3 (V * vec4 (light_position_world[i], 1.0));
		vec3 position_to_light_dir_eye = normalize (light_position_eye - position_eye);

		float light_reach = 15.0f;
		float distance_from_light_to_position = length(light_position_eye-position_eye);
		float light_intensity = max(1.0/pow(distance_from_light_to_position/light_reach,2.0),0.0);

		float dot_prod = dot (position_to_light_dir_eye, normalize(normal_eye));
		dot_prod = max (dot_prod, 0.0);
		vec3 Id = Ld * Kd * dot_prod * light_intensity; // final diffuse intensity

		vec3 Ia = La * Ka * light_intensity;

		vec3 reflection_of_light_off_position_eye = reflect (-position_to_light_dir_eye, normalize(normal_eye));
		vec3 position_to_viewer_dir_eye = normalize (-position_eye);

		float dot_prod_specular = dot (reflection_of_light_off_position_eye, position_to_viewer_dir_eye);
		dot_prod_specular = max (dot_prod_specular, 0.0);
		float specular_factor = pow (dot_prod_specular, specular_exponent);
		vec3 Is = Ls * Ks * specular_factor * light_intensity; // final specular intensity

		light += Is + Id + Ia;
	}

	vec4 col = texture(uni_tex,tex_coords);

	fragment_colour = col*vec4(light + vec3(0.0,0,0.2), 1.0)*sqrt(col.g);


}
