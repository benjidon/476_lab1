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

void Player::getUpdate(bool sprinting, int boardSize)
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

	int boundary = (boardSize / 2) - 3;


	if (strafeLeft)
	{
		vec3 newPos = posShift + strafe;
		if (abs(newPos.x) < boundary && abs(newPos.z) < boundary) {
			strafeSpeed = 1;
			if (sprinting) {
				strafeSpeed *= 2;
			}
			ashift += strafeOff;
			viewBob += .1;
		}
	}
	else if (strafeRight)
	{
		vec3 newPos = posShift - strafe;
		if (abs(newPos.x) < boundary && abs(newPos.z) < boundary) {
			strafeSpeed = -1;
			if (sprinting) {
				strafeSpeed *= 2;
			}
			dshift += strafeOff;
			viewBob += .1;
		}
	}

	if (dollyForward)
	{
		vec3 newPos = posShift + view;
		if (abs(newPos.x) < boundary && abs(newPos.z) < boundary) {
			dollySpeed = 1;
			if (sprinting) {
				dollySpeed *= 2;
			}
			wshift += dollyOff;
			viewBob += .1;
		}
	}
	else if (dollyBackward)
	{
		vec3 newPos = posShift - view;
		if (abs(newPos.x) < boundary && abs(newPos.z) < boundary) {
			dollySpeed = -1;
			if (sprinting) {
				dollySpeed *= 2;
			}			
			sshift += dollyOff;
			viewBob += .1;
		}
	}
	//cout << wshift.x << " " << wshift.y << " " << wshift.z << endl;
	posShift = (dshift + sshift + wshift + ashift);
	posShift.y += this->height;
	// if (posShift.x >= boundary) {
	// 	posShift.x = boundary - 3;
	// }
	// if (posShift.z >= boundary) {
	// 	posShift.z = boundary - 5;
	// }
	// if (posShift.x <= -boundary) {
	// 	posShift.x = -(boundary - 5);
	// }
	// if (posShift.z <= -boundary) {
	// 	posShift.z = -(boundary - 5);
	// }



}