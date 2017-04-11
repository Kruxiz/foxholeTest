#include "Player.h"

Player::Player()

	: GameObject("Player", glm::vec3(5.0f, 1.0f, 1.5f), glm::vec3(1.0f, 1.0f, 1.0f), NULL, NULL, 0)
{
	playerRespawnPos = position;
}

void Player::playerJump()
{
	if (jumpCounter < jumpMax) {
		playerState = JUMPING;
		jumpCounter++;
		position.y += jumpIncrement;
	}
	else if (jumpCounter >= jumpMax) {
		playerState = FALLING;
	}

}

void Player::playerStand()
{
	playerState = STANDING;
	position.y += fallIncrement;
	jumpCounter = 0;
}

void Player::playerFall()
{
	playerState = FALLING;
	position.y -= fallIncrement;
	jumpCounter = jumpMax;
}
