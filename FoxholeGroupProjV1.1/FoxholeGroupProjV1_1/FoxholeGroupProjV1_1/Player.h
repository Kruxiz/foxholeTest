#ifndef PLAYER_H
#define PLAYER_H

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <string>
#include "GameObject.h"

class Player : public GameObject {
public:
	Player();
	GLfloat getPlayerR() { return r; }
	void setPlayerR(GLfloat newR) { r = newR; }
	void setPlayerMesh(GLuint mesh) { meshId = mesh; }
	void setPlayerTexture(GLuint tex) { textureId = tex; }
private:
	GLfloat r;
};

#endif
