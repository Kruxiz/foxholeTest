#pragma once
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
#include "CollisionDetector.h"
#include <random>
//#include "bass.h"  //sound library
//#include "SDL_ttf.h" //text library
//#include "Model.h"


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
	GLuint textureProgram;
	GLuint modelProgram;

	//Model *foxModel;

	Player player;

	std::vector<GameObject> gameObjects;

	int level; // probs better as struct

	const GLuint gravity = 1.1; // needed?

	static glm::vec3 moveForward(glm::vec3 pos, GLfloat angle, GLfloat d);
	static glm::vec3 moveRight(glm::vec3 pos, GLfloat angle, GLfloat d);
	void initCamera();
	void initGameObjects();
	void buildTrees();
	void initPlayer();
	void renderObject(GameObject gObj);
	void renderPlayer();
	double getTimeScalar();
	int countCollectables();

public:
	SceneManager();
	void checkSwitchLevel();
	void renderSkybox(glm::mat4 projection);
	void clearScreen();
	glm::mat4 initRendering();
	void init();
	void popMatrixStack() { mvStack.pop(); }
	void setShaderProjection(glm::mat4 projection);
	void setLights();
	void renderObjects();
	void sound();
	void textOnScreen();
	void updatePlayerR(GLfloat deltaR);
	void detectCollectableCollision();
	void movePlayerForward(GLfloat delta);
	void movePlayerRight(GLfloat delta);
	bool checkCollisions();
	bool checkCollisions(GameObject &specObj);
	void playerJump();
	void playerFall();
	GameObject getGameObject(std::string objName);
	int getGameObjectIndex(std::string objName);
	void setPlayerJumpFalse();
	void respawnPlayer();
	void checkPlayerRespawn();

};

#endif