#include "WorldManager.h"
#include "Model.h"
#include "rt3d.h"

typedef stack<glm::mat4> mvstack;

namespace WorldManager {

	GLuint shaderProgram;
	GLuint textureProgram;
	GLuint modelProgram;

	const char *cubeTexFiles[6] = {
		"Town-skybox/Town_bk.bmp", "Town-skybox/Town_ft.bmp", "Town-skybox/Town_rt.bmp", "Town-skybox/Town_lf.bmp", "Town-skybox/Town_up.bmp", "Town-skybox/Town_dn.bmp"
	};
	loadCubeMap(cubeTexFiles, &skybox[0]);

	// Load models
	Model *foxModel;

	mvstack mvStack;

	glm::vec3 eye(0.0f, 1.0f, 0.0f);
	glm::vec3 at(0.0f, 1.0f, -1.0f);
	glm::vec3 up(0.0f, 1.0f, 0.0f);

	rt3d::lightStruct testLight = {
		{ 0.3f, 0.3f, 0.3f, 1.0f }, // ambient
		{ 1.0f, 1.0f, 1.0f, 1.0f }, // diffuse
		{ 1.0f, 1.0f, 1.0f, 1.0f }, // specular
		{ -10.0f, 10.0f, 10.0f, 1.0f }  // position
	};
	glm::vec4 lightPos(0.0f, 2.0f, -6.0f, 1.0f); //light position

	rt3d::materialStruct material0 = {
		{ 0.2f, 0.4f, 0.2f, 1.0f }, // ambient
		{ 0.5f, 1.0f, 0.5f, 1.0f }, // diffuse
		{ 0.0f, 0.1f, 0.0f, 1.0f }, // specular
		2.0f  // shininess
	};
	
	void initialise(void) {
		shaderProgram = rt3d::initShaders("phong-tex.vert", "phong-tex.frag");
		textureProgram = rt3d::initShaders("textured.vert", "textured.frag");
		modelProgram = rt3d::initShaders("modelLoading.vert", "modelLoading.frag");
		foxModel = new Model("Nanosuit/nanosuit.obj");

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	glm::vec3 moveForward(glm::vec3 pos, GLfloat angle, GLfloat d) {
		return glm::vec3(pos.x + d*std::sin(r*DEG_TO_RADIAN), pos.y, pos.z - d*std::cos(r*DEG_TO_RADIAN));
	}

	glm::vec3 moveRight(glm::vec3 pos, GLfloat angle, GLfloat d) {
		return glm::vec3(pos.x + d*std::cos(r*DEG_TO_RADIAN), pos.y, pos.z + d*std::sin(r*DEG_TO_RADIAN));
	}
}
