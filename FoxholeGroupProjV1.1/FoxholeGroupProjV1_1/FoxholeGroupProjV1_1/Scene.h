#pragma once
#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include <functional>
#include "Player.h"

class Scene {
public:
	Scene(); // scene for each level

	//events - key presses
	void keyPressW();
	void keyPressA();
	void keyPressS();
	void keyPressD();
	void keyPressSpace();

	void keyPress1();
	void keyPress2();

	void keyPressR();


private:
	//vector of game objects
	std::vector<GameObject> gameObjects;

	//skybox
	GameObject skybox;

	//player reference - should only be one!!!
	Player player;

	//scene events


	//ui stuff

	//sound stuff
};

#endif