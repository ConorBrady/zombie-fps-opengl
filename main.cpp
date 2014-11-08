/*
_Optional_ Starter Code for Virtual Camera lab

You don't need to use this starter if you are confident in writing your own!

Things to do:
1. find a maths library to help you write matrices and multiply matrices
together (you could also write your own)
2. add a view and projection matrix to the shader and transformation pipeline
3. try creating a smaller viewport (1/4 size)
4. think about creating additional viewports and drawing scenes for each
5. use glfwGetTime() to help with animation, or alternatively the gflw manual
for keyboard controls to move either a camera or an object
* remember to multiply matrices together to combine them
* some people like to combine Model and View matrices together into a single MV
matrix, or even Model View and Projection into MVP. I prefer to keep them
separate. Up to you.

Anton Gerdelan 7 Oct 2014
*/

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

#define FPS 25.0

#include <IL/il.h>

#include "BoneAnimation.cpp"

using namespace std;
//
// dimensions of the window drawing surface
int gl_width = 800;
int gl_height = 800;

//
// copy a shader from a plain text file into a character array
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



void load_mesh(GLuint &vao, const aiMesh* mesh) {

	aiMesh aMesh;
	aiMaterial aMat;
	GLuint buffer;

	// create array with faces
	// have to convert from Assimp format to array
	unsigned int *faceArray;
	faceArray = (unsigned int *)malloc(sizeof(unsigned int) * mesh->mNumFaces * 3);
	unsigned int faceIndex = 0;

	for (unsigned int t = 0; t < mesh->mNumFaces; ++t) {
		const aiFace* face = &mesh->mFaces[t];

		memcpy(&faceArray[faceIndex], face->mIndices,3 * sizeof(unsigned int));
		faceIndex += 3;
	}

	// generate Vertex Array for mesh
	glGenVertexArrays(1,&vao);
	glBindVertexArray(vao);

	// buffer for faces
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * mesh->mNumFaces * 3, faceArray, GL_STATIC_DRAW);

	// buffer for vertex positions
	if (mesh->HasPositions()) {
		glGenBuffers(1, &buffer);
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float)*3*mesh->mNumVertices, mesh->mVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, 0, 0, 0);
	}

	// buffer for vertex normals
	if (mesh->HasNormals()) {
		glGenBuffers(1, &buffer);
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float)*3*mesh->mNumVertices, mesh->mNormals, GL_STATIC_DRAW);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, 0, 0, 0);
	}

	// buffer for vertex texture coordinates
	if (mesh->HasTextureCoords(0)) {
		float *texCoords = (float *)malloc(sizeof(float)*2*mesh->mNumVertices);
		for (int k = 0; k < mesh->mNumVertices; k++) {

			texCoords[k*2]   = mesh->mTextureCoords[0][k].x;
			texCoords[k*2+1] = mesh->mTextureCoords[0][k].y;

		}
		glGenBuffers(1, &buffer);
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float)*2*mesh->mNumVertices, texCoords, GL_STATIC_DRAW);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, 0, 0, 0);
	}

	// load bone influence

	typedef struct {
		int bone_ids[4];
		float weights[4];
	} BoneInfluence;

	BoneInfluence* bone_influence = (BoneInfluence*)calloc(mesh->mNumVertices,sizeof(BoneInfluence));

	for(int i = 0; i < mesh->mNumBones; i++) {
		aiBone* bone = mesh->mBones[i];
		for(int j = 0; j < bone->mNumWeights; j++) {
			aiVertexWeight vw = bone->mWeights[j];
			int k = 0;
			while( bone_influence[vw.mVertexId].weights[k] != 0 && k < 4 ){
				k++;
			}
			if(k!=4) {
				bone_influence[vw.mVertexId].weights[k] = vw.mWeight;
				bone_influence[vw.mVertexId].bone_ids[k] = i;
			} else {
				printf("Bone overflow occured");
			}
		}
	}

    glEnableVertexAttribArray(3);
    glVertexAttribIPointer(3, 4, GL_INT, sizeof(BoneInfluence), (const GLvoid*)0);
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(BoneInfluence), (const GLvoid*)16);

	// unbind buffers
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER,0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);

}

GLuint load_shader_programme(const char* vertex_shader, const char* fragment_shader) {

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
		return -1;
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
		return -1;
	}

	GLuint shader_programme = glCreateProgram ();
	glAttachShader (shader_programme, fs);
	glAttachShader (shader_programme, vs);
	glLinkProgram (shader_programme);
	return shader_programme;
}

GLFWwindow* window_init() {
	GLFWwindow* window = NULL;
	const GLubyte* renderer;
	const GLubyte* version;
	if (!glfwInit ()) {
		fprintf (stderr, "ERROR: could not start GLFW3\n");
		return window;
	}

	glfwWindowHint (GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint (GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint (GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow (gl_width, gl_height, "Hello Triangle", NULL, NULL);
	if (!window) {
		fprintf (stderr, "ERROR: opening OS window\n");
		return window;
	}
	glfwMakeContextCurrent (window);

	glewExperimental = GL_TRUE;
	glewInit ();

	renderer = glGetString (GL_RENDERER); /* get renderer string */
	version = glGetString (GL_VERSION); /* version as a string */
	printf ("Renderer: %s\n", renderer);
	printf ("OpenGL version supported %s\n", version);
	glEnable (GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc (GL_LESS); // depth-testing interprets a smaller value as "closer"
	glClearColor (0.5, 0.5, 0.5, 1.0);

	GLuint texture_buffer;
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &texture_buffer);
	glBindTexture(GL_TEXTURE_2D, texture_buffer);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	return window;
}

int main () {

	GLuint shader_programme;
	GLuint vao;

	GLFWwindow* window = window_init();
	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile("resources/newguy.md5mesh", aiProcess_Triangulate);
	if(!scene){
		fprintf(stderr, "%s", importer.GetErrorString());
		return 0;
	}
	const aiMesh* mesh = scene->mMeshes[0];

	load_mesh(vao,mesh);

	shader_programme = load_shader_programme("shader.vert","shader.frag");

	int M_loc = glGetUniformLocation (shader_programme, "M");

	unsigned int imageID;

	ilInit();
	ilGenImages(1, &imageID);
	ilBindImage(imageID);
	ilEnable(IL_ORIGIN_SET);
	ilOriginFunc(IL_ORIGIN_LOWER_LEFT);

	aiString path;
	scene->mMaterials[mesh->mMaterialIndex]->GetTexture(aiTextureType_DIFFUSE, 0, &path);

	ilLoadImage((ILstring)path.C_Str());
	ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT),0, GL_RGBA, GL_UNSIGNED_BYTE, ilGetData());

	double last_tick = glfwGetTime();

	BoneAnimation bone_animation(mesh->mBones,mesh->mNumBones,scene->mAnimations[0],scene->mRootNode);
	int bones_loc = glGetUniformLocation (shader_programme, "BONES");

	while (!glfwWindowShouldClose (window)) {

		glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram (shader_programme);

		glm::mat4 V,P,M;
		M = glm::scale(glm::mat4(1.0),glm::vec3(0.5,0.5,0.5));
		M = glm::rotate(M,(float)M_PI/2.0f,glm::vec3(-1,0,0));

		V = glm::lookAt(glm::vec3(0.0,5.0,10.0),glm::vec3(0,2,0),glm::vec3(0,1,0));
		P = glm::ortho(-30.0f,30.0f,-30.0f,30.0f,0.1f,100.0f);

		glUniformMatrix4fv (M_loc, 1, GL_FALSE, glm::value_ptr(P*V*M));
		glUniformMatrix4fv (bones_loc, mesh->mNumBones, GL_FALSE, bone_animation.getBonesAtTime(last_tick));
		glBindVertexArray (vao);

		glDrawArrays( GL_TRIANGLES, 0, mesh->mNumVertices );

		/* this just updates window events and keyboard input events (not used yet) */
		glfwPollEvents ();
		glfwSwapBuffers (window);

		while(glfwGetTime()<last_tick+1/FPS);
		last_tick = glfwGetTime();
	}

	return 0;
}
