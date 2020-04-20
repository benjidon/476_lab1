#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include "GameObject.h"
#include <glm/gtc/type_ptr.hpp>
#include <vector>

class GameManager
{
public:
	GameManager();
	void init();
	std::vector<GameObject> getGameObjects();

private:
	std::vector<GameObject> gameObjects;
};


#endif