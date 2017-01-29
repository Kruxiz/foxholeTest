#include "Initialise.h"
#include <vector>
#include "SDLManager.h"
#include "rt3d.h"
#include "rt3dObjLoader.h"
#include <GL/glew.h>

void Initialise::init(void)
{
	GLuint shaderProg = rt3d::initShaders("phong-tex.vert", "phong-tex.frag");
	rt3d::setLight(shaderProg, light0);
	rt3d::setMaterial(shaderProg, material0);

	//matching textureUnits
	GLuint uniformIndex = glGetUniformLocation(shaderProg, "textureUnit0");
	glUniform1i(uniformIndex, 0);
	uniformIndex = glGetUniformLocation(shaderProg, "textureUnit1");
	glUniform1i(uniformIndex, 1);
	uniformIndex = glGetUniformLocation(shaderProg, "textureUnit2");
	glUniform1i(uniformIndex, 2);

	//skybox program needed in draw method
	//GLuint skyboxProgram = rt3d::initShaders("cubeMap.vert", "cubeMap.frag");

	//doesn't appear to be used anywhere else question mark question mark question mark
	GLuint textureProgram = rt3d::initShaders("textured.vert", "textured.frag");

	const char *cubeTexFiles[6] = {
		"Town-skybox/grass1.bmp", "Town-skybox/grass1.bmp", "Town-skybox/grass1.bmp", "Town-skybox/grass1.bmp", "Town-skybox/grass1.bmp", "Town-skybox/grass1.bmp"
	};

	SDLManager::loadCubeMap(cubeTexFiles, skybox);

	std::vector<GLfloat> verts;
	std::vector<GLfloat> norms;
	std::vector<GLfloat> tex_coords;
	std::vector<GLuint> indices;
	rt3d::loadObj("cube.obj", verts, norms, tex_coords, indices);
	meshIndexCount = indices.size();

	textures.push_back(SDLManager::loadBitmap("fabric.bmp"));

	meshObjects.push_back(rt3d::createMesh(verts.size() / 3, 
		verts.data(), nullptr, norms.data(), 
		tex_coords.data(), meshIndexCount, 
		indices.data()));

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

//all these should be in some other class/file
/*
GLuint getMeshIndexCount() {
	GLuint meshIndexCount = 0;
	return meshIndexCount;
}

GLuint* getSkyBox() {
	GLuint skybox[5];
	return skybox;
}

rt3d::lightStruct getLight0() {
	rt3d::lightStruct light0 = {
		{ 0.3f, 0.3f, 0.3f, 1.0f }, // ambient
		{ 1.0f, 1.0f, 1.0f, 1.0f }, // diffuse
		{ 1.0f, 1.0f, 1.0f, 1.0f }, // specular
		{ -10.0f, 10.0f, 10.0f, 1.0f }  // position
	};

	return light0;
}

rt3d::materialStruct getMaterial0() {
	rt3d::materialStruct material0 = {
		{ 0.0f, 0.0f, 1.0f, 1.0f }, // ambient
		{ 0.5f, 1.0f, 0.5f, 1.0f }, // diffuse
		{ 0.0f, 0.1f, 0.0f, 1.0f }, // specular
		2.0f  // shininess
	};

	return material0;

}

GLuint getSkyBoxProg() {
	GLuint skyboxProgram = rt3d::initShaders("cubeMap.vert", "cubeMap.frag");
	return skyboxProgram;
}*/
