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
#include "rt3d.h"

SDL_Window * setupRC(SDL_GLContext &context) {
	SDL_Window * window;
	if (SDL_Init(SDL_INIT_VIDEO) < 0) // Initialize video
		rt3d::exitFatalError("Unable to initialize SDL");

	// Request an OpenGL 3.0 context.

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);  // double buffering on
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8); // 8 bit alpha buffering
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4); // Turn on x4 multisampling anti-aliasing (MSAA)

													   // Create 800x600 window
	window = SDL_CreateWindow("SDL/GLM/OpenGL Demo", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	if (!window) // Check window was created OK
		rt3d::exitFatalError("Unable to create window");

	context = SDL_GL_CreateContext(window); // Create opengl context and attach to window
	SDL_GL_SetSwapInterval(1); // set swap buffers to sync with monitor's vertical refresh rate
	return window;
}

void draw(SDL_Window * window) {
	// clear the screen

	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	//rt3d::drawMesh(meshObjects[0], 3, GL_TRIANGLES);
	//rt3d::drawMesh(meshObjects[1], 3, GL_TRIANGLES);
	//rt3d::drawMesh(meshObjects[2], 6, GL_TRIANGLES);

	SDL_GL_SwapWindow(window); // swap buffers
}

// Program entry point - SDL manages the actual WinMain entry point for us
int main(int argc, char *argv[]) {

	std::cout << "blorp" << std::endl;

	/*
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
	return 0;*/

	SDL_Window * hWindow; // window handle
	SDL_GLContext glContext; // OpenGL context handle
	hWindow = setupRC(glContext); // Create window and render context 

								  // Required on Windows *only* init GLEW to access OpenGL beyond 1.1
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (GLEW_OK != err) { // glewInit failed, something is seriously wrong
		std::cout << "glewInit failed, aborting." << std::endl;
		exit(1);
	}
	//cout << glGetString(GL_VERSION) << endl;

	//init();

	bool running = true; // set running to true
	SDL_Event sdlEvent;  // variable to detect SDL events
	while (running) {	// the event loop
		while (SDL_PollEvent(&sdlEvent)) {

			if (sdlEvent.type == SDL_QUIT)
				running = false;

			//if (sdlEvent.type == SDL_MOUSEMOTION)
			//{
			//	SDL_SetRelativeMouseMode(SDL_TRUE);
			//	SDL_WarpMouseInWindow(NULL, 800, 600);
			//	/* If the mouse is moving to the left */
			//	if (sdlEvent.motion.xrel < 0)
			//		r -= 3.0f;
			//	/* If the mouse is moving to the right */
			//	else if (sdlEvent.motion.xrel > 0)
			//		r += 3.0f;
			//}
		}

		if (sdlEvent.type = SDL_KEYDOWN && sdlEvent.key.keysym.sym == SDLK_ESCAPE) {
			running = false;
		}

		//update();
		draw(hWindow); // call the draw function
		//bool checkCollision(SDL_Rect a, SDL_Rect b);
	}

	SDL_GL_DeleteContext(glContext);
	SDL_DestroyWindow(hWindow);
	SDL_Quit();
	return 0;
}
