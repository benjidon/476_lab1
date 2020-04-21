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
	void update(double dt);
	std::vector<GameObject> getGameObjects();
	int getBoardsize();

private:
	int numObjects;
	int boardSize;
	float objectRadius;
	void initGameObjects();
	void updateGameObjects(double dt);
	glm::vec3 boundaryCollision(glm::vec3 currPos, glm::vec3 currVelocity, double radius);
	glm::vec3 objectCollisions(GameObject currObj, glm::vec3 currVelocity);
	std::vector<GameObject> gameObjects;
};


#endif