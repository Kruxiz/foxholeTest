#ifndef WORLDMANAGER
#define WORLDMANAGER

#include "SDL.h"
#include <stack>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace WorldManager {
	void initialise(void);
	void update(SDL_Window * window, SDL_Event sdlEvent);
	void draw(SDL_Window *window, float fps);
}

#endif
