#ifndef SDLMANAGER_H
#define SDLMANAGER_H

#include <SDL.h>
#include <gl/glew.h>
#include "SceneManager.h"
#include "rt3d.h"
#include "GLEWManager.h"

class SceneManager;

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
	static GLuint loadCubeMap(const char * fname[6], GLuint * texID);
	static GLuint loadBitmap(char * fname);
	//static Uint8 
};

#endif
