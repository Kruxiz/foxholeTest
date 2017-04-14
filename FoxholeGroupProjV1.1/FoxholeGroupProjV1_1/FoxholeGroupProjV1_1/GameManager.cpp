
#include "GameManager.h"

GameManager::GameManager() {
	window = nullptr;
	context = 0;
};

void GameManager::GameInit()
{
	//SDL_Window * window;
	if (SDL_Init(SDL_INIT_VIDEO) < 0) // Initialize video
		rt3d::exitFatalError("Unable to initialize SDL");

	// Request an OpenGL 3.0 context.
	// Not able to use SDL to choose profile (yet), should default to core profile on 3.2 or later
	// If you request a context not supported by your drivers, no OpenGL context will be created

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);  // double buffering on
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4); // Turn on x4 multisampling anti-aliasing (MSAA)

													   // Create 800x600 window
	window = SDL_CreateWindow("Foxhole", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	if (!window) // Check window was created OK
		rt3d::exitFatalError("Unable to create window");

	context = SDL_GL_CreateContext(window); // Create opengl context and attach to window
	SDL_GL_SetSwapInterval(1); // set swap buffers to sync with monitor's vertical refresh rate

	// Required on Windows *only* init GLEW to access OpenGL beyond 1.1
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (GLEW_OK != err) { // glewInit failed, something is seriously wrong
		std::cout << "glewInit failed, aborting." << std::endl;
		exit(1);
	}
	std::cout << glGetString(GL_VERSION) << std::endl;

	scene = new SceneManager();

	scene->init();
}

void GameManager::GameEnd(void)
{
	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
	scene->freeBass();
	delete scene;
	SDL_Quit();
}

void GameManager::GameRun(void)
{
	bool running = true; // set running to true
	SDL_Event sdlEvent;  // variable to detect SDL events
	while (running) {	// the event loop
		bool spaceUp = false;
		while (SDL_PollEvent(&sdlEvent)) {
			if (sdlEvent.type == SDL_QUIT || (sdlEvent.type == SDL_KEYDOWN && sdlEvent.key.keysym.sym == SDLK_ESCAPE  && scene->inMainMenu()))
				running = false;

			if (sdlEvent.type == SDL_KEYUP && sdlEvent.key.keysym.sym == SDLK_TAB && (scene->inGame() ^ scene->paused() ^ scene->inCountdown())) {
				scene->togglePause();
			}

			if (sdlEvent.type == SDL_MOUSEMOTION && !scene->paused() && !scene->inCountdown())
			{
				SDL_SetRelativeMouseMode(SDL_TRUE);
				SDL_WarpMouseInWindow(NULL, 800, 600);
				/* If the mouse is moving to the left */
				if (sdlEvent.motion.xrel < 0)
					scene->updatePlayerR(1.5f);
				/* If the mouse is moving to the right */
				else if (sdlEvent.motion.xrel > 0)
					scene->updatePlayerR(-1.5f);
			}

			if (sdlEvent.type == SDL_KEYUP && sdlEvent.key.keysym.sym == SDLK_SPACE) {
				spaceUp = true;
			}

			if (scene->inChooseName()) {
				if (sdlEvent.key.keysym.sym == SDLK_RIGHT && sdlEvent.type == SDL_KEYUP) {
					scene->changeActiveChar(true);
					scene->playBleep();
				}
				if (sdlEvent.key.keysym.sym == SDLK_LEFT && sdlEvent.type == SDL_KEYUP) {
					scene->changeActiveChar(false);
					scene->playBleep();
				}
				if (sdlEvent.key.keysym.sym == SDLK_UP && sdlEvent.type == SDL_KEYUP) {
					scene->changeCurrentChar(true);
					scene->playBleep();
				}
				if (sdlEvent.key.keysym.sym == SDLK_DOWN && sdlEvent.type == SDL_KEYUP) {
					scene->changeCurrentChar(false);
					scene->playBleep();
				}

			}

		}
		GameDraw();
		GameUpdate(spaceUp);
	}

}

void GameManager::GameDraw()
{
	scene->clearScreen();

	if (scene->inMainMenu() || scene->inControls() || scene->inScores() || scene->inChooseName()) {
		glm::mat4 projection = scene->initRendering();

		scene->setShaderProjection(projection);

		scene->renderMenus();
	}
	else {
		glm::mat4 projection = scene->initRendering();

		scene->renderSkybox(projection);

		scene->setShaderProjection(projection);

		//render objects
		scene->renderObjects();

		scene->popMatrixStack();

		glDepthMask(GL_TRUE);
	}
	SDL_GL_SwapWindow(window); // swap buffers
}

void GameManager::GameUpdate(bool spaceUp)
{
	const Uint8 *keys = SDL_GetKeyboardState(NULL);

	if (scene->inMainMenu()) {
		if (keys[SDL_SCANCODE_RETURN]) {
			scene->play();
			scene->playBloop();
		}
		if (keys[SDL_SCANCODE_S]) {
			scene->scores();
			scene->playBloop();
		}
		if (keys[SDL_SCANCODE_C]) {
			scene->controls();
			scene->playBloop();
		}
	}
	else if (scene->inControls()) {
		if (keys[SDL_SCANCODE_BACKSPACE]) {
			scene->mainMenu();
			scene->playBloop();
		}
	}
	else if (scene->inScores()) {
		if (keys[SDL_SCANCODE_BACKSPACE]) {
			scene->mainMenu();
			scene->playBloop();
		}
	}
	else if (scene->paused()) {
		if (keys[SDL_SCANCODE_BACKSPACE]) {
			scene->mainMenu();
			scene->playBloop();
		}
	}
	else if (scene->inChooseName()) {
		if (keys[SDL_SCANCODE_BACKSPACE]) {
			scene->mainMenu();
			scene->playBloop();
		}
		if (keys[SDL_SCANCODE_SPACE]) {
			scene->chooseNameAndPlay();
			scene->playBloop();
		}
	}
	else if (scene->inGame()) {
		if (keys[SDL_SCANCODE_1]) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glDisable(GL_CULL_FACE);
		}

		if (keys[SDL_SCANCODE_2]) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glEnable(GL_CULL_FACE);
		}

		if (keys[SDL_SCANCODE_W] || keys[SDL_SCANCODE_S] || keys[SDL_SCANCODE_A] || keys[SDL_SCANCODE_D]) {
			if (keys[SDL_SCANCODE_W]) scene->movePlayerForward(0.1f);
			if (keys[SDL_SCANCODE_S]) scene->movePlayerForward(-0.1f);
			if (keys[SDL_SCANCODE_A]) scene->movePlayerRight(-0.1f);
			if (keys[SDL_SCANCODE_D]) scene->movePlayerRight(0.1f);
		}
		else {
			scene->stand();
		}
		if (keys[SDL_SCANCODE_SPACE]) {
			scene->playerJump();
		}

		scene->playerUpdate(spaceUp);
	}

}
