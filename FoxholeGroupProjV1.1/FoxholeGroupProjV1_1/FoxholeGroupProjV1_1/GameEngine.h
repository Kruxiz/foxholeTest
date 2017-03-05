#pragma once
#ifndef GAMEENGINE_H
#define GAMEENGINE_H

class GameEngine {
public:
	GameEngine();
	//GameEngine(with params for greater customisation);
	int run(); //int used as error code - loop occurs here
private:
	int initialise(); //params may be added for customisation

	//vector of scenes
	//bool addScene
	//bool deleteScene

	//game event reference - holds methods of all game events - only one reference

	//update() - check game events  - update accordingly

};

#endif
