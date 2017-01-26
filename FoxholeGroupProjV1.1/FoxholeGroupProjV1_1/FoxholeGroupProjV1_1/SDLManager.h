#ifndef SDLMANAGER_H
#define SDLMANAGER_H

#include <SDL.h>

class SDLManager {
private:
	SDL_Window * window;
	SDL_GLContext context;
	void SDLDraw();
	void SDLUpdate();
public:
	SDLManager();
	void SDLInit(void);
	void SDLEnd(void);
	void SDLRun(void);
};

#endif
