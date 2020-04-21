#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <glm/gtc/type_ptr.hpp>

class GameObject
{
public:
	GameObject();
	glm::vec3 getPosition();
	glm::vec3 getVelocity();
	double getRadius();
	void setPosition(glm::vec3 newPos);
	void setVelocity(glm::vec3 newVelocity);
	void setRadius(double radius);

private:
	glm::vec3 position;
	glm::vec3 velocity;
	double radius;
};


#endif