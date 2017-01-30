#ifndef PLAYER_H
#define PLAYER_H

#include <glm/glm.hpp>

class Player {
public:
	Player();
	glm::vec3 getPlayerPos() { return playerPos; }
private:
	glm::vec3 playerPos;
};

#endif
