#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <glm/gtc/type_ptr.hpp>

class GameObject
{
public:
	GameObject();
	glm::vec3 getPosition();
	glm::vec3 getVelocity();
	void setPosition(glm::vec3 newPos);
	void setVelocity(glm::vec3 newVelocity);

private:
	glm::vec3 position;
	glm::vec3 velocity;
};


#endif