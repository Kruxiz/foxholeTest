#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

#include <vector>
#include <stack>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "rt3d.h"
#include "rt3dObjLoader.h"
#include "SDLManager.h"
#include "Player.h"

class SceneManager {
private:
	std::vector<GLuint> meshObjects;
	GLuint meshIndexCount = 0;
	glm::vec3 eye;
	glm::vec3 at;
	glm::vec3 up;
	std::stack<glm::mat4> mvStack;
	std::vector<GLuint> textures;
	GLuint skybox[5];

	std::vector<rt3d::lightStruct> lights;
	glm::vec4 lightPos; //light position

	std::vector<rt3d::materialStruct> materials;

	GLuint skyboxProgram;
	GLuint shaderProgram;

	Player player;

	static glm::vec3 moveForward(glm::vec3 pos, GLfloat angle, GLfloat d);
	static glm::vec3 moveRight(glm::vec3 pos, GLfloat angle, GLfloat d);
	void initCamera();
	void renderObject();
public:
	SceneManager();
	void renderSkybox(glm::mat4 projection);
	void clearScreen();
	glm::mat4 initRendering();
	void init();
	void popMatrixStack() { mvStack.pop(); }
	void setShaderProjection(glm::mat4 projection);
	void setLights();
	void renderObjects();
	void updatePlayerR(GLfloat deltaR);
};

#endif