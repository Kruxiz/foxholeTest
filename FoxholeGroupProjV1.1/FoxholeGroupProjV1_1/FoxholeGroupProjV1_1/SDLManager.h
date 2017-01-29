#ifndef SDLMANAGER_H
#define SDLMANAGER_H

#include <SDL.h>
#include <gl/glew.h>
#include <stack>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Initialise.h"

class SDLManager {
private:
	SDL_Window * window;
	SDL_GLContext context;
	glm::vec3 position; // position of player - WILL NEED TO BE PUT INTO PLAYER CLASS OR SOMETHING
	glm::vec3 eye;
	glm::vec3 at;
	glm::vec3 up;
	std::stack<glm::mat4> mvStack;
	GLfloat r = 0.0f;
	void SDLDraw();
	void SDLUpdate();
	static void renderSkyBox();
public:
	SDLManager();
	void SDLInit(void);
	void SDLEnd(void);
	void SDLRun(void);
	static GLuint loadCubeMap(const char *fname[6], GLuint *texID);
	static GLuint loadBitmap(char *fname);
	static glm::vec3 moveForward(glm::vec3 pos, GLfloat angle, GLfloat d, GLfloat r);
	static glm::vec3 moveRight(glm::vec3 pos, GLfloat angle, GLfloat d, GLfloat r);
};

#endif
