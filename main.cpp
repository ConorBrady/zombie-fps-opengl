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
#include <vector>

#define FPS 25.0

#define STRAFE_LEFT -1
#define STRAFE_RIGHT 1
#define NO_STRAFE 0

#include <IL/il.h>

#include "BoneAnimation.cpp"

using namespace std;
//
// dimensions of the window drawing surface
int gl_width = 800;
int gl_height = 800;

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
static void cursorPosCallback(GLFWwindow * window, double x, double y);


glm::vec3 camera_xyz = glm::vec3(0,-5,5);
float camera_pitch = M_PI/4;
float camera_yaw = 0;

double cursor_x = gl_width/2;
double cursor_y = gl_height/2;

double stride_factor = 0;
double strafe_factor = NO_STRAFE;
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



void load_meshes(GLuint &vao, vector<const aiMesh*> meshes) {

	GLuint buffer;

	// generate Vertex Array for mesh
	glGenVertexArrays(1,&vao);
	glBindVertexArray(vao);

	int combined_size = 0;
	for (int i = 0; i < meshes.size(); i++) {
		combined_size += meshes[i]->mNumVertices;
	}


	{
		float* vertices = (float*)malloc(sizeof(float)*combined_size*3);
		int offset = 0;
		for(int i = 0; i < meshes.size(); i++) {
			int count = meshes[i]->mNumVertices*3;
			memcpy(&vertices[offset],meshes[i]->mVertices,count*sizeof(float));
			offset += count;
		}

		glGenBuffers(1, &buffer);
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float)*3*combined_size, vertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, 0, 0, 0);
	}

	{

		float* normals = (float*)malloc(sizeof(float)*combined_size*3);
		int offset = 0;
		for(int i = 0; i < meshes.size(); i++) {
			int count = meshes[i]->mNumVertices*3;
			memcpy(&normals[offset],meshes[i]->mNormals,count*sizeof(float));
			offset += count;
		}

		glGenBuffers(1, &buffer);
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float)*3*combined_size, normals, GL_STATIC_DRAW);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, 0, 0, 0);
	}

	{
		float *texCoords = (float*)malloc(sizeof(float)*combined_size*2);
		int offset = 0;
		for (int k = 0; k < meshes.size(); k++) {
			for(int i = 0; i < meshes[k]->mNumVertices; i++) {
				texCoords[offset+i*2]   = meshes[k]->mTextureCoords[0][i].x;
				texCoords[offset+i*2+1] = meshes[k]->mTextureCoords[0][i].y;
			}
			offset += meshes[k]->mNumVertices*2;
		}

		glGenBuffers(1, &buffer);
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float)*2*combined_size, texCoords, GL_STATIC_DRAW);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, 0, 0, 0);
	}

	// load bone influence

	typedef struct {
		int bone_ids[4];
		float weights[4];
	} BoneInfluence;

	BoneInfluence* bone_influence = (BoneInfluence*)calloc(combined_size,sizeof(BoneInfluence));
	for(int h = 0; h < meshes.size(); h++) {
		for(int i = 0; i < meshes[h]->mNumBones; i++) {
			aiBone* bone = meshes[h]->mBones[i];
			for(int j = 0; j < bone->mNumWeights; j++) {
				aiVertexWeight vw = bone->mWeights[j];
				int k = 0;
				while( bone_influence[vw.mVertexId].weights[k] != 0 && k < 4 ){
					k++;
				}
				if(k!=4) {
					bone_influence[vw.mVertexId].weights[k] = vw.mWeight;
					bone_influence[vw.mVertexId].bone_ids[k] = i;
				}
			}
		}
	}

	glGenBuffers(1,&buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(BoneInfluence)*combined_size,bone_influence,GL_STATIC_DRAW);
    glEnableVertexAttribArray(3);
    glVertexAttribIPointer(3, 4, GL_INT, sizeof(BoneInfluence), (const GLvoid*)0);
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(BoneInfluence), (const GLvoid*)16);

	// unbind buffers
	// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);

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
	glClearColor (0.0, 0.0, 0.0, 1.0);

	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, cursorPosCallback);

	return window;
}

void updatePitchYaw(GLFWwindow* window) {
	const static float MAX_ROTATION_PER_UPDATE = 0.2;
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	float dyaw = ((cursor_x-width/2)/width);
	if(dyaw > 0.2) {
		camera_yaw += (dyaw-0.2)*1.25*MAX_ROTATION_PER_UPDATE;
	}
	if(dyaw < -0.2) {
		camera_yaw += (dyaw+0.2)*1.25*MAX_ROTATION_PER_UPDATE;
	}

	float dpitch = (((height-cursor_y)-height/2)/height);
	if(dpitch > 0.2){
		camera_pitch += (dpitch-0.2)*1.25*MAX_ROTATION_PER_UPDATE;
	}
	if(dpitch < -0.2) {
		camera_pitch += (dpitch+0.2)*1.25*MAX_ROTATION_PER_UPDATE;
	}

	if(camera_pitch>M_PI) {
		camera_pitch=M_PI;
	}
	if(camera_pitch<0) {
		camera_pitch = 0;
	}
}

void updateStrideStrafe() {
	const static float SPEED = 0.5;
	camera_xyz.z -= stride_factor*cos(camera_pitch);
	camera_xyz.x += stride_factor*sin(camera_pitch)*sin(camera_yaw);
	camera_xyz.y += stride_factor*sin(camera_pitch)*cos(camera_yaw);

	camera_xyz.x += abs(strafe_factor)*SPEED*sin(camera_yaw+strafe_factor*M_PI/2);
	camera_xyz.y += abs(strafe_factor)*SPEED*cos(camera_yaw+strafe_factor*M_PI/2);
}

int main () {

	GLuint shader_programme;
	GLuint vao;

	GLFWwindow* window = window_init();
	Assimp::Importer importer1;

	const aiScene* character = importer1.ReadFile("resources/character.dae", aiProcess_Triangulate);
	if(!character){
		fprintf(stderr, "%s", importer1.GetErrorString());
		return 0;
	}
	const aiMesh* ch_mesh = character->mMeshes[0];

	Assimp::Importer importer2;
	const aiScene* ground = importer2.ReadFile("resources/ground.dae", aiProcess_Triangulate | aiProcess_FlipUVs);
	if(!ground){
		fprintf(stderr, "%s", importer2.GetErrorString());
		return 0;
	}
	const aiMesh* gr_mesh = ground->mMeshes[0];

	vector<const aiMesh*> meshes;
	meshes.push_back(ch_mesh);
	meshes.push_back(gr_mesh);
	load_meshes(vao,meshes);

	shader_programme = load_shader_programme("shader.vert","shader.frag");

	unsigned int imageID;

	/**
		Load textures
	 */
	ilInit();
	ilGenImages(1, &imageID);
	ilBindImage(imageID);
	ilEnable(IL_ORIGIN_SET);
	ilOriginFunc(IL_ORIGIN_LOWER_LEFT);

	aiString path;
	if(character->mMaterials[ch_mesh->mMaterialIndex]->GetTexture(aiTextureType_DIFFUSE, 0, &path) != AI_SUCCESS) {
		cout << "Could not load texture filename" << endl;
	}

	if(!ilLoadImage(("resources/"+string(path.C_Str())).c_str())){
		cout << "Failed to load image" << endl;
	}
	ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);

	GLuint texture_buffer;
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &texture_buffer);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_buffer);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT),0, GL_RGBA, GL_UNSIGNED_BYTE, ilGetData());
	glGenerateMipmap(GL_TEXTURE_2D);

	double last_tick = glfwGetTime();

	BoneAnimation bone_animation(ch_mesh->mBones,ch_mesh->mNumBones,character->mAnimations[0],character->mRootNode);
	int bones_loc = glGetUniformLocation (shader_programme, "BONES");

	int M_loc = glGetUniformLocation (shader_programme, "M");
	int V_loc = glGetUniformLocation (shader_programme, "V");
	int P_loc = glGetUniformLocation (shader_programme, "P");

	glm::mat4 V,P,M;

	glUseProgram (shader_programme);

	GLuint texLoc = glGetUniformLocation(shader_programme, "uni_tex");
	glUniform1i(texLoc, 0);

	P = glm::perspective(70.0f,1.0f,0.1f,100.0f);
	glUniformMatrix4fv (P_loc, 1, GL_FALSE, glm::value_ptr(P));


	while (!glfwWindowShouldClose (window)) {

		glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		V = glm::rotate(glm::mat4(1.0),camera_pitch,glm::vec3(-1,0,0));
		V = glm::rotate(V,camera_yaw,glm::vec3(0,0,1));
		V = glm::translate(V,glm::vec3(-camera_xyz.x,-camera_xyz.y,-camera_xyz.z));
		glUniformMatrix4fv (V_loc, 1, GL_FALSE, glm::value_ptr(V));

		M = glm::scale(glm::mat4(1.0),glm::vec3(0.5,0.5,0.5));
		glUniformMatrix4fv (M_loc, 1, GL_FALSE, glm::value_ptr(M));
		glUniformMatrix4fv (bones_loc, ch_mesh->mNumBones, GL_FALSE, bone_animation.getBonesAtTime(last_tick));
		glDrawArrays( GL_TRIANGLES, 0, ch_mesh->mNumVertices );


		M = glm::scale(glm::mat4(1.0),glm::vec3(500,500,0));
		glUniformMatrix4fv(M_loc, 1, GL_FALSE, glm::value_ptr(M));
		glDrawArrays( GL_TRIANGLES, ch_mesh->mNumVertices, gr_mesh->mNumVertices);

		glfwPollEvents();
		updatePitchYaw(window);
		updateStrideStrafe();
		glfwSwapBuffers(window);

		while(glfwGetTime()<last_tick+1/FPS);
		last_tick = glfwGetTime();
	}

	return 0;
}

static void cursorPosCallback(GLFWwindow * window, double x, double y) {
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	if(x>0&&x<width) {
		cursor_x = x;
		if(y>0&&y<height){
			cursor_y = y;
		}
		else {
			cursor_x = width/2;
			cursor_y = height/2;
		}
	}
	else {
		cursor_x = width/2;
		cursor_y = height/2;
	}

}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action==GLFW_PRESS){
	    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
	        glfwSetWindowShouldClose(window, GL_TRUE);
		}

		if (key == GLFW_KEY_UP) {
			stride_factor = 1;
		}

		if (key == GLFW_KEY_DOWN) {
			stride_factor = -1;
		}

		if(key == GLFW_KEY_LEFT) {
			strafe_factor = STRAFE_LEFT;
		}

		if(key == GLFW_KEY_RIGHT) {
			strafe_factor = STRAFE_RIGHT;
		}
	}
	if (action==GLFW_RELEASE) {
		if(key==GLFW_KEY_UP && stride_factor == 1) {
			stride_factor = 0;
		}
		if(key==GLFW_KEY_DOWN && stride_factor == -1) {
			stride_factor = 0;
		}

		if(key == GLFW_KEY_LEFT && strafe_factor == STRAFE_LEFT) {
			strafe_factor = NO_STRAFE;
		}

		if(key == GLFW_KEY_RIGHT && strafe_factor == STRAFE_RIGHT) {
			strafe_factor = NO_STRAFE;
		}
	}
}
