#include "GameObject.h"

GameObject::GameObject()
{
}


glm::vec3 GameObject::getPosition() {
	return this->position;
}

glm::vec3 GameObject::getVelocity() {
	return this->position;
}

void GameObject::setPosition(glm::vec3 newPos) {
	this->position = newPos;
}

void GameObject::setVelocity(glm::vec3 newVelocity) {
	this->velocity = newVelocity;
}
