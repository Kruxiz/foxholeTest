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
	GLfloat getBaseJumpIncrement() { return baseJumpAndFallIncrement; }
	GLfloat getMaxJumpIncrement() { return maxJumpIncrement; }
	GLfloat getMaxFallIncrement() { return maxFallIncrement; }
	GLuint getJumpMax() { return jumpMax; }
	GLfloat getAccnValue() { return accnValue; } // needed?
	bool isFalling() { return falling; }
	void fall(bool newBool) { falling = newBool; }
	bool isJumping() { return jumping; }
	void jump(bool newBool) { jumping = newBool; }
	bool isJumpIncrementSet() { return jumpIncrementSet; }
	GLuint getJumpCounter() { return jumpCounter; }
	void incrementJumpCounter() { jumpCounter++; }
	void decrementJumpCounter() { jumpCounter--; } // needed?
	void resetJumpCounter() { jumpCounter = 0; }
	GLfloat getJumpIncrement() { return jumpIncrement; }
	void increaseJumpIncrement() { jumpIncrement *= accnValue; }
	void resetJumpIncrement() { jumpIncrement = baseJumpAndFallIncrement; }
	GLfloat getFallIncrement() { return fallIncrement; }
	void increaseFallIncrement() { fallIncrement *= accnValue; }
	void resetFallIncrement() { fallIncrement = baseJumpAndFallIncrement; }
	void setIsOnObj(bool onObj) { isOnObj = onObj; }
	bool isOnObject() { return isOnObj; }
	void maxJumpCounter() { jumpCounter = jumpMax; }
private:
	GLfloat r = 0.0f;
	GLuint jumpCounter = 0;
	GLfloat jumpIncrement = 0.1f;
	GLfloat fallIncrement = 0.1f;
	GLfloat baseJumpAndFallIncrement = 0.1f;
	GLfloat maxJumpIncrement = 0.5f;
	GLfloat maxFallIncrement = 0.5f;
	GLuint jumpMax = 50;
	GLfloat accnValue = 1.1f; // needed
	bool falling = false;
	bool jumping = false;
	bool jumpIncrementSet = false;
	bool isOnObj = false;
};

#endif
