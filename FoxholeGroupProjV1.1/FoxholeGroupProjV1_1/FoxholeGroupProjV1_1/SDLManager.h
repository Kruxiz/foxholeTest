#ifndef SDLMANAGER_H
#define SDLMANAGER_H

#include <SDL.h>
#include <gl/glew.h>
#include "SceneManager.h"
#include "rt3d.h"
#include "GLEWManager.h"

class SceneManager;

class SDLManager {
private:
	SDL_Window * window;
	SDL_GLContext context;
	SceneManager * scene;
	void SDLDraw();
	void SDLUpdate();
public:
	SDLManager();
	void SDLInit(void);
	void SDLEnd(void);
	void SDLRun(void);
	static GLuint loadCubeMap(const char * fname[6], GLuint * texID);
	static GLuint loadBitmap(char * fname);
};

#endif
