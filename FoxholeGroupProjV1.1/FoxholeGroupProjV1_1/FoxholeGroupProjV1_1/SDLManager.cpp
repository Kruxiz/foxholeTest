
#include "SDLManager.h"

SDLManager::SDLManager() {
	window = nullptr;
	context = 0;
};

void SDLManager::SDLInit()
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
	window = SDL_CreateWindow("SDL/GLM/OpenGL Demo", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	if (!window) // Check window was created OK
		rt3d::exitFatalError("Unable to create window");

	context = SDL_GL_CreateContext(window); // Create opengl context and attach to window
	SDL_GL_SetSwapInterval(1); // set swap buffers to sync with monitor's vertical refresh rate

	GLEWManager::GLEWInitialise();

	scene = new SceneManager();

	scene->init();
}

void SDLManager::SDLEnd(void)
{
	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
	scene->freeBass();
	delete scene;
	SDL_Quit();
}

void SDLManager::SDLRun(void)
{
	bool running = true; // set running to true
	SDL_Event sdlEvent;  // variable to detect SDL events
	while (running) {	// the event loop
		while (SDL_PollEvent(&sdlEvent)) {
			if (sdlEvent.type == SDL_QUIT || sdlEvent.type == SDL_KEYDOWN && sdlEvent.key.keysym.sym == SDLK_ESCAPE)
				running = false;

			if (/*keys[SDL_SCANCODE_TAB] *//*&& !scene->paused()*/sdlEvent.type == SDL_KEYUP && sdlEvent.key.keysym.sym == SDLK_TAB) {
				scene->togglePause();
			}

			if (sdlEvent.type == SDL_MOUSEMOTION && !scene->paused())
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


		}
		SDLUpdate(sdlEvent);
		SDLDraw();
	}

}

GLuint SDLManager::loadCubeMap(const char * fname[6], GLuint * texID)
{
	glGenTextures(1, texID); // generate texture ID
	GLenum sides[6] = { GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
		GL_TEXTURE_CUBE_MAP_POSITIVE_X,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Y };
	SDL_Surface *tmpSurface;

	glBindTexture(GL_TEXTURE_CUBE_MAP, *texID); // bind texture and set parameters
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	GLuint externalFormat;
	for (int i = 0;i < 6;i++)
	{
		// load file - using core SDL library
		tmpSurface = SDL_LoadBMP(fname[i]);
		if (!tmpSurface)
		{
			std::cout << "Error loading bitmap" << std::endl;
			return *texID;
		}

		// skybox textures should not have alpha (assuming this is true!)
		SDL_PixelFormat *format = tmpSurface->format;
		externalFormat = (format->Rmask < format->Bmask) ? GL_RGB : GL_BGR;

		glTexImage2D(sides[i], 0, GL_RGB, tmpSurface->w, tmpSurface->h, 0,
			externalFormat, GL_UNSIGNED_BYTE, tmpSurface->pixels);
		// texture loaded, free the temporary buffer
		SDL_FreeSurface(tmpSurface);
	}
	return *texID;	// return value of texure ID, redundant really
}

GLuint SDLManager::loadBitmap(char * fname)
{
	GLuint texID;
	glGenTextures(1, &texID); // generate texture ID

							  // load file - using core SDL library
	SDL_Surface *tmpSurface;
	tmpSurface = SDL_LoadBMP(fname);
	if (!tmpSurface) {
		std::cout << "Error loading bitmap" << std::endl;
	}

	// bind texture and set parameters
	glBindTexture(GL_TEXTURE_2D, texID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	SDL_PixelFormat *format = tmpSurface->format;

	GLuint externalFormat, internalFormat;
	if (format->Amask) {
		internalFormat = GL_RGBA;
		externalFormat = (format->Rmask < format->Bmask) ? GL_RGBA : GL_BGRA;
	}
	else {
		internalFormat = GL_RGB;
		externalFormat = (format->Rmask < format->Bmask) ? GL_RGB : GL_BGR;
	}

	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, tmpSurface->w, tmpSurface->h, 0,
		externalFormat, GL_UNSIGNED_BYTE, tmpSurface->pixels);
	glGenerateMipmap(GL_TEXTURE_2D);

	SDL_FreeSurface(tmpSurface); // texture loaded, free the temporary buffer
	return texID;	// return value of texture ID
}

void SDLManager::SDLDraw()
{
	//call scene manager

	scene->clearScreen();

	glm::mat4 projection = scene->initRendering();

	scene->renderSkybox(projection);

	scene->setShaderProjection(projection);

	//set lights
	scene->setLights();

	//render objects
	scene->renderObjects();

	scene->popMatrixStack();

	glDepthMask(GL_TRUE);

	SDL_GL_SwapWindow(window); // swap buffers
}



void SDLManager::SDLUpdate(SDL_Event sdlEvent)
{
	const Uint8 *keys = SDL_GetKeyboardState(NULL);

	if (!scene->paused()) {
		if (keys[SDL_SCANCODE_1]) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glDisable(GL_CULL_FACE);
		}

		if (keys[SDL_SCANCODE_2]) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glEnable(GL_CULL_FACE);
		}

		//if using <> - update playerR through scene->updatePlayerR();

		if (keys[SDL_SCANCODE_COMMA]) scene->updatePlayerR(0.50f);
		if (keys[SDL_SCANCODE_PERIOD]) scene->updatePlayerR(-0.50f);

		if (keys[SDL_SCANCODE_W]) scene->movePlayerForward(0.1f);
		if (keys[SDL_SCANCODE_S]) scene->movePlayerForward(-0.1f);
		if (keys[SDL_SCANCODE_A]) scene->movePlayerRight(-0.1f);
		if (keys[SDL_SCANCODE_D]) scene->movePlayerRight(0.1f);

		if (keys[SDL_SCANCODE_SPACE]) {
			scene->playerJump();
		}

		if (sdlEvent.type == SDL_KEYUP && sdlEvent.key.keysym.sym == SDLK_SPACE) {
			scene->setPlayerJumpFalse();
		}

		if (keys[SDL_SCANCODE_R]) scene->respawnPlayer();

		scene->playerFall();
		scene->checkPlayerRespawn();
		scene->detectCollectableCollision();
		scene->checkSwitchLevel();
	}

}
