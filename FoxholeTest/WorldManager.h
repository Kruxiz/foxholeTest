#ifndef WORLDMANAGER
#define WORLDMANAGER

#include "SDL.h"
#include <stack>
#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>

#define DEG_TO_RADIAN 0.017453293

namespace WorldManager {
	void init(void);
	void update(SDL_Window * window, SDL_Event sdlEvent);
	void draw(SDL_Window *window, float fps);
}

#endif
