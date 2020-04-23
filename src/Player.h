#ifndef PLAYER_H
#define PLAYER_H

#include "GameObject.h"
#include <glm/gtc/type_ptr.hpp>
#include <vector>

class Player
{
public:
	Player();
	void getUpdate(bool sprinting, int boardSize);

	float phi;
	float theta;

	bool strafeLeft;
	bool strafeRight;
	bool dollyForward;
	bool dollyBackward;
	float dollySpeed;
	float strafeSpeed;
	float height;

	float viewBob;

	glm::vec3 wshift;
	glm::vec3 sshift;
	glm::vec3 ashift;
	glm::vec3 dshift;
	glm::vec3 up;
	glm::vec3 cameraPos;
	glm::vec3 posShift;
	glm::vec3 direction;

};


#endif