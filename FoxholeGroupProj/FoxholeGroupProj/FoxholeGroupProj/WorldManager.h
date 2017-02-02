#ifndef WORLDMANAGER
#define WORLDMANAGER

#include "SDL.h"
#include <stack>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class WorldManager {
public:
	static void initialise(void);
	static void update(SDL_Window * window, SDL_Event sdlEvent);
	static void draw(SDL_Window *window, float fps);
};

#endif
