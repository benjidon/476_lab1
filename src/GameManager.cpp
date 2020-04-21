#include "GameManager.h"
#include <iostream>


using namespace std;
using namespace glm;

GameManager::GameManager(int objectCount, int boardLength) 
{
	numObjects = objectCount;
	boardSize = boardLength;
}


GameManager::GameManager()
{
	numObjects = 20;
	boardSize = 50;
	objectRadius = 10;
}


void GameManager::init() 
{
	this->initGameObjects();
}


void GameManager::update(double dt)  
{
	this->updateGameObjects(dt);
}

vec3 GameManager::boundaryCollision(vec3 currPos, vec3 currVelocity, double radius) 
{
	if (abs(currPos.x) + radius >= boardSize / 2)
	{
		currVelocity.x *= -1;
	}

	if (abs(currPos.z) + radius >= boardSize / 2) 
	{
		currVelocity.z *= -1;
	}

	return currVelocity;
}

vec3 GameManager::objectCollisions(GameObject currObj, vec3 currVelocity) 
{
	vec3 currPos = currObj.getPosition();
	for (int i = 0; i < this->gameObjects.size(); i++) {
		GameObject other = this->gameObjects[i];
		vec3 otherPos = other.getPosition();

		if (currPos != otherPos  && 
			glm::distance(currPos, otherPos) < currObj.getRadius() * 2) 
		{
			vec3 otherVelocity = other.getVelocity();
			double dotProd = glm::dot(currVelocity, otherVelocity);
			double dotTheta = dotProd / (glm::length(currVelocity) * glm::length(otherVelocity));
			double angleDiff = acos(dotTheta);
			if (abs(angleDiff) < .2) {
				currVelocity.x *= -1;
			}
			currVelocity.x *= -1;
			currVelocity.z *= -1;
		}
	}
	return currVelocity;
}

void GameManager::updateGameObjects(double dt)
{
	vector<GameObject> updatedObjects;
	//cout << "updating" << endl;
	for (int i = 0; i < this->numObjects; i++) {
		GameObject gameObj = this->gameObjects[i];
		vec3 currPos = gameObj.getPosition();
		vec3 currVelocity = gameObj.getVelocity();
		double radius = gameObj.getRadius();


		// currVelocity.x /= dt;
		// currVelocity.z /= dt;
	
		currVelocity = this->boundaryCollision(currPos, currVelocity, radius);
		currVelocity = this->objectCollisions(gameObj, currVelocity);
		//cout << currVelocity.x << " " << currVelocity.z << endl;


		gameObj.setVelocity(currVelocity);
		gameObj.setPosition(currPos + currVelocity);
		updatedObjects.push_back(gameObj);
	}
	this->gameObjects = updatedObjects;

}

void GameManager::initGameObjects() 
{
	std::vector<GameObject> objs;
	for (int i = 0; i < this->numObjects; i++) {
		int sign = 1;
		int radius = 8;
		if (rand() % 10 < 5) {
			sign = -1;
		}
		GameObject *gameObj = new GameObject();
		gameObj->setRadius(radius);
		vec3 position = vec3(sign * rand() % boardSize / 2.5, 13, sign * rand() % boardSize / 2.5);


		double vx = sign * (float)(rand() % 100) / 100; 
		double vz = sign * (float)(rand() % 100) / 100; 

		vec3 velocity = vec3(vx, 0, vz);
		//cout << velocity.x << " " << velocity.z << endl;

		gameObj->setPosition(position);
		gameObj->setVelocity(velocity);
		gameObjects.push_back(*gameObj);
	}
}



std::vector<GameObject> GameManager::getGameObjects() 
{
	return this->gameObjects;
}

int GameManager::getBoardsize() 
{
	return this->boardSize;
}



























