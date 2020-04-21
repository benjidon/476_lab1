#include "Player.h"
#include <iostream>

using namespace std;
using namespace glm;

Player::Player() 
{
	phi = 0.0;
	theta = -1.5708;
	wshift = vec3(0, 0, 0);
	sshift = vec3(0, 0, 0);
	ashift = vec3(0, 0, 0);
	dshift = vec3(0, 0, 0);
	up = vec3(0, 1, 0);
	cameraPos = vec3(0, 0, 0);

	strafeLeft = false;
	strafeRight = false;
	dollyForward = false;
	dollyBackward = false;
	dollySpeed = 20;
	strafeSpeed = 20;
	posShift = vec3(0, 0, 0);
	direction = vec3(0, 1, 0);
	height = 40;
	viewBob = 0;
}

void Player::getUpdate()
{
	// Calculate direction angles
	float x = cos(this->phi) * cos(this->theta);
	float y = sin(this->phi);
	float z = cos(this->phi) * cos(1.57079632679 - this->theta);

	direction = vec3(x, y, z);
	vec3 view = direction - cameraPos;
	
	view = glm::normalize(view);
	vec3 strafe = glm::cross(up, view);
		
	vec3 strafeOff = (strafe * strafeSpeed);
	vec3 dollyOff = (view * dollySpeed);

	dollyOff.y = 0;

	if (strafeLeft)
	{
		strafeSpeed = 1;
		ashift += strafeOff;
		viewBob += .1;
	}
	else if (strafeRight)
	{
		strafeSpeed = -1;
		dshift += strafeOff;
		viewBob += .1;
	}

	if (dollyForward)
	{
		dollySpeed = 1;
		wshift += dollyOff;
		viewBob += .1;
	}
	else if (dollyBackward)
	{
		dollySpeed = -1;
		sshift += dollyOff;
		viewBob += .1;
	}
	posShift = (dshift + sshift + wshift + ashift);
	posShift.y += this->height;
}