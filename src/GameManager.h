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
	void update();
	std::vector<GameObject> getGameObjects();
	int getBoardsize();

private:
	int numObjects;
	int boardSize;
	void initGameObjects();
	void updateGameObjects();
	glm::vec3 boundaryCollision(glm::vec3 currPos, glm::vec3 currVelocity);
	std::vector<GameObject> gameObjects;
};


#endif