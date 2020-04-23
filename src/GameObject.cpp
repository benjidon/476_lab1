#include "GameObject.h" 

GameObject::GameObject()
{
	dead = false;
	deathScale = 1.0;
}


glm::vec3 GameObject::getPosition() {
	return this->position;
}

glm::vec3 GameObject::getVelocity() {
	return this->velocity;
}

double GameObject::getRadius() {
	return this->radius;
}

void GameObject::setPosition(glm::vec3 newPos) {
	this->position = newPos;
}

void GameObject::setVelocity(glm::vec3 newVelocity) {
	this->velocity = newVelocity;
}

void GameObject::setRadius(double radius) {
	this->radius = radius;
}
