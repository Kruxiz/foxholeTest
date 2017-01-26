//#include "main.h"
#if _DEBUG
#pragma comment(linker, "/subsystem:\"console\" /entry:\"WinMainCRTStartup\"")
#endif

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <SDL.h>
#include <iostream>
#include <glew.h>
#include "WorldManager.h"
#include "SDLController.h"



// Program entry point - SDL manages the actual WinMain entry point for us
int main(int argc, char *argv[]) {


	SDL_Window * hWindow; // window handle
	SDL_GLContext glContext; // OpenGL context handle
	hWindow = SDLController::setupRC(glContext); // Create window and render context 

	// Required on Windows *only* init GLEW to access OpenGL beyond 1.1
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	 
	if (GLEW_OK != err) { // glewInit failed, something is seriously wrong
		std::cout << "glewInit failed, aborting." << std::endl;
		exit(1);
	}
	std::cout << glGetString(GL_VERSION) << std::endl;

	WorldManager::initialise();

	bool running = true; // set running to true
	SDL_Event sdlEvent;  // variable to detect SDL events
	while (running) {	// the event loop
		while (SDL_PollEvent(&sdlEvent)) {
			if (sdlEvent.type == SDL_QUIT)
				running = false;
		}
		WorldManager::update(hWindow, sdlEvent);	// update function
		WorldManager::draw(hWindow, 0); // draw function

	}

	SDL_GL_DeleteContext(glContext);
	SDL_DestroyWindow(hWindow);
	SDL_Quit();
	return 0;
}
