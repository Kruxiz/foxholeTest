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
#include "Player.h"
#include "CollisionDetector.h"
#include <random>
#include "bass.h"  //sound library
#include "SDL_ttf.h" //text library
#include "md2model.h"

enum SceneState {
	PAUSE,
	IN_GAME,
	MAIN_MENU,
	SCORES,
	CONTROLS,
	COUNTDOWN,
	CHOOSE_NAME
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
	GLuint skybox2[5];

	int collectables;

	TTF_Font * textFont;

	std::unordered_map<std::string, Menu> menus;
	SceneState sceneState;


	std::chrono::time_point<std::chrono::system_clock> timer;
	double time = 0;
	std::chrono::time_point<std::chrono::system_clock> pauseTimer;
	double pauseTime = 0;
	std::chrono::time_point<std::chrono::system_clock> waterRespawnTimer;
	std::chrono::time_point<std::chrono::system_clock> carRespawnTimer;
	double levelTime = 0;

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

	GLuint textToTexture(const char * str, GLuint textID);

	std::vector<rt3d::lightStruct> lights;
	glm::vec4 lightPos; //light position

	std::vector<rt3d::materialStruct> materials;

	GLuint skyboxProgram;
	GLuint shaderProgram;
	GLuint textureProgram;
	GLuint modelProgram;


	Player player;
	GLfloat cameraR = 0.0f;
	md2model foxModel;
	int currentAnimation = 0;

	std::vector<GameObject> gameObjects;
	std::unordered_map<std::string, int> objMeshIndexCounts;

	int level; 

	std::vector<HSAMPLE> sounds;
	HCHANNEL backgroundNoise;
	HCHANNEL walkingNoise = NULL;

	static glm::vec3 moveForward(glm::vec3 pos, GLfloat angle, GLfloat d);
	static glm::vec3 moveRight(glm::vec3 pos, GLfloat angle, GLfloat d);
	void initCamera();
	void initTTF();
	//holds the game objects for the levels - only loads particular objects based on current level
	void initGameObjects();
	//update specific car in game objects vector to make it drive back and forth
	void updateCar(int carIndex);
	//build trees round first level
	void buildTrees();
	void initPlayer();
	//render specific game object
	void renderObject(GameObject gObj);
	//render timer, collectables left, countdown display, level messages
	void renderHUD();
	//separate render method for player as player needs different call to rt3d
	void renderPlayer();
	double getTimeScalar();
	HSAMPLE loadSounds(char * filename);
	void initSounds();
	//render specific hud object
	void renderHUDObject(MenuObject menuObj);
	//render scores in score vector as hud objects
	void addToScores();
	//if finished first level, save score in temp score
	//if finished second level, save score in temp score and add to score vectors
	void saveScores(double levelTime, int level);
	//fall if not colliding with objects
	void playerFall(bool spaceUp);
	//respawn if colliding with cars, water or level end on level 1
	void checkPlayerRespawn();
	//if player collided with collectables, erase collectable from game objects vector
	void detectCollectableCollision();
	//check if finished level 2
	void checkEndLevel();
	//sort and erase score vectors so only contains top 5
	void findHighScores();
	//load scores from txt
	void loadScores();
	void renderPlayerChars();
	//write scores to txt
	void writeScores();
	void chooseName() { sceneState = CHOOSE_NAME; }
	//rotate all collectables
	void updateCollectables();
	GLuint loadCubeMap(const char * fname[6], GLuint * texID);
	GLuint loadBitmap(char * fname);
	//check player collision with every other object in level
	bool checkCollisions();
	//check specific object with every other object in level
	bool checkCollisions(GameObject &specObj);
	GameObject getGameObject(std::string objName);
	int getGameObjectIndex(std::string objName);
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
	void playerJump();
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
