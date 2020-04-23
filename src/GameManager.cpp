#include "GameManager.h"
#include <iostream>


using namespace std;
using namespace glm;

GameManager::GameManager(int objectCount, int boardLength) 
{
	numObjects = objectCount;
	boardSize = boardLength;
	timeCounter = 0;
}


GameManager::GameManager()
{
	numObjects = 20;
	boardSize = 50;
	objectRadius = 10;
	timeCounter = 0;
}


void GameManager::init() 
{
	this->initGameObjects();
}


void GameManager::update(double dt, vec3 playerPos)  
{
	this->updateGameObjects(dt, playerPos);
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

bool GameManager::playerCollision(GameObject currObj, vec3 playerPos, double radius) 
{
	vec3 currPos = currObj.getPosition();
	playerPos.y = 13;
	if (glm::distance(currPos, playerPos) < currObj.getRadius() * 2) 
	{
		return true;
	}
		
	
	return false;
}

void GameManager::updateGameObjects(double dt, vec3 playerPos)
{

	if (!start) {
		cout << "Game Start!" << endl;
		cout << "------------" << endl;
		cout << "GOOMBAS LEFT: " << this->goombaCount << endl;
		cout << "SCORE: " << this->score << endl;
		cout << "------------" << endl;
		start = true;
	}

	if (this->goombaCount == 0 && !this->won) {
		cout << "------------" << endl;
		cout << "YOU WON!!! CONGRATS!!!" << endl;
		cout << "------------" << endl;
		this->won = true;
	}

	vector<GameObject> updatedObjects;
	timeCounter += dt;
	if (timeCounter > 5 && !this->won) {
		this->createGameObject();
		timeCounter = 0;
		cout << "------------" << endl;		
		cout << "You're too slow, adding another Goomba!" << endl;
		this->goombaCount += 1;
		cout << "GOOMBAS LEFT: " << this->goombaCount << endl;
		cout << "Score: " << this->score << endl;
		cout << "------------" << endl;
	}
	//cout << "updating" << endl;
	for (int i = 0; i < this->gameObjects.size(); i++) {
		GameObject gameObj = this->gameObjects[i];
		vec3 currPos = gameObj.getPosition();
		vec3 currVelocity = gameObj.getVelocity();
		double radius = gameObj.getRadius();

		if (this->playerCollision(gameObj, playerPos, radius) && !gameObj.dead) {
			gameObj.dead = true;
			this->goombaCount -= 1;
			score += 1;
			cout << "------------" << endl;
			cout << "Got one!!!" << endl;
			cout << "GOOMBAS LEFT: " << this->goombaCount << endl;
			cout << "Score: " << this->score << endl;
			cout << "------------" << endl;		
		}


		if (gameObj.dead) {
			if (gameObj.deathScale >= 0) {
				gameObj.deathScale -= .01;
				updatedObjects.push_back(gameObj);
			}
		}
		else {
			currVelocity = this->boundaryCollision(currPos, currVelocity, radius);
			currVelocity = this->objectCollisions(gameObj, currVelocity);
	
			gameObj.setVelocity(currVelocity);
			gameObj.setPosition(currPos +  (float)dt * 50 * currVelocity);
			updatedObjects.push_back(gameObj);
		}
	}
	this->gameObjects = updatedObjects;

}

vec3 GameManager::getEmptySpace(int sign, int radius) {
	
	vec3 position = vec3(sign * rand() % boardSize / 2.5, 17, sign * rand() % boardSize / 2.5);

	if (gameObjects.size() < 2) {
		return position;
	}
	for (int i = 0; i < this->gameObjects.size(); i++) {
		GameObject other = gameObjects[i];
		if (distance(position, other.getPosition()) < radius * 2) {
			return this->getEmptySpace(sign, radius);
		}
	}
	return position;
}

void GameManager::createGameObject() {
	int sign = 1;
	int radius = 8;
	if (rand() % 10 < 5) {
		sign = -1;
	}
	GameObject *gameObj = new GameObject();
	gameObj->setRadius(radius);
	vec3 position = getEmptySpace(sign, radius);	
	double vx = sign * (float)(rand() % 100) / 100; 	
	double vz = sign * (float)(rand() % 100) / 100; 	
	vec3 velocity = vec3(vx, 0, vz);	
	//cout << velocity.x << " " << velocity.z << endl;	
	gameObj->setPosition(position * (float).75);	
	gameObj->setVelocity(velocity);
	gameObjects.push_back(*gameObj);
}

void GameManager::initGameObjects() 
{
	std::vector<GameObject> objs;
	for (int i = 0; i < this->numObjects; i++) {
		createGameObject();
	}
	this->goombaCount = numObjects;
	this->score = 0;
	this->start = false;
	this->won = false;
}


std::vector<GameObject> GameManager::getGameObjects() 
{
	return this->gameObjects;
}

int GameManager::getBoardsize() 
{
	return this->boardSize;
}








