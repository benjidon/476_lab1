#include "GameManager.h"
#include <iostream>

using namespace std;

GameManager::GameManager()
{
}


void GameManager::init() 
{
	cout << "Initializing!!" << endl;
}


std::vector<GameObject> GameManager::getGameObjects() {
	return this->gameObjects;
}
