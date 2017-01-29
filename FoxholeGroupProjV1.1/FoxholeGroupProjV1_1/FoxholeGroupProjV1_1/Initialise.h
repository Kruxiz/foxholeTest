#ifndef INITIALISE_H
#define INITIALISE_H

class Initialise {
public:
	/*GLuint getMeshIndexCount() { return meshIndexCount; }
	GLuint* getSkyBox() { return skybox; }
	GLuint getSkyBoxProg() { return skyboxProgram; }
	rt3d::lightStruct getLight0() { return light0; }
	rt3d::materialStruct getMaterial0() { return material0; }*/
	static void init(void);
private:
	GLuint meshIndexCount = 0;
	GLuint skybox[5];
	rt3d::lightStruct light0 = {
		{ 0.3f, 0.3f, 0.3f, 1.0f }, // ambient
		{ 1.0f, 1.0f, 1.0f, 1.0f }, // diffuse
		{ 1.0f, 1.0f, 1.0f, 1.0f }, // specular
		{ -10.0f, 10.0f, 10.0f, 1.0f }  // position
	};
	rt3d::materialStruct material0 = {
		{ 0.0f, 0.0f, 1.0f, 1.0f }, // ambient
		{ 0.5f, 1.0f, 0.5f, 1.0f }, // diffuse
		{ 0.0f, 0.1f, 0.0f, 1.0f }, // specular
		2.0f  // shininess
	};
	GLuint skyboxProgram = rt3d::initShaders("cubeMap.vert", "cubeMap.frag");
};
#endif
