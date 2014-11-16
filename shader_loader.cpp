#include "shader_loader.hpp"

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

#include <GL/glew.h>

bool parse_file_into_str (const char* file_name, char* shader_str, int max_len) {

	FILE* file = fopen (file_name , "r");
	int current_len = 0;
	char line[2048];

	shader_str[0] = '\0'; /* reset string */
	if (!file) {
		fprintf (stderr, "ERROR: opening file for reading: %s\n", file_name);
		return false;
	}
	strcpy (line, ""); /* remember to clean up before using for first time! */
	while (!feof (file)) {
		if (NULL != fgets (line, 2048, file)) {
			current_len += strlen (line); /* +1 for \n at end */
			if (current_len >= max_len) {
				fprintf (stderr,
					"ERROR: shader length is longer than string buffer length %i\n",
					max_len
				);
			}
			strcat (shader_str, line);
		}
	}
	if (EOF == fclose (file)) { /* probably unnecesssary validation */
		fprintf (stderr, "ERROR: closing file from reading %s\n", file_name);
		return false;
	}
	return true;
}


Shader::Shader(const char* vertex_shader, const char* fragment_shader) {

	char* vertex_shader_str;
	char* fragment_shader_str;

	// allocate some memory to store shader strings
	vertex_shader_str = (char*)malloc (81920);
	fragment_shader_str = (char*)malloc (81920);
	// load shader strings from text files
	assert (parse_file_into_str (vertex_shader, vertex_shader_str, 81920));
	assert (parse_file_into_str (fragment_shader, fragment_shader_str, 81920));
	GLuint vs, fs;
	vs = glCreateShader (GL_VERTEX_SHADER);
	fs = glCreateShader (GL_FRAGMENT_SHADER);
	glShaderSource (vs, 1, (const char**)&vertex_shader_str, NULL);
	glShaderSource (fs, 1, (const char**)&fragment_shader_str, NULL);
	// free memory
	free (vertex_shader_str);
	free (fragment_shader_str);
	glCompileShader (vs);
	GLint isCompiled = 0;

	GLuint shader;
	shader = vs;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
	if(isCompiled == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

		//The maxLength includes the NULL character
		char errorLog[maxLength];
		glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);

		//Provide the infolog in whatever manor you deem best.
		printf("vs: %s\n", errorLog);
		//Exit with failure.
		glDeleteShader(shader); //Don't leak the shader.
	}

	glCompileShader (fs);

	shader = fs;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
	if(isCompiled == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

		//The maxLength includes the NULL character
		char errorLog[maxLength];
		glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);

		//Provide the infolog in whatever manor you deem best.
		printf("fs: %s\n", errorLog);
		//Exit with failure.
		glDeleteShader(shader); //Don't leak the shader.
	}

	GLuint shader_programme = glCreateProgram ();
	glAttachShader (shader_programme, fs);
	glAttachShader (shader_programme, vs);
	glLinkProgram (shader_programme);

	glUseProgram (shader_programme);

	_shaderId = shader_programme;
}
