#pragma once
#ifndef _SHAPE_H_
#define _SHAPE_H_

#include <string>
#include <vector>
#include <memory>
#include <glm/gtc/type_ptr.hpp>
#include <tiny_obj_loader/tiny_obj_loader.h>

class Program;

class Shape
{
public:
	Shape();
	virtual ~Shape();
	void createShape(tinyobj::shape_t & shape);
	std::vector<glm::vec3> proceduralCreate(int xSize, int zSize);
	void init();
    void createNormals();
	void measure();
	void draw(const std::shared_ptr<Program> prog) const;
    std::vector<float> posBuf;
    std::vector<unsigned int> eleBuf;
    std::vector<float> norBuf;
    std::vector<float> texBuf;
	glm::vec3 min;
	glm::vec3 max;
	
private:
	unsigned eleBufID;
	unsigned posBufID;
	unsigned norBufID;
	unsigned texBufID;
   unsigned vaoID;
	bool texOff;
};

#endif
