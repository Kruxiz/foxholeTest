#ifndef PLAYER_H
#define PLAYER_H

#include <glm/glm.hpp>
#include <GL/glew.h>

class Player {
public:
	Player();
	glm::vec3 getPlayerPos() { return playerPos; }
	void setPlayerPos(glm::vec3 newPos) { playerPos = newPos; }
	GLfloat getPlayerR() { return r; }
	void setPlayerR(GLfloat newR) { r = newR; }
private:
	glm::vec3 playerPos;
	GLfloat r;
};

#endif
