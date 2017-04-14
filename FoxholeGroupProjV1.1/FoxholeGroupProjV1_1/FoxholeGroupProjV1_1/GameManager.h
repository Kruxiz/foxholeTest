#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include <SDL.h>
#include <gl/glew.h>
#include "SceneManager.h"
#include "rt3d.h"


class GameManager {
private:
	SDL_Window * window;
	SDL_GLContext context;
	SceneManager * scene;
	void GameDraw();
	void GameUpdate(bool spaceUp);
public:
	GameManager();
	void GameInit(void);
	void GameEnd(void);
	void GameRun(void);
};

#endif
