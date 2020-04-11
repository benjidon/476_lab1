/*
 * Final Project
 * "GEOCEAM"
 * By Benjamin Don
 */

#include <iostream>
#include <glad/glad.h>

#include "GLSL.h"
#include "Program.h"
#include "Shape.h"
#include "MatrixStack.h"
#include "WindowManager.h"
#include "stb_image.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader/tiny_obj_loader.h>

// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;
using namespace glm;


class Application : public EventCallbacks
{

public:

	WindowManager * windowManager = nullptr;

	// Our shader program
	std::shared_ptr<Program> prog;

	// Cube Prog
	std::shared_ptr<Program> cubeProg;


	// Shape to be used (from  file) - modify to support multiple
	vector<shared_ptr<Shape>> mesh;
	shared_ptr<Shape> cubeMesh;
	shared_ptr<Shape> smoothSphereMesh;
	shared_ptr<Shape> titanicMesh;

	vector<shared_ptr<Shape>> corals;

	int flockSize = 250;
	int count = 0;
	int sign = 1;


	// Cubemap Faces
	vector<std::string> faces {
	 "uw_rt.jpg",
	 "uw_lf.jpg",
	 "uw_up.jpg",
	 "uw_dn.jpg",
	 "uw_bk.jpg",
	 "uw_ft.jpg"
 	}; 

 	unsigned int cubemapTexture; 

	// Contains vertex information for OpenGL
	GLuint VertexArrayID;

	// Data necessary to give our triangle to OpenGL
	GLuint VertexBufferID;

	//example data that might be useful when trying to compute bounds on multi-shape
	vec3 gMin;
	vec3 gMax;

	//animation data
	float sTheta = 0;
	float cTheta = 0;
	float eTheta = 0;
	float hTheta = 0;
	float gTrans = 0;
	float gTransZ = 0;
	bool first = true;

	bool strafeLeft = false;
	bool strafeRight = false;
	bool dollyForward = false;
	bool dollyBackward = false;
	float dollySpeed = 0;
	float strafeSpeed = 0;

	// Different materials for the setMaterial() function
	unsigned int mat1 = 0;
	unsigned int mat2 = 1;
	unsigned int mat3 = 2;
	unsigned int mat4 = 3;
	unsigned int mat5 = 4;
	unsigned int mat6 = 5;
	unsigned int mat7 = 6;
	float radius = 1;

	glm::vec3 cameraPos = vec3(0, 0, 0);
	glm::vec3 up = vec3(0, 1, 0);

	float phi = 0;
	float theta = -1.5708;

	float scrollY = 0;
	float scrollX = 0;

	glm::vec3 wshift = vec3(0, 0, 0);
	glm::vec3 sshift = vec3(0, 0, 0);
	glm::vec3 ashift = vec3(0, 0, 0);
	glm::vec3 dshift = vec3(0, 0, 0);

	vec3 lightPos;


void resize_obj(vector<shared_ptr<Shape>> shapes){
   float minX, minY, minZ;
   float maxX, maxY, maxZ;
   float scaleX, scaleY, scaleZ;
   float shiftX, shiftY, shiftZ;
   float epsilon = 0.001;

   	minX = gMin.x;
   	minY = gMin.y;
   	minZ = gMin.z;

   	maxX = gMax.x;
   	maxY = gMax.y;
   	maxZ = gMax.z;


	//From min and max compute necessary scale and shift for each dimension
   float maxExtent, xExtent, yExtent, zExtent;
   xExtent = maxX-minX;
   yExtent = maxY-minY;
   zExtent = maxZ-minZ;
   if (xExtent >= yExtent && xExtent >= zExtent) {
      maxExtent = xExtent;
   }
   if (yExtent >= xExtent && yExtent >= zExtent) {
      maxExtent = yExtent;
   }
   if (zExtent >= xExtent && zExtent >= yExtent) {
      maxExtent = zExtent;
   }
   scaleX = 2.0 /maxExtent;
   shiftX = minX + (xExtent/ 2.0);
   scaleY = 2.0 / maxExtent;
   shiftY = minY + (yExtent / 2.0);
   scaleZ = 2.0/ maxExtent;
   shiftZ = minZ + (zExtent)/2.0;

   //Go through all verticies shift and scale them
   for (size_t i = 0; i < shapes.size(); i++) {
      for (size_t v = 0; v < shapes[i]->posBuf.size() / 3; v++) {
         shapes[i]->posBuf[3*v+0] = (shapes[i]->posBuf[3*v+0] - shiftX) * scaleX;
        // assert(shapes[i].mesh.positions[3*v+0] >= -1.0 - epsilon);
        // assert(shapes[i].mesh.positions[3*v+0] <= 1.0 + epsilon);
         shapes[i]->posBuf[3*v+1] = (shapes[i]->posBuf[3*v+1] - shiftY) * scaleY;
       //  assert(shapes[i].mesh.positions[3*v+1] >= -1.0 - epsilon);
       //  assert(shapes[i].mesh.positions[3*v+1] <= 1.0 + epsilon);
      	shapes[i]->posBuf[3*v+2] = (shapes[i]->posBuf[3*v+2] - shiftZ) * scaleZ;
       //  assert(shapes[i].mesh.positions[3*v+2] >= -1.0 - epsilon);
       //  assert(shapes[i].mesh.positions[3*v+2] <= 1.0 + epsilon);
      }
   }
}

	void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
	{
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
		}

		// Strafe Controls
		if (key == GLFW_KEY_A && action == GLFW_PRESS) {

				strafeLeft = true;
		}
		if (key == GLFW_KEY_D && action == GLFW_PRESS) {
			
				strafeRight = true;
			
		}
		if (key == GLFW_KEY_A && action == GLFW_RELEASE) {
			strafeLeft = false;
		}
		if (key == GLFW_KEY_D && action == GLFW_RELEASE) {
			strafeRight = false;
		}

		// Dolly Controls
		if (key == GLFW_KEY_W && action == GLFW_PRESS) {
			if (!dollyBackward) {
				dollyForward = true;
			}
		}
		if (key == GLFW_KEY_S && action == GLFW_PRESS) {
			if (!dollyForward) {
				dollyBackward = true;
			}
		}
		if (key == GLFW_KEY_W && action == GLFW_RELEASE) {
			dollyForward = false;
		}
		if (key == GLFW_KEY_S && action == GLFW_RELEASE) {
			dollyBackward = false;
		}


		if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
			lightPos.x -= 5.0;
		}
		if (key == GLFW_KEY_E && action == GLFW_PRESS) {
			lightPos.x += 5.0;
		}
		if (key == GLFW_KEY_R && action == GLFW_PRESS) {
			phi += 1;
		}
		if (key == GLFW_KEY_M && action == GLFW_PRESS) {
			unsigned int temp = mat1;
			mat1 = mat2;
			mat2 = mat3;
			mat3 = mat4;
			mat4 = mat5;
			mat5 = mat6;
			mat6 = mat7;
			mat7 = temp;
		}

		if (key == GLFW_KEY_Z && action == GLFW_PRESS) {
			glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
		}
		if (key == GLFW_KEY_Z && action == GLFW_RELEASE) {
			glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
		}
	}

	void mouseCallback(GLFWwindow *window, int button, int action, int mods)
	{
		double posX, posY;

		if (action == GLFW_PRESS)
		{
			 glfwGetCursorPos(window, &posX, &posY);
			 cout << "Pos X " << posX <<  " Pos Y " << posY << endl;
		}
		
	}

	void scrollCallback(GLFWwindow * window, double in_deltaX, double in_deltaY)
	{
		
		float scrollY = -1 * in_deltaY / 50;
		float scrollX = in_deltaX / 50;

		//cout << "phi: " << phi << " " << "scrollY " << scrollY << endl;
		//cout << in_deltaX << endl;
		
		if (scrollY > 0) {
			if (!((phi + scrollY) >= .82)) {
				phi += scrollY;
			}
		}

		else if (scrollY < 0) {
			if (!((phi + scrollY) <= -1.4)) {
				phi += scrollY;
			}
		}

		theta += scrollX;
	}

	void resizeCallback(GLFWwindow *window, int width, int height)
	{
		glViewport(0, 0, width, height);
	}

	void init(const std::string& resourceDirectory)
	{
		GLSL::checkVersion();

		// Set background color.
		glClearColor(.12f, .34f, .56f, 1.0f);
		// Enable z-buffer test.
		glEnable(GL_DEPTH_TEST);

		// Initialize the GLSL program.
		prog = make_shared<Program>();
		prog->setVerbose(true);
		prog->setShaderNames(resourceDirectory + "/simple_vert.glsl", resourceDirectory + "/lighting_frag.glsl");
		prog->init();
		prog->addUniform("P");
		prog->addUniform("V");
		prog->addUniform("M");
		prog->addUniform("difCol");
		prog->addUniform("lightPos");
		prog->addUniform("MatAmb");
		prog->addUniform("MatDif");
		prog->addUniform("MatSpec");
		prog->addUniform("shine");
		prog->addAttribute("vertPos");
		prog->addAttribute("vertNor");


		cubeProg = make_shared<Program>();
		cubeProg->setVerbose(true);
		cubeProg->setShaderNames(resourceDirectory + "/cube_vert.glsl", resourceDirectory + "/cube_frag.glsl");
		cubeProg->init();
		cubeProg->addUniform("P");
		cubeProg->addUniform("V");
		cubeProg->addUniform("M");
		cubeProg->addAttribute("vertPos");
		cubeProg->addAttribute("vertNor");
		cubeProg->addUniform("skybox");


		lightPos.x = -10.0;
		lightPos.y = 2.0;
		lightPos.z = 2.0;

		cubemapTexture = createSky(resourceDirectory + "/cracks/", faces); 

}


	void initGeom(const std::string& resourceDirectory)
	{

		//EXAMPLE set up to read one shape from one obj file - convert to read several
		// Initialize mesh
		// Load geometry
 		// Some obj files contain material information.We'll ignore them for this assignment.
 		vector<tinyobj::shape_t> TOshapes;
 		vector<tinyobj::material_t> objMaterials;
 		string errStr;
		//load in the mesh and make the shape(s)
 		bool rc = tinyobj::LoadObj(TOshapes, objMaterials, errStr, (resourceDirectory + "/dummy.obj").c_str());
		if (!rc) {
			cerr << errStr << endl;
		} 

		rc = tinyobj::LoadObj(TOshapes, objMaterials, errStr, (resourceDirectory + "/cube.obj").c_str());
		if (!rc) {
			cerr << errStr << endl;
		} else {
			cubeMesh = make_shared<Shape>();
			cubeMesh->createShape(TOshapes[0]);
			cubeMesh->measure();
			cubeMesh->init();
		}


		rc = tinyobj::LoadObj(TOshapes, objMaterials, errStr, (resourceDirectory + "/titanic.obj").c_str());
		if (!rc) {
			cerr << errStr << endl;
		} else {
			titanicMesh = make_shared<Shape>();
			titanicMesh->createShape(TOshapes[0]);
			titanicMesh->measure();
			titanicMesh->init();
		}


	}

	void setModel(std::shared_ptr<Program> prog, std::shared_ptr<MatrixStack>M) {
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
   }

   	void setMaterial(int i) {
 		switch (i) {
 			case 0: //shiny blue plastic
 				glUniform3f(prog->getUniform("MatAmb"), 0.02, 0.04, 0.2);
 				glUniform3f(prog->getUniform("MatDif"), 0.0, 0.16, 0.9);
 				glUniform3f(prog->getUniform("MatSpec"), 0.14, 0.2, 0.8);
 				glUniform1f(prog->getUniform("shine"), 120.0);
 				break;
 			case 1: // flat grey
 				glUniform3f(prog->getUniform("MatAmb"), 0.13, 0.13, 0.14);
 				glUniform3f(prog->getUniform("MatDif"), 0.3, 0.3, 0.4);
 				glUniform3f(prog->getUniform("MatSpec"), 0.3, 0.3, 0.4);
 				glUniform1f(prog->getUniform("shine"), 4.0);
 				break;
 			case 2: //brass
 				glUniform3f(prog->getUniform("MatAmb"), 0.3294, 0.2235, 0.02745);
 				glUniform3f(prog->getUniform("MatDif"), 0.7804, 0.5686, 0.11373);
 				glUniform3f(prog->getUniform("MatSpec"), 0.9922, 0.941176, 0.80784);
 				glUniform1f(prog->getUniform("shine"), 27.9);
				break;
			case 3: // flat grey
 				glUniform3f(prog->getUniform("MatAmb"), 0.13, 0.13, 0.14);
 				glUniform3f(prog->getUniform("MatDif"), 0.3, 0.3, 0.4);
 				glUniform3f(prog->getUniform("MatSpec"), 0.9922, 0.941176, 0.80784);
 				glUniform1f(prog->getUniform("shine"), 120.0);
 				break;

 			case 4: // flat white
 				glUniform3f(prog->getUniform("MatAmb"), 0.13, 0.13, 0.14);
 				glUniform3f(prog->getUniform("MatDif"), 0.9, 0.9, 0.9);
 				glUniform3f(prog->getUniform("MatSpec"), 0.9922, 0.941176, 0.80784);
 				glUniform1f(prog->getUniform("shine"), 120.0);
 				break;

 			case 5: // shiny black
 				glUniform3f(prog->getUniform("MatAmb"), 0.1, 0.1, 0.1);
 				glUniform3f(prog->getUniform("MatDif"), 0.1, 0.1, 0.1);
 				glUniform3f(prog->getUniform("MatSpec"), 0.1, 0.1, 0.1);
 				glUniform1f(prog->getUniform("shine"), 1.0);
 				break;

 			case 6: //sand
 				glUniform3f(prog->getUniform("MatAmb"), 0.2, .2, 0.3);
 				glUniform3f(prog->getUniform("MatDif"), 0.5, 0.7215, 0.20222);
 				glUniform3f(prog->getUniform("MatSpec"), 0.1, 0.21, 0.22184);
 				glUniform1f(prog->getUniform("shine"), 0.5);
 				break;

 			case 7: //dark brown
 				glUniform3f(prog->getUniform("MatAmb"), 0.3254, 0.2832, 0.2614);
 				glUniform3f(prog->getUniform("MatDif"), 0.34588, 0.3282, 0.3029);
 				glUniform3f(prog->getUniform("MatSpec"), 0.1, 0.1, 0.01);
 				glUniform1f(prog->getUniform("shine"), 1.0);
 				break;

 		}
}

	unsigned int createSky(string dir, vector<string> faces) {
		unsigned int textureID;
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
		int width, height, nrChannels;
		stbi_set_flip_vertically_on_load(false);
			for(GLuint i = 0; i < faces.size(); i++) {
				unsigned char *data =
				stbi_load((dir+faces[i]).c_str(), &width, &height, &nrChannels, 0);
				if (data) {
					glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
					0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
				} else {
					cout << "failed to load: " << (dir+faces[i]).c_str() << endl;
				}
			}
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			cout << " creating cube map any errors : " << glGetError() << endl;
			return textureID;
	} 

	void render() {
		// Get current frame buffer size.
		int width, height;
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
		glViewport(0, 0, width, height);

		// Clear framebuffer.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Use the matrix stack for Lab 6
		float aspect = width/(float)height;

		// Create the matrix stacks - please leave these alone for now
		auto Projection = make_shared<MatrixStack>();
		auto View = make_shared<MatrixStack>();
		auto Model = make_shared<MatrixStack>();

		// Apply perspective projection.
		Projection->pushMatrix();
		Projection->perspective(45.0f, aspect, 0.01f, 10000.0f);


		// Calculate direction angles
		float x = cos(phi) * cos(theta);
		float y = sin(phi);
		float z = cos(phi)*cos(1.57079632679 - theta);


		// Set up lookAt vectors
		vec3 direction = vec3(x, y, z);
		vec3 view = direction - cameraPos;
		view = glm::normalize(view);
		vec3 strafe = glm::cross(up, view);
		

		vec3 strafeOff = (strafe * strafeSpeed);
		vec3 dollyOff = (view * dollySpeed);


		if (strafeLeft) {
			strafeSpeed = 1;
			ashift += strafeOff;
		} 
		else if (strafeRight) {
			strafeSpeed = -1;
			dshift += strafeOff;
		}

		if (dollyForward) {
			dollySpeed = 1;
			wshift += dollyOff;
		}
		else if (dollyBackward) {
			dollySpeed = -1;
			sshift += dollyOff;
		}

		vec3 posShift = (dshift + sshift + wshift + ashift);

		//glm::mat4 lookAt = glm::lookAt(cameraPos, direction, up); 
		View->lookAt(cameraPos + posShift, direction + posShift, up);


		// View is global translation along negative z for now
		View->pushMatrix();
		Model->pushMatrix();
			Model->loadIdentity();


		// Draw a stack of cubes with indiviudal transforms
		prog->bind();
		glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, value_ptr(Projection->topMatrix()));
		glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, value_ptr(View->topMatrix()));
		
		glUniform3f(prog->getUniform("difCol"), 0.0, 0.16, 0.9);
		glUniform3f(prog->getUniform("lightPos"), lightPos.x, lightPos.y, lightPos.z);
		
		glUniform3f(prog->getUniform("MatAmb"), 0.3294, 0.2235, 0.02745);
 		glUniform3f(prog->getUniform("MatDif"), 0.7804, 0.5686, 0.11373);
 		glUniform3f(prog->getUniform("MatSpec"), 0.9922, 0.941176, 0.80784);
		glUniform1f(prog->getUniform("shine"), 127.9);



		// draw mesh 
		Model->pushMatrix();
			Model->loadIdentity();

			// Draw Ground
			setMaterial(7);
			Model->pushMatrix();
				 Model->translate(vec3(0, -20, 0));
				 Model->scale(vec3(2400, 2, 2400));
			 	setModel(prog, Model);
			  	cubeMesh->draw(prog);
			Model->popMatrix();


			// draw titanic
			setMaterial(3);
			Model->pushMatrix();
				Model->scale(vec3(10.5, 10.5, 10.5));
				Model->translate(vec3(20, -3, -50));
				Model->rotate(.8, vec3(1, 0, 0));
				setModel(prog, Model);
				titanicMesh->draw(prog);
			Model->popMatrix();

			prog->unbind();

			// Draw skybox
			Model->pushMatrix();
				Model->translate(vec3(0, 500, 0));

				Model->scale(vec3(2530, 2530, 2530));
				cubeProg->bind();
				glUniformMatrix4fv(cubeProg->getUniform("P"), 1, GL_FALSE, value_ptr(Projection->topMatrix()));
				glDepthFunc(GL_LEQUAL);
				glUniformMatrix4fv(cubeProg->getUniform("V"), 1, GL_FALSE,value_ptr(View->topMatrix()) ); 
				glUniformMatrix4fv(cubeProg->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
				glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
			
				cubeMesh->draw(cubeProg);  
				//set the depth test back to normal!
				glDepthFunc(GL_LESS);
				//unbind the shader for the skybox
				cubeProg->unbind();
			Model->popMatrix();

		Model->popMatrix();

		

		sTheta = sin(glfwGetTime());
		cTheta = cos(glfwGetTime()) / 5;
		eTheta = std::max(0.0f, (float)sin(glfwGetTime()));
		hTheta = std::max(0.0f, (float)sin(glfwGetTime()));



		// Pop matrix stacks.
		Projection->popMatrix();
		View->popMatrix();

	}
};


int main(int argc, char *argv[])
{
	// Where the resources are loaded from
	std::string resourceDir = "../resources";

	if (argc >= 2)
	{
		resourceDir = argv[1];
	}

	Application *application = new Application();

	// Your main will always include a similar set up to establish your window
	// and GL context, etc.

	WindowManager *windowManager = new WindowManager();
	windowManager->init(640, 480);
	windowManager->setEventCallbacks(application);
	application->windowManager = windowManager;

	// This is the code that will likely change program to program as you
	// may need to initialize or set up different data and state
	application->init(resourceDir);
	application->initGeom(resourceDir);
	// Loop until the user closes the window.
	while (! glfwWindowShouldClose(windowManager->getHandle()))
	{
		// Render scene.
		application->render();
		// Swap front and back buffers.
		glfwSwapBuffers(windowManager->getHandle());
		// Poll for and process events.
		glfwPollEvents();
		
	}

	// Quit program.
	windowManager->shutdown();
	return 0;
}
