#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include "GameObject.h"
#include <glm/gtc/type_ptr.hpp>
#include <vector>

class GameManager
{
public:
	GameManager();
	GameManager(int objectCount, int boardLength);
	void init();
	void update(double dt, glm::vec3 playerPos);
	std::vector<GameObject> getGameObjects();
	int getBoardsize();

private:
	int numObjects;
	int boardSize;
	int goombaCount;
	int score;
	bool start;
	bool won;
	float objectRadius;
	float timeCounter;
	void initGameObjects();
	void createGameObject();
	void updateGameObjects(double dt, glm::vec3 playerPos);
	bool playerCollision(GameObject currObj, glm::vec3 playerPos, double radius);
	glm::vec3 boundaryCollision(glm::vec3 currPos, glm::vec3 currVelocity, double radius);
	glm::vec3 objectCollisions(GameObject currObj, glm::vec3 currVelocity);
	glm::vec3 getEmptySpace(int sign, int radius);
	std::vector<GameObject> gameObjects;
};


#endif