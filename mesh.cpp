#include "mesh.hpp"

#include <vector>
#include <iostream>

#include <GL/glew.h>

#include <IL/il.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#define GLM_FORCE_RADIANS

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

using namespace std;

static int _countingTextureIndex = 0;;

Mesh::Mesh(const char* filename) {

	_textureIndex = _countingTextureIndex;
	_countingTextureIndex ++;

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filename, aiProcess_Triangulate);
	if(!scene){
		fprintf(stderr, "%s", importer.GetErrorString());
	}
	vector<const aiMesh*> meshes(scene->mMeshes,scene->mMeshes+scene->mNumMeshes);

	GLuint buffer;

	// generate Vertex Array for mesh
	glGenVertexArrays(1,&_vao);
	glBindVertexArray(_vao);

	_polyCount = meshes[0]->mNumVertices;


	{

		float* normals = (float*)malloc(sizeof(float)*_polyCount*3);
		int offset = 0;
		for(int i = 0; i < meshes.size(); i++) {
			int count = meshes[i]->mNumVertices*3;
			memcpy(&normals[offset],meshes[i]->mNormals,count*sizeof(float));
			offset += count;
		}

		glGenBuffers(1, &buffer);
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float)*3*_polyCount, normals, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, 0, 0, 0);
	}

	{
		float *texCoords = (float*)malloc(sizeof(float)*_polyCount*2);

		for(int i = 0; i < meshes[0]->mNumVertices; i++) {
			texCoords[i*2]   = meshes[0]->mTextureCoords[0][i].x;
			texCoords[i*2+1] = meshes[0]->mTextureCoords[0][i].y;
		}

		glGenBuffers(1, &buffer);
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float)*2*_polyCount, texCoords, GL_STATIC_DRAW);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, 0, 0, 0);
	}
	
	// load bone influence
	typedef struct {
		int bone_ids[4];
		float weights[4];
	} BoneInfluence;

	BoneInfluence* bone_influence = (BoneInfluence*)calloc(_polyCount,sizeof(BoneInfluence));
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
	glBufferData(GL_ARRAY_BUFFER, sizeof(BoneInfluence)*_polyCount,bone_influence,GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribIPointer(2, 4, GL_INT, sizeof(BoneInfluence), (const GLvoid*)0);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(BoneInfluence), (const GLvoid*)16);

	for(int i = 0; i < meshes.size(); i++){
		glGenBuffers(1, &buffer);
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float)*3*_polyCount, meshes[i]->mVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(4+i);
		glVertexAttribPointer(4+i, 3, GL_FLOAT, 0, 0, 0);
	}

	// unbind buffers
	glBindBuffer(GL_ARRAY_BUFFER,0);
	glBindVertexArray(0);

	GLuint imageID;
	ilInit();
	ilGenImages(1, &imageID);
	ilBindImage(imageID);
	ilEnable(IL_ORIGIN_SET);
	ilOriginFunc(IL_ORIGIN_LOWER_LEFT);

	aiString path;
	if(scene->mMaterials[meshes[0]->mMaterialIndex]->GetTexture(aiTextureType_DIFFUSE, 0, &path) != AI_SUCCESS) {
		cout << "Could not load texture filename" << endl;
	}

	if(!ilLoadImage(("resources/"+string(path.C_Str())).c_str())){
		cout << "Failed to load image:" << path.C_Str() << endl;
	}
	ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);

	ILuint texture_buffer;
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &texture_buffer);
	glActiveTexture(GL_TEXTURE0 + _textureIndex);
	glBindTexture(GL_TEXTURE_2D, texture_buffer);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT),0, GL_RGBA, GL_UNSIGNED_BYTE, ilGetData());
	glGenerateMipmap(GL_TEXTURE_2D);

	ilShutDown();

	_boneAnimation = new BoneAnimation(scene);
}


void Mesh::draw(GLuint shader, double time) {
	GLuint bones_loc = glGetUniformLocation (shader, "BONES");
	glUniformMatrix4fv (bones_loc, _boneAnimation->numBones(), GL_FALSE, _boneAnimation->getBonesAtTime(time));

	GLuint textureLoc = glGetUniformLocation(shader, "uni_tex");
	glUniform1i(textureLoc, _textureIndex);
	glBindVertexArray(_vao);
	glDrawArrays( GL_TRIANGLES, 0, _polyCount );
	glBindVertexArray(0);
}
