#pragma once
#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

#include <vector>
#include <stack>
#include <unordered_map>
#include <tuple>
#include <chrono>
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
#include "bass.h"  //sound library
#include "SDL_ttf.h" //text library
//#include "Model.h"

enum SceneState {
	PAUSE, //todo possibly add 'countdown' state
	IN_GAME,
	MAIN_MENU,
	SCORES,
	CONTROLS,
	COUNTDOWN
};

typedef std::tuple<std::string, glm::vec3, glm::vec3> MenuObject;
typedef std::vector<MenuObject> Menu;

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

	int collectables;

	//hud
	//GLuint labels[5];
	TTF_Font * textFont;

	std::unordered_map<std::string, Menu> menus;
	SceneState sceneState;

	//bool pause = false; //todo deprecate

	std::chrono::time_point<std::chrono::system_clock> timer;
	double time = 0;
	std::chrono::time_point<std::chrono::system_clock> pauseTimer;
	double pauseTime = 0;
	std::chrono::time_point<std::chrono::system_clock> respawnTimer;

	GLuint textToTexture(const char * str, GLuint textID);

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

	std::vector<HSAMPLE> sounds;

	static glm::vec3 moveForward(glm::vec3 pos, GLfloat angle, GLfloat d);
	static glm::vec3 moveRight(glm::vec3 pos, GLfloat angle, GLfloat d);
	void initCamera();
	void initTTF();
	void initGameObjects();
	void buildTrees();
	void initPlayer();
	void renderObject(GameObject gObj);
	void renderHUD();
	void renderPlayer();
	double getTimeScalar();
	int countCollectables();
	HSAMPLE loadSounds(char * filename);
	void initSounds();
	void renderHUDObject(MenuObject menuObj);
	void addToScores();

public:
	SceneManager();
	void togglePause();
	void checkSwitchLevel();
	void renderSkybox(glm::mat4 projection);
	void clearScreen();
	glm::mat4 initRendering();
	void init();
	void popMatrixStack() { mvStack.pop(); }
	void setShaderProjection(glm::mat4 projection);
	void setLights();
	void renderObjects();
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
	void freeBass();
	bool inGame() { return sceneState == IN_GAME; }
	bool inMainMenu() { return sceneState == MAIN_MENU; }
	bool inControls() { return sceneState == CONTROLS; }
	bool inScores() { return sceneState == SCORES; }
	bool paused() { return sceneState == PAUSE; }
	bool inCountdown();
	void play();
	void mainMenu();
	void controls() { sceneState = CONTROLS; }
	void scores() { sceneState = SCORES; }
	void countdown() { sceneState = COUNTDOWN; }
	void renderMenus();
};

#endif