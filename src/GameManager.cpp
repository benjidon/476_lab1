#include "GameManager.h"
#include <iostream>


using namespace std;
using namespace glm;

GameManager::GameManager(int objectCount, int boardLength) 
{
	numObjects = objectCount;
	boardSize = 800;
}


GameManager::GameManager()
{
	numObjects = 20;
	boardSize = 900;
}


void GameManager::init() 
{
	this->initGameObjects();
}


void GameManager::update()  
{
	this->updateGameObjects();
}

vec3 GameManager::boundaryCollision(vec3 currPos, vec3 currVelocity) 
{
	if (abs(currPos.x) >= boardSize / 2)
	{
		currVelocity.x *= -1;
	}

	if (abs(currPos.z) >= boardSize / 2) 
	{
		currVelocity.z *= -1;
	}

	return currVelocity;
}

void GameManager::updateGameObjects()
{
	vector<GameObject> updatedObjects;
	//cout << "updating" << endl;
	for (int i = 0; i < this->numObjects; i++) {
		GameObject gameObj = this->gameObjects[i];
		vec3 currPos = gameObj.getPosition();
		vec3 currVelocity = gameObj.getVelocity();
	
		currVelocity = this->boundaryCollision(currPos, currVelocity);

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
		if (rand() % 10 < 5) {
			sign = -1;
		}
		GameObject *gameObj = new GameObject();
		vec3 position = vec3(sign * rand() % boardSize / 2, 4, sign * rand() % boardSize / 2);
		vec3 velocity = vec3(.5 + sign * rand() % 5, 0, .5 + sign * rand() % 2 % 100 / 100);
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



























