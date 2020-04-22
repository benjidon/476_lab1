/*
 * CPE 476 - Lab 1
 * By Benjamin Don and Kyle Chin
 */

#include <iostream>
#include <glad/glad.h>
#include <algorithm>

#include "GLSL.h"
#include "Program.h"
#include "Shape.h"
#include "Texture.h"
#include "MatrixStack.h"
#include "WindowManager.h"
#include "stb_image.h"
#include "GameManager.h"
#include "Player.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader/tiny_obj_loader.h>

// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define PI 3.14159265358979

using namespace std;
using namespace glm;

class Application : public EventCallbacks
{

public:
	WindowManager *windowManager = nullptr;

	// Initialize GameManager Object
	GameManager *gameManager = new GameManager(10, 600);

	// Initialize Player Object 
	Player *player = new Player();

	// Our shader program
	std::shared_ptr<Program> prog;

	// Texture program
	std::shared_ptr<Program> texProg;
	
	// Cube Prog
	std::shared_ptr<Program> cubeProg;

	// Shape to be used (from  file) - modify to support multiple
	vector<shared_ptr<Shape>> mesh;
	shared_ptr<Shape> cubeMesh;
	shared_ptr<Shape> smoothSphereMesh;
	shared_ptr<Shape> groundMesh;
	shared_ptr<Shape> goombaMesh;



	vector<shared_ptr<Shape>> corals;

	shared_ptr<Texture> texture0;
	shared_ptr<Texture> texture1;


	int sign = 1;

	// Cubemap Faces
	vector<std::string> faces{
		"cs_rt.png",
		"cs_lf.png",
		"cs_up.png",
		"cs_dn.png",
		"cs_bk.png",
		"cs_ft.png"};

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
	float dollySpeed = 20;
	float strafeSpeed = 20;

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

	glm::vec3 wshift = vec3(0, 0, 0);
	glm::vec3 sshift = vec3(0, 0, 0);
	glm::vec3 ashift = vec3(0, 0, 0);
	glm::vec3 dshift = vec3(0, 0, 0);

	vec3 lightPos;

	void resize_obj(vector<shared_ptr<Shape>> shapes)
	{
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
		xExtent = maxX - minX;
		yExtent = maxY - minY;
		zExtent = maxZ - minZ;
		if (xExtent >= yExtent && xExtent >= zExtent)
		{
			maxExtent = xExtent;
		}
		if (yExtent >= xExtent && yExtent >= zExtent)
		{
			maxExtent = yExtent;
		}
		if (zExtent >= xExtent && zExtent >= yExtent)
		{
			maxExtent = zExtent;
		}
		scaleX = 2.0 / maxExtent;
		shiftX = minX + (xExtent / 2.0);
		scaleY = 2.0 / maxExtent;
		shiftY = minY + (yExtent / 2.0);
		scaleZ = 2.0 / maxExtent;
		shiftZ = minZ + (zExtent) / 2.0;

		//Go through all verticies shift and scale them
		for (size_t i = 0; i < shapes.size(); i++)
		{
			for (size_t v = 0; v < shapes[i]->posBuf.size() / 3; v++)
			{
				shapes[i]->posBuf[3 * v + 0] = (shapes[i]->posBuf[3 * v + 0] - shiftX) * scaleX;
				// assert(shapes[i].mesh.positions[3*v+0] >= -1.0 - epsilon);
				// assert(shapes[i].mesh.positions[3*v+0] <= 1.0 + epsilon);
				shapes[i]->posBuf[3 * v + 1] = (shapes[i]->posBuf[3 * v + 1] - shiftY) * scaleY;
				//  assert(shapes[i].mesh.positions[3*v+1] >= -1.0 - epsilon);
				//  assert(shapes[i].mesh.positions[3*v+1] <= 1.0 + epsilon);
				shapes[i]->posBuf[3 * v + 2] = (shapes[i]->posBuf[3 * v + 2] - shiftZ) * scaleZ;
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
		if (key == GLFW_KEY_A && action == GLFW_PRESS)
		{
			player->strafeLeft = true;
		}
		if (key == GLFW_KEY_D && action == GLFW_PRESS)
		{

			player->strafeRight = true;
		}
		if (key == GLFW_KEY_A && action == GLFW_RELEASE)
		{
			player->strafeLeft = false;
		}
		if (key == GLFW_KEY_D && action == GLFW_RELEASE)
		{
			player->strafeRight = false;
		}

		// Dolly Controls
		if (key == GLFW_KEY_W && action == GLFW_PRESS)
		{
			if (!dollyBackward)
			{
				player->dollyForward = true;
			}
		}
		if (key == GLFW_KEY_S && action == GLFW_PRESS)
		{
			if (!dollyForward)
			{
				player->dollyBackward = true;
			}
		}
		if (key == GLFW_KEY_W && action == GLFW_RELEASE)
		{
			player->dollyForward = false;
		}
		if (key == GLFW_KEY_S && action == GLFW_RELEASE)
		{
			player->dollyBackward = false;
		}

		if (key == GLFW_KEY_Q && action == GLFW_PRESS)
		{
			lightPos.x -= 5.0;
		}
		if (key == GLFW_KEY_E && action == GLFW_PRESS)
		{
			lightPos.x += 5.0;
		}

		if (key == GLFW_KEY_Z && action == GLFW_PRESS)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		if (key == GLFW_KEY_Z && action == GLFW_RELEASE)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
	}

	void mouseCallback(GLFWwindow *window, int button, int action, int mods)
	{
		double posX, posY;

		if (action == GLFW_PRESS)
		{
			glfwGetCursorPos(window, &posX, &posY);
			cout << "Pos X " << posX << " Pos Y " << posY << endl;
		}
	}

	void scrollCallback(GLFWwindow *window, double in_deltaX, double in_deltaY)
	{

		float scrollY = -1 * in_deltaY / 50;
		float scrollX = in_deltaX / 50;

		//cout << "phi: " << phi << " " << "scrollY " << scrollY << endl;
		//cout << in_deltaX << endl;

		if (scrollY > 0)
		{
			if (!((phi + scrollY) >= .82))
			{
				phi += scrollY;
				player->phi += scrollY;
			}
		}

		else if (scrollY < 0)
		{
			if (!((phi + scrollY) <= -1.4))
			{
				phi += scrollY;
				player->phi += scrollY;

			}
		}

		theta += scrollX;
		player->theta += scrollX;
	}

	void resizeCallback(GLFWwindow *window, int width, int height)
	{
		glViewport(0, 0, width, height);
	}

	void init(const std::string &resourceDirectory)
	{
		GLSL::checkVersion();

		// Set background color.
		glClearColor(.12f, .34f, .56f, 1.0f);
		// Enable z-buffer test.
		glEnable(GL_DEPTH_TEST);

		// Initialize player and game objects
		gameManager->init();

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

		texProg = make_shared<Program>();
		texProg->setVerbose(true);
		texProg->setShaderNames(resourceDirectory + "/tex_vert.glsl", resourceDirectory + "/tex_frag0.glsl");
		texProg->init();
		texProg->addUniform("P");
		texProg->addUniform("V");
		texProg->addUniform("M");
		texProg->addAttribute("vertPos");
		texProg->addAttribute("vertNor");

		texProg->addAttribute("vertTex");
		texProg->addUniform("Texture0");

		lightPos.x = -10.0;
		lightPos.y = 2.0;
		lightPos.z = 2.0;

		cubemapTexture = createSky(resourceDirectory + "/cracks/", faces);
		glfwSetInputMode(windowManager->getHandle(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}

	// Code to load in textures
	void initTex(const std::string& resourceDirectory){
 		texture0 = make_shared<Texture>();
 		texture0->setFilename(resourceDirectory + "/gamegrass.jpg");
 		texture0->init();
 		texture0->setUnit(0);
 		texture0->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

 		texture1 = make_shared<Texture>();
 		texture1->setFilename(resourceDirectory + "/goomba.bmp");
 		texture1->init();
 		texture1->setUnit(1);
 		texture1->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	} 

	void initGeom(const std::string &resourceDirectory)
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
		if (!rc)
		{
			cerr << errStr << endl;
		}

		rc = tinyobj::LoadObj(TOshapes, objMaterials, errStr, (resourceDirectory + "/cubetex.obj").c_str());
		if (!rc)
		{
			cerr << errStr << endl;
		}
		else
		{
			groundMesh = make_shared<Shape>();
			groundMesh->createShape(TOshapes[0]);
			groundMesh->measure();
			groundMesh->init();
		}

		rc = tinyobj::LoadObj(TOshapes, objMaterials, errStr, (resourceDirectory + "/cube.obj").c_str());
		if (!rc)
		{
			cerr << errStr << endl;
		}
		else
		{
			cubeMesh = make_shared<Shape>();
			cubeMesh->createShape(TOshapes[0]);
			cubeMesh->measure();
			cubeMesh->init();
		}

		rc = tinyobj::LoadObj(TOshapes, objMaterials, errStr, (resourceDirectory + "/goomba.obj").c_str());
		if (!rc)
		{
			cerr << errStr << endl;
		}
		else
		{
			goombaMesh = make_shared<Shape>();
			goombaMesh->createShape(TOshapes[0]);
			goombaMesh->measure();
			goombaMesh->init();
		}

	}

	void setModel(std::shared_ptr<Program> prog, std::shared_ptr<MatrixStack> M)
	{
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
	}

	void setMaterial(int i)
	{
		switch (i)
		{
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

	unsigned int createSky(string dir, vector<string> faces)
	{
		unsigned int textureID;
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
		int width, height, nrChannels;
		stbi_set_flip_vertically_on_load(false);
		for (GLuint i = 0; i < faces.size(); i++)
		{
			unsigned char *data =
				stbi_load((dir + faces[i]).c_str(), &width, &height, &nrChannels, 0);
			if (data)
			{
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
							 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			}
			else
			{
				cout << "failed to load: " << (dir + faces[i]).c_str() << endl;
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

	void render()
	{
		// Get current frame buffer size.
		int width, height;
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
		glViewport(0, 0, width, height);

		// Clear framebuffer.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Use the matrix stack for Lab 6
		float aspect = width / (float)height;

		// Create the matrix stacks - please leave these alone for now
		auto Projection = make_shared<MatrixStack>();
		auto View = make_shared<MatrixStack>();
		auto Model = make_shared<MatrixStack>();

		// Apply perspective projection.
		Projection->pushMatrix();
		Projection->perspective(45.0f, aspect, 0.01f, 10000.0f);

		player->getUpdate();

		vec3 bob = vec3(0,  sin(player->viewBob * 1.3), 0);

		//glm::mat4 lookAt = glm::lookAt(cameraPos, direction, up);
		View->lookAt(player->posShift + bob, player->direction + player->posShift + bob, up);

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

		vector<GameObject> gameObjects = gameManager->getGameObjects();
		int boardSize = gameManager->getBoardsize();

	
		prog->unbind();
	
		// Draw Ground
		texProg->bind();


		glUniformMatrix4fv(texProg->getUniform("P"), 1, GL_FALSE, value_ptr(Projection->topMatrix()));
		glUniformMatrix4fv(texProg->getUniform("V"), 1, GL_FALSE, value_ptr(View->topMatrix()));
		glUniformMatrix4fv(texProg->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
		texture0->bind(texProg->getUniform("Texture0"));

		Model->pushMatrix();
			Model->translate(vec3(0, 0, 0));
			Model->scale(vec3(boardSize, 2, boardSize));

			setModel(texProg, Model);
			groundMesh->draw(texProg);
		Model->popMatrix();

		texture1->bind(texProg->getUniform("Texture0"));


		for (int i = 0; i < gameObjects.size(); i++) {
			GameObject gameObject = gameObjects[i];
			vec3 objVelocity = gameObject.getVelocity();

			//cout << gameObject.getVelocity().x << " " << gameObject.getVelocity().y <<  " " << gameObject.getVelocity().z << endl;

			double dotProd = glm::dot(vec3(-1, 0, 0), objVelocity);
			double mag1 = 1.0;
			double mag2 = glm::length(objVelocity);

			//cout << "Mag2: " << mag2 << endl;

			double dotTheta = dotProd / (mag1 * mag2);
			double angleDiff = acos(dotTheta);

			if (objVelocity.z < 0) {
				angleDiff *= -1;
			}

			//cout << angleDiff << endl;

			Model->pushMatrix();
				Model->translate(gameObject.getPosition());
				Model->rotate(angleDiff + 3.14159, vec3(0, 1, 0));
				Model->rotate(0.1*sTheta, vec3(1, 0, 0));
				//Model->scale(vec3(8, 8, 8));
				setModel(texProg, Model);
				goombaMesh->draw(texProg);
			Model->popMatrix();
		}
		
		// Model->pushMatrix();

		// 	GameObject gameObject = gameObjects[0];
		// 	vec3 objVelocity = vec3(-0.5, 0, 0.5);

		// 	cout << "Velocity: " << gameObject.getVelocity().x << " " << gameObject.getVelocity().y <<  " " << gameObject.getVelocity().z << endl;

		// 	double dotProd = dot(vec3(-1, 0, 0), objVelocity);
		// 	double mag1 = 1.0;
		// 	double mag2 = glm::length(objVelocity);

		// 	cout << "dot " << dotProd << endl;

		// 	double dotTheta = dotProd / (mag1 * mag2);
		// 	double angleDiff = acos(dotTheta);

		// 	if (z < 0) {
		// 		angleDiff *= -1;
		// 	}

		// 	cout << "Angle: " << angleDiff << endl; 
		// 	Model->translate(vec3(0, 20, -40));
		// 	Model->rotate(angleDiff, vec3(0, 1, 0));
			
		// 	setModel(texProg, Model);
		// 	goombaMesh->draw(texProg);
		// Model->popMatrix();
		texProg->unbind();

		prog->bind();
		setMaterial(4);
		Model->pushMatrix();
			Model->translate(vec3(0, 5, -boardSize / 2));
			Model->scale(vec3(boardSize + 2.5, 20, 2));

			setModel(prog, Model);
			cubeMesh->draw(prog);
		Model->popMatrix();

		Model->pushMatrix();
			Model->translate(vec3(0, 5, boardSize / 2));
			Model->scale(vec3(boardSize + 2.5, 20, 2));

			setModel(prog, Model);
			cubeMesh->draw(prog);
		Model->popMatrix();

		Model->pushMatrix();
			Model->translate(vec3(-boardSize / 2, 5, 0));
			Model->scale(vec3(2, 20, boardSize + 2.5));

			setModel(prog, Model);
			cubeMesh->draw(prog);
		Model->popMatrix();

		Model->pushMatrix();
			Model->translate(vec3(boardSize / 2, 5, 0));
			Model->scale(vec3(2, 20, boardSize + 2.5));

			setModel(prog, Model);
			cubeMesh->draw(prog);
		Model->popMatrix();

		prog->unbind();

		// Draw skybox
		Model->pushMatrix();
			Model->translate(vec3(0, 50, 0));
			Model->scale(vec3(3030, 3030, 3030));
			cubeProg->bind();
			glUniformMatrix4fv(cubeProg->getUniform("P"), 1, GL_FALSE, value_ptr(Projection->topMatrix()));
			glDepthFunc(GL_LEQUAL);
			glUniformMatrix4fv(cubeProg->getUniform("V"), 1, GL_FALSE, value_ptr(View->topMatrix()));
			glUniformMatrix4fv(cubeProg->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
			glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
			cubeMesh->draw(cubeProg);

			//set the depth test back to normal!
			glDepthFunc(GL_LESS);
			//unbind the shader for the skybox
			cubeProg->unbind();
			Model->popMatrix();

		Model->popMatrix();
		

		sTheta = sin(glfwGetTime() * 10);
		cTheta = cos(glfwGetTime()) / 5;
		eTheta = std::max(0.0f, (float)sin(glfwGetTime()));
		hTheta = std::max(0.0f, (float)sin(glfwGetTime()));

		// Pop matrix stacks.
		Projection->popMatrix();
		View->popMatrix();

		//gameManager->update();
		updatePlayerOrientation();
	}

	/*
	 * A helper function to be called in render. Orients the player (who is the camera)
	 * to change where it's looking based off of the mouse.
	 */
	void updatePlayerOrientation()
	{
		/*
		 * Get the width and height of the window in pixels
		 */
		int width, height;
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);

		/* 
		 * Get the x and y position of the cursor in screen coordinates 
		 * relative to the upper-left corner. GLFW_CURSOR_DISABLED should
		 * be set so that these values are not limited by the screen bounds.
		 */
		double mouseXPos, mouseYPos;
		glfwGetCursorPos(windowManager->getHandle(), &mouseXPos, &mouseYPos);

		/*
		 * Adjust the "mouse sensitivity". With this division value, moving 
		 * the mouse a distance of half of the "canvas" results in one "shift".
		 * How much a "shift" is is defined below.
		 */
		double xShift = mouseXPos / (width / 2);
		double yShift = mouseYPos / (height / 2);

		/*
		 * Looking all the way up and all the way down is bad: it flips the
		 * camera controls for pitch. In order to prevent this, don't let the 
		 * player look all the way up or all the way down. Presently, these 
		 * numbers are hardcoded as I don't know what defines the upper and 
		 * lower limits. TODO: remove these magic numbers; use an epsilon 
		 * value
		 */
		if (yShift >= 1.5) {
			yShift = 1.49999;
			/* 
			 * The cursor position must be reset or else you get this undesirable effect
			 * where looking too far up or down creates a "buffer" such that you need
			 * to move the mouse just as far in the other direction to change the pitch
			 * again. TODO: genericize this cursor setting to accept any shift divition
			 * value.
			 */
			glfwSetCursorPos(windowManager->getHandle(), mouseXPos, yShift * height / 2);
		}
		else if (yShift <= 0.5) {
			yShift = 0.49999;
			glfwSetCursorPos(windowManager->getHandle(), mouseXPos, yShift * height / 2);
		}
	
		/*
		 * One "shift" for both yaw and pitch is a 180 degree turn
		 */
		player->theta = xShift * PI ;
		player->phi = yShift * PI;
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
	windowManager->init(800, 600);
	windowManager->setEventCallbacks(application);
	application->windowManager = windowManager;

	// This is the code that will likely change program to program as you
	// may need to initialize or set up different data and state
	application->init(resourceDir);
	application->initGeom(resourceDir);
	application->initTex(resourceDir);
	// Loop until the user closes the window.

	double curr_tick = 0;
	double prev_tick = 0;
	double dt = 0;
	while (!glfwWindowShouldClose(windowManager->getHandle()))
	{

		prev_tick = curr_tick;
		curr_tick = glfwGetTime();
		dt = curr_tick - prev_tick;

		// .cout << floor(1 / dt) << endl;

		// Update game state
		application->gameManager->update(dt);
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
