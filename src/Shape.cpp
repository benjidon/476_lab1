#include "Shape.h"
#include <iostream>
#include <assert.h>

#include "GLSL.h"
#include "Program.h"

using namespace std;
  int p[512];

double fade(double t) { return t * t * t * (t * (t * 6 - 15) + 10); }
double lerp(double t, double a, double b) { return a + t * (b - a); }
double grad(int hash, double x, double y, double z) {
        int h = hash & 15;                      
        double u = h<8 ? x : y,                 
               v = h<4 ? y : h==12||h==14 ? x : z;
        return ((h&1) == 0 ? u : -u) + ((h&2) == 0 ? v : -v);
     }
   
  double noise(double x, double y, double z) {
        int X = (int)floor(x) & 255,                  
            Y = (int)floor(y) & 255,                  
            Z = (int)floor(z) & 255;
        x -= floor(x);                                
        y -= floor(y);                                
        z -= floor(z);
        double u = fade(x),                                
               v = fade(y),                                
               w = fade(z);
        int A = p[X  ]+Y, AA = p[A]+Z, AB = p[A+1]+Z,      
            B = p[X+1]+Y, BA = p[B]+Z, BB = p[B+1]+Z;   
   
        return lerp(w, lerp(v, lerp(u, grad(p[AA  ], x  , y  , z   ), 
                                       grad(p[BA  ], x-1, y  , z   )),
                               lerp(u, grad(p[AB  ], x  , y-1, z   ), 
                                       grad(p[BB  ], x-1, y-1, z   ))),
                       lerp(v, lerp(u, grad(p[AA+1], x  , y  , z-1 ), 
                                       grad(p[BA+1], x-1, y  , z-1 )), 
                               lerp(u, grad(p[AB+1], x  , y-1, z-1 ),
                                       grad(p[BB+1], x-1, y-1, z-1 ))));
     }
   
  void loadPermutation(){
  	FILE* fp = fopen("../resources/perms.txt","r");
  	int permutation[256],i;

  	if (fp == NULL) {
  		cout << "ERROR OPENING FILE" << endl;
  		return;
  	}
   
  	for(i=0;i<256;i++)
  		fscanf(fp,"%d",&permutation[i]);
   
  	fclose(fp);
   
  	for (int i=0; i < 256 ; i++) p[256+i] = p[i] = permutation[i];
  }

 double OctavePerlin(double x, double y, double z, int octaves, double persistence) {
    double total = 0;
    double frequency = 0.01;
    double amplitude = 2255;
    double maxValue = 0;  // Used for normalizing result to 0.0 - 1.0
    for(int i=0;i<octaves;i++) {
        total += noise(x * frequency, y * frequency, z * frequency) * amplitude;
        
        maxValue += amplitude;
        
        amplitude *= persistence;
        frequency *= 2;
    }
    
    return total/maxValue;
}


Shape::Shape() :
	eleBufID(0),
	posBufID(0),
	norBufID(0),
	texBufID(0), 
   vaoID(0)
{
	min = glm::vec3(0);
	max = glm::vec3(0);
	texOff = false;

	loadPermutation();
}

Shape::~Shape()
{
}

/* copy the data from the shape to this object */
void Shape::createShape(tinyobj::shape_t & shape)
{
		posBuf = shape.mesh.positions;
		norBuf = shape.mesh.normals;
		texBuf = shape.mesh.texcoords;
		eleBuf = shape.mesh.indices;

}

void Shape::measure() {
  float minX, minY, minZ;
   float maxX, maxY, maxZ;

   minX = minY = minZ = 1.1754E+38F;
   maxX = maxY = maxZ = -1.1754E+38F;

   //Go through all vertices to determine min and max of each dimension
   for (size_t v = 0; v < posBuf.size() / 3; v++) {
		if(posBuf[3*v+0] < minX) minX = posBuf[3*v+0];
		if(posBuf[3*v+0] > maxX) maxX = posBuf[3*v+0];

		if(posBuf[3*v+1] < minY) minY = posBuf[3*v+1];
		if(posBuf[3*v+1] > maxY) maxY = posBuf[3*v+1];

		if(posBuf[3*v+2] < minZ) minZ = posBuf[3*v+2];
		if(posBuf[3*v+2] > maxZ) maxZ = posBuf[3*v+2];
	}

	min.x = minX;
	min.y = minY;
	min.z = minZ;
   	max.x = maxX;
   	max.y = maxY;
   	max.z = maxZ;
}


std::vector<glm::vec3> Shape::proceduralCreate(int xSize, int zSize) {
	int i, x, z = 0;


	double amplitude = 120.0;
	double frequency = 0.005;


	std::vector<glm::vec3> coralCoords;

	int it1 = 0;
	int it2 = 0;

	for (double z = 0.0; z < zSize; z += 1.0) {
		it1 = 0;
		for (double x = 0.0; x < xSize; x += 1.0) {

			// Layering Octaves
			double y = noise(x  * frequency, 0, z * frequency) * amplitude;
			y += noise(x  * frequency, 0, z * frequency) * amplitude;
			y += noise((x + 200) * frequency*8, 0, (z + 200) * frequency*8) * 2;
			y += noise(x * frequency*10, 0, z * frequency*10) * 1.5;

			y += noise(x * frequency*20, 0, z * frequency*20) * 1.5;
			y += noise(x * frequency*5, 0, z * frequency*5) * 10.5;

			y += noise(x * frequency*100, 0, z * frequency*100) * .25;

			for (int i = 1; i < 5; i ++) {
				y += noise(((x + (i * rand() % 100)) * frequency * i), 0, ((z + (i * rand() % 100) * frequency * i))) / (i * 10);
			}

			if (y < 30 &&  z > 100 && z < zSize - 100 && x > 100 && x < 500  &&  rand() % 5000 == 50) {
				coralCoords.push_back(glm::vec3(x, y, z));
			}

			if (y < 30 &&  z > 300 && z < 400 && x > 200 && x < 300 &&  rand() % 150 == 50) {
				coralCoords.push_back(glm::vec3(x, y, z));
			}

			posBuf.push_back(x);
			posBuf.push_back(y);
			posBuf.push_back(z);

		}

	}

	for (int z = 0; z < zSize - 1; z++) {
		for (int x = 0; x < xSize - 1; x++) {
			eleBuf.push_back(x + (z*xSize));
			eleBuf.push_back(x + (z*xSize) + xSize);
			eleBuf.push_back(x + (z*xSize) + 1);

			eleBuf.push_back(x + (z*xSize) + xSize);
			eleBuf.push_back(x + (z*xSize) + xSize + 1);
			eleBuf.push_back(x + (z*xSize) + 1);

		}
	}


	return coralCoords;
}

void Shape::createNormals() {
	glm::vec3 p1;
	glm::vec3 p2;
	glm::vec3 p3;

	glm::vec3 v1;
	glm::vec3 v2;

	glm::vec3 cross;

	norBuf = posBuf;

	int index1, index2, index3 = 0;

	for (size_t i = 0; i < norBuf.size(); i++) {
		norBuf[i] = 0;
	}

	for (size_t v = 0; v < eleBuf.size(); v += 3) {

		index1 = eleBuf[v] * 3;
		p1.x = posBuf[index1 + 0];
		p1.y = posBuf[index1 + 1];
		p1.z = posBuf[index1 + 2];

		index2 = eleBuf[v + 1] * 3;
		p2.x = posBuf[index2 + 0];
		p2.y = posBuf[index2 + 1];
		p2.z = posBuf[index2 + 2];

		index3 = eleBuf[v + 2] * 3;
		p3.x = posBuf[index3 + 0];
		p3.y = posBuf[index3 + 1];
		p3.z = posBuf[index3 + 2];

		v1.x = p1.x - p2.x;
		v1.y = p1.y - p2.y;
		v1.z = p1.z - p2.z;


		v2.x = p2.x - p3.x;
		v2.y = p2.y - p3.y;
		v2.z = p2.z - p3.z;


		cross = glm::cross(v1, v2);

		norBuf[index1 + 0] = (norBuf[index1 + 0] + cross.x) / 2;
		norBuf[index1 + 1] = (norBuf[index1 + 1] + cross.y) / 2;
		norBuf[index1 + 2] = (norBuf[index1 + 2] + cross.z) / 2;

		norBuf[index2 + 0] = (norBuf[index2 + 0] + cross.x) / 2;
		norBuf[index2 + 1] = (norBuf[index2 + 1] + cross.y) / 2;
		norBuf[index2 + 2] = (norBuf[index2 + 2] + cross.z) / 2;

		norBuf[index3 + 0] = (norBuf[index3 + 0] + cross.x) / 2;
		norBuf[index3 + 1] = (norBuf[index3 + 1] + cross.y) / 2;
		norBuf[index3 + 2] = (norBuf[index3 + 2] + cross.z) / 2;
	}

}

void Shape::init()
{
   // Initialize the vertex array object
   glGenVertexArrays(1, &vaoID);
   glBindVertexArray(vaoID);

	// Send the position array to the GPU
	glGenBuffers(1, &posBufID);
	glBindBuffer(GL_ARRAY_BUFFER, posBufID);
	glBufferData(GL_ARRAY_BUFFER, posBuf.size()*sizeof(float), &posBuf[0], GL_STATIC_DRAW);
	
	// Send the normal array to the GPU
	if(norBuf.empty()) {
		//create normals here
		norBufID = 0;
		createNormals();
		glGenBuffers(1, &norBufID);
		glBindBuffer(GL_ARRAY_BUFFER, norBufID);
		glBufferData(GL_ARRAY_BUFFER, norBuf.size()*sizeof(float), &norBuf[0], GL_STATIC_DRAW);
	} else {
		glGenBuffers(1, &norBufID);
		glBindBuffer(GL_ARRAY_BUFFER, norBufID);
		glBufferData(GL_ARRAY_BUFFER, norBuf.size()*sizeof(float), &norBuf[0], GL_STATIC_DRAW);
	}

	
	// Send the texture array to the GPU
	if(texBuf.empty()) {
		texBufID = 0;
	} else {
		glGenBuffers(1, &texBufID);
		glBindBuffer(GL_ARRAY_BUFFER, texBufID);
		glBufferData(GL_ARRAY_BUFFER, texBuf.size()*sizeof(float), &texBuf[0], GL_STATIC_DRAW);
	}
	
	// Send the element array to the GPU
	glGenBuffers(1, &eleBufID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eleBufID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, eleBuf.size()*sizeof(unsigned int), &eleBuf[0], GL_STATIC_DRAW);
	
	// Unbind the arrays
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	
	assert(glGetError() == GL_NO_ERROR);
}

void Shape::draw(const shared_ptr<Program> prog) const
{
	int h_pos, h_nor, h_tex;
	h_pos = h_nor = h_tex = -1;

   	//glBindVertexArray(vaoID);
	CHECKED_GL_CALL(glBindVertexArray(vaoID));


	// Bind position buffer
	h_pos = prog->getAttribute("vertPos");
	GLSL::enableVertexAttribArray(h_pos);
	glBindBuffer(GL_ARRAY_BUFFER, posBufID);
	glVertexAttribPointer(h_pos, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0);
	
	// Bind normal buffer
	h_nor = prog->getAttribute("vertNor");
	if(h_nor != -1 && norBufID != 0) {
		GLSL::enableVertexAttribArray(h_nor);
		glBindBuffer(GL_ARRAY_BUFFER, norBufID);
		glVertexAttribPointer(h_nor, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0);
	}

	if (texBufID != 0) {	
		// Bind texcoords buffer
		h_tex = prog->getAttribute("vertTex");
		if(h_tex != -1 && texBufID != 0) {
			GLSL::enableVertexAttribArray(h_tex);
			glBindBuffer(GL_ARRAY_BUFFER, texBufID);
			glVertexAttribPointer(h_tex, 2, GL_FLOAT, GL_FALSE, 0, (const void *)0);
		}
	}
	
	// Bind element buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eleBufID);
	
	// Draw
	glDrawElements(GL_TRIANGLES, (int)eleBuf.size(), GL_UNSIGNED_INT, (const void *)0);
	
	// Disable and unbind
	if(h_tex != -1) {
		GLSL::disableVertexAttribArray(h_tex);
	}
	if(h_nor != -1) {
		GLSL::disableVertexAttribArray(h_nor);
	}
	GLSL::disableVertexAttribArray(h_pos);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
