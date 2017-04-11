#pragma once
#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

#include <vector>
#include <stack>
#include <unordered_map>
#include <tuple>
#include <sstream>
#include <chrono>
#include <algorithm>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "rt3d.h"
#include "rt3dObjLoader.h"
#include "GameManager.h"
#include "Player.h"
#include "CollisionDetector.h"
#include <random>
#include "bass.h"  //sound library
#include "SDL_ttf.h" //text library
#include "md2model.h"
//#include "Model.h"

enum SceneState {
	PAUSE,
	IN_GAME,
	MAIN_MENU,
	SCORES,
	CONTROLS,
	COUNTDOWN,
	CHOOSE_NAME
};

//enum ActivePlayerChar {
//	FIRST,
//	SECOND,
//	THIRD
//};

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
	GLuint skybox2[5];

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
	std::chrono::time_point<std::chrono::system_clock> waterRespawnTimer;
	std::chrono::time_point<std::chrono::system_clock> carRespawnTimer;
	double levelTime = 0;

	//could change to tuple to accomodate more levels
	//std::unordered_map<std::string, double> highScores1;
	//std::unordered_map<std::string, double> highScores2;
	std::vector<std::pair<std::string, double>> highscores1;
	std::vector<std::pair<std::string, double>> highscores2;

	std::pair<double, double> tempScore;
	std::string playerName;
	bool playerNameSet = false;
	char playerName1 = 'A';
	char playerName2 = 'A';
	char playerName3 = 'A';
	int activeChar = 1;
	bool highscoreOnLevel1 = false;
	bool highscoreOnLevel2 = false;
	//double tempScore;

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
	GLfloat cameraR = 0.0f;
	md2model foxModel;
	int currentAnimation = 0;

	std::vector<GameObject> gameObjects;
	std::unordered_map<std::string, int> objMeshIndexCounts;

	int level; // probs better as struct

	const GLuint gravity = 1.1; // needed?

	std::vector<HSAMPLE> sounds;
	HCHANNEL backgroundNoise;
	HCHANNEL walkingNoise = NULL;

	static glm::vec3 moveForward(glm::vec3 pos, GLfloat angle, GLfloat d);
	static glm::vec3 moveRight(glm::vec3 pos, GLfloat angle, GLfloat d);
	void initCamera();
	void initTTF();
	void initGameObjects();
	void updateCar(int carIndex);
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
	void saveScores(double levelTime, int level);
	void playerFall(bool spaceUp);
	void checkPlayerRespawn();
	void detectCollectableCollision();
	void checkEndLevel();
	void findHighScores();
	void loadScores();
	void renderPlayerChars();
	void writeScores();
	void chooseName() { sceneState = CHOOSE_NAME; }
	void updateCollectables();
public:
	SceneManager();
	void togglePause();
	void playBloop();
	void playBleep();
	void stand();
	void renderSkybox(glm::mat4 projection);
	void clearScreen();
	glm::mat4 initRendering();
	void respawnPlayer();
	void init();
	void popMatrixStack() { mvStack.pop(); }
	void setShaderProjection(glm::mat4 projection);
	void setLights();
	void renderObjects();
	void updatePlayerR(GLfloat deltaR);
	void movePlayerForward(GLfloat delta);
	void movePlayerRight(GLfloat delta);
	bool checkCollisions();
	bool checkCollisions(GameObject &specObj);
	void playerJump();
	GameObject getGameObject(std::string objName);
	int getGameObjectIndex(std::string objName);
	void freeBass();
	bool inGame() { return sceneState == IN_GAME; }
	bool inMainMenu() { return sceneState == MAIN_MENU; }
	bool inControls() { return sceneState == CONTROLS; }
	bool inScores() { return sceneState == SCORES; }
	bool paused() { return sceneState == PAUSE; }
	bool inChooseName() { return sceneState == CHOOSE_NAME; }
	bool inCountdown();
	void play();
	void mainMenu();
	void controls() { sceneState = CONTROLS; }
	void scores() { BASS_Pause(); sceneState = SCORES; }
	void countdown() { sceneState = COUNTDOWN; }
	void renderMenus();
	void playerUpdate(bool spaceUp);
	void changeActiveChar(bool right);
	void changeCurrentChar(bool up);
	void chooseNameAndPlay();
};

#endif