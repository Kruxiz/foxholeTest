#pragma once
#ifndef GAMEENGINE_H
#define GAMEENGINE_H

class GameEngine {
public:
	GameEngine();
	//GameEngine(with params for greater customisation);
	int run(); //note: int used as error code
private:
	//vector of scenes
	int initialise(); //params may be added for customisation
	//bool addScene
	//bool deleteScene



};

#endif
