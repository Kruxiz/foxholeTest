#ifndef PLAYER_H
#define PLAYER_H

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <string>
#include "GameObject.h"

enum PlayerState {
	STANDING,
	JUMPING,
	FALLING
};

class Player : public GameObject {
public:
	Player();
	GLfloat getPlayerR() { return r; }
	void setPlayerR(GLfloat newR) { r = newR; }
	void setPlayerMesh(GLuint mesh) { meshId = mesh; }
	void setPlayerTexture(GLuint tex) { textureId = tex; }
	void setPlayerScale(glm::vec3 newScale) { scale = newScale; }
	void setMeshIndexCount(GLuint newCount) { playerMeshIndexCount = newCount; }
	GLuint getMeshIndexCount() { return playerMeshIndexCount; }
	void playerJump();
	void playerStand();
	void playerFall();
	bool isPlayerJumping() { return playerState == JUMPING; }
	bool isPlayerFalling() { return playerState == FALLING; }
	bool isPlayerStanding() { return playerState == STANDING; }
	void playerReset() { position = playerRespawnPos; lastCollisionObjName = ""; }
private:
	GLfloat r = 0.0f;
	GLuint jumpCounter = 0;
	GLfloat jumpIncrement = 0.2f;
	GLfloat fallIncrement = 0.2f;
	GLuint jumpMax = 25;
	GLuint playerMeshIndexCount = 0;
	PlayerState playerState = STANDING;
	glm::vec3 playerRespawnPos;

};

#endif
