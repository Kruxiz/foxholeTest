
#ifndef SDLMANAGER
#define SDLMANAGER

#include <SDL.h>
#include <iostream>

class SDLController {
public:
	// Something went wrong - print error message and quit
	static void exitFatalError(char *message);
	static SDL_Window * setupRC(SDL_GLContext &context);


};

#endif