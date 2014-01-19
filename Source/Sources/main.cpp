/*
	Name:			main.cpp
	Project:		OpenGL
	Description:	Contains entry point for OpenGL project
	Doc Version:	1.6
	Author:			Jonathan Simon Jones
	Date(D/M/Y):	19-01-2014
	To do:			Error check readTextFromFile
					Fix min/max values in calculateBoxExtremes
*/

// OpenGL includes
#include <GL\glew.h>
#include <SFML/Graphics.hpp>
#include <glm/glm.hpp>						// Math library
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>				// Converting a matrix object into a float array
#include <glm/gtc/quaternion.hpp>

// OS include
#include <Windows.h>

// STL includes
#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <vector>

struct AABB
{
	glm::vec3 center_position;
	glm::vec3 radius;
};

struct extremes
{
	extremes(): minX(0.0f)
	, maxX(0.0f)
	, minY(0.0f)
	, maxY(0.0f)
	, minZ(0.0f)
	, maxZ(0.0f)
	{}

	float minX;
	float maxX;
	float minY;
	float maxY;
	float minZ;
	float maxZ;
};

// Prototypes
std::string readTextFromFile(std::string nameOfFile);
void checkShaderForErrors(GLuint shader);
void createShaderProgram(GLuint &shaderProgram_);
inline glm::fquat AngularVelocityToSpin(glm::fquat orientation, glm::vec3 angularVelocity);
bool testAABBAABB(const AABB &a, const AABB &b);
void calculateBoxExtremes(glm::mat4 transformMat, GLfloat newBoxCoords[], extremes &boxExtremes);

// Main
int main()
{
	unsigned int windowWidth, windowHeight;
	windowWidth = 800;
	windowHeight = 600;
	// Create a window, set size, give name, set style
	sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "OpenGL", sf::Style::Close);

	// OpenGL vars
	glewExperimental = GL_TRUE;
	glewInit();

	// Get openGL version
	printf("OpenGL version supported by this platform (%s): \n", glGetString(GL_VERSION));

	// Enable depth testing
	glEnable(GL_DEPTH_TEST);

	// Set up vertex array object
	GLuint vao;
	glGenVertexArrays(1, &vao);

	// Bing the vertex array object
	glBindVertexArray(vao);

	GLfloat vertices[] = {
		// First cube
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 
		 0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 
		 0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 
		 0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 
		-0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 1.0f, 
		-0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 

		-0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 
		 0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 1.0f,

		-0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f,
		-0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
		-0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 
		-0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 1.0f,
		-0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 1.0f,

		 0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
		 0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 1.0f,
		 0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 1.0f,

		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 1.0f,

		-0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 1.0f,
		-0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 1.0f,

		// Second cube
		/*-0.5f, -0.5f, -0.5f, 0.3f, 0.3f, 0.3f, 
		 0.5f, -0.5f, -0.5f, 0.3f, 0.3f, 0.3f, 
		 0.5f,  0.5f, -0.5f, 0.3f, 0.3f, 0.3f, 
		 0.5f,  0.5f, -0.5f, 0.3f, 0.3f, 0.3f, 
		-0.5f,  0.5f, -0.5f, 0.3f, 0.3f, 0.3f, 
		-0.5f, -0.5f, -0.5f, 0.3f, 0.3f, 0.3f, 

		-0.5f, -0.5f,  0.5f, 0.3f, 0.3f, 0.3f,
		 0.5f, -0.5f,  0.5f, 0.3f, 0.3f, 0.3f, 
		 0.5f,  0.5f,  0.5f, 0.3f, 0.3f, 0.3f,
		 0.5f,  0.5f,  0.5f, 0.3f, 0.3f, 0.3f,
		-0.5f,  0.5f,  0.5f, 0.3f, 0.3f, 0.3f,
		-0.5f, -0.5f,  0.5f, 0.3f, 0.3f, 0.3f,

		-0.5f,  0.5f,  0.5f, 0.3f, 0.3f, 0.3f,
		-0.5f,  0.5f, -0.5f, 0.3f, 0.3f, 0.3f,
		-0.5f, -0.5f, -0.5f, 0.3f, 0.3f, 0.3f,
		-0.5f, -0.5f, -0.5f, 0.3f, 0.3f, 0.3f,
		-0.5f, -0.5f,  0.5f, 0.3f, 0.3f, 0.3f,
		-0.5f,  0.5f,  0.5f, 0.3f, 0.3f, 0.3f,

		 0.5f,  0.5f,  0.5f, 0.3f, 0.3f, 0.3f,
		 0.5f,  0.5f, -0.5f, 0.3f, 0.3f, 0.3f,
		 0.5f, -0.5f, -0.5f, 0.3f, 0.3f, 0.3f,
		 0.5f, -0.5f, -0.5f, 0.3f, 0.3f, 0.3f,
		 0.5f, -0.5f,  0.5f, 0.3f, 0.3f, 0.3f,
		 0.5f,  0.5f,  0.5f, 0.3f, 0.3f, 0.3f,

		-0.5f, -0.5f, -0.5f, 0.3f, 0.3f, 0.3f,
		 0.5f, -0.5f, -0.5f, 0.3f, 0.3f, 0.3f,
		 0.5f, -0.5f,  0.5f, 0.3f, 0.3f, 0.3f,
		 0.5f, -0.5f,  0.5f, 0.3f, 0.3f, 0.3f,
		-0.5f, -0.5f,  0.5f, 0.3f, 0.3f, 0.3f,
		-0.5f, -0.5f, -0.5f, 0.3f, 0.3f, 0.3f,

		-0.5f,  0.5f, -0.5f, 0.3f, 0.3f, 0.3f,
		 0.5f,  0.5f, -0.5f, 0.3f, 0.3f, 0.3f,
		 0.5f,  0.5f,  0.5f, 0.3f, 0.3f, 0.3f,
		 0.5f,  0.5f,  0.5f, 0.3f, 0.3f, 0.3f,
		-0.5f,  0.5f,  0.5f, 0.3f, 0.3f, 0.3f,
		-0.5f,  0.5f, -0.5f, 0.3f, 0.3f, 0.3f,*/

		// Floor
		-0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.5f, 
		 0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.5f,
		 0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 0.5f,
		 0.5f,  0.5f, 0.0f, 0.5f, 0.0f, 0.0f, 
		-0.5f,  0.5f, 0.0f, 0.5f, 0.0f, 0.0f, 
		-0.5f, -0.5f, 0.0f, 0.5f, 0.0f, 0.0f, 
	};

	GLfloat boundingBoxVertices[] = { 
		// Second cube
		-0.5f, -0.5f, -0.5f, 0.3f, 0.3f, 0.3f,
		 0.5f, -0.5f, -0.5f, 0.3f, 0.3f, 0.3f, 
		 0.5f,  0.5f, -0.5f, 0.3f, 0.3f, 0.3f, 
		 0.5f,  0.5f, -0.5f, 0.3f, 0.3f, 0.3f, 
		-0.5f,  0.5f, -0.5f, 0.3f, 0.3f, 0.3f, 
		-0.5f, -0.5f, -0.5f, 0.3f, 0.3f, 0.3f, 

		-0.5f, -0.5f,  0.5f, 0.3f, 0.3f, 0.3f,
		 0.5f, -0.5f,  0.5f, 0.3f, 0.3f, 0.3f, 
		 0.5f,  0.5f,  0.5f, 0.3f, 0.3f, 0.3f,
		 0.5f,  0.5f,  0.5f, 0.3f, 0.3f, 0.3f,
		-0.5f,  0.5f,  0.5f, 0.3f, 0.3f, 0.3f,
		-0.5f, -0.5f,  0.5f, 0.3f, 0.3f, 0.3f,

		-0.5f,  0.5f,  0.5f, 0.3f, 0.3f, 0.3f,
		-0.5f,  0.5f, -0.5f, 0.3f, 0.3f, 0.3f,
		-0.5f, -0.5f, -0.5f, 0.3f, 0.3f, 0.3f,
		-0.5f, -0.5f, -0.5f, 0.3f, 0.3f, 0.3f,
		-0.5f, -0.5f,  0.5f, 0.3f, 0.3f, 0.3f,
		-0.5f,  0.5f,  0.5f, 0.3f, 0.3f, 0.3f,

		 0.5f,  0.5f,  0.5f, 0.3f, 0.3f, 0.3f,
		 0.5f,  0.5f, -0.5f, 0.3f, 0.3f, 0.3f,
		 0.5f, -0.5f, -0.5f, 0.3f, 0.3f, 0.3f,
		 0.5f, -0.5f, -0.5f, 0.3f, 0.3f, 0.3f,
		 0.5f, -0.5f,  0.5f, 0.3f, 0.3f, 0.3f,
		 0.5f,  0.5f,  0.5f, 0.3f, 0.3f, 0.3f,

		-0.5f, -0.5f, -0.5f, 0.3f, 0.3f, 0.3f,
		 0.5f, -0.5f, -0.5f, 0.3f, 0.3f, 0.3f,
		 0.5f, -0.5f,  0.5f, 0.3f, 0.3f, 0.3f,
		 0.5f, -0.5f,  0.5f, 0.3f, 0.3f, 0.3f,
		-0.5f, -0.5f,  0.5f, 0.3f, 0.3f, 0.3f,
		-0.5f, -0.5f, -0.5f, 0.3f, 0.3f, 0.3f,

		-0.5f,  0.5f, -0.5f, 0.3f, 0.3f, 0.3f,
		 0.5f,  0.5f, -0.5f, 0.3f, 0.3f, 0.3f,
		 0.5f,  0.5f,  0.5f, 0.3f, 0.3f, 0.3f,
		 0.5f,  0.5f,  0.5f, 0.3f, 0.3f, 0.3f,
		-0.5f,  0.5f,  0.5f, 0.3f, 0.3f, 0.3f,
		-0.5f,  0.5f, -0.5f, 0.3f, 0.3f, 0.3f,
	};

	GLfloat boundingBoxVertices2[48] = {0.0f};

	// Setup vertex buffer
	// This is used to store the vert info that is to be passed to the graphics card
	GLuint vertexBuffer[3];				// Reference to memory where buffer is
	glGenBuffers(1, vertexBuffer);		// We only want one buffer

	// Make buffer active
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer[0]);	// Make vertexBuffer the active array buffer
													// Now that it is active we can copy the vertex data to it

	// Copy vertex data to the active array buffer (currently vertexBuffer)
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);		// glBufferData(the array to deal with, size in bytes, pointer to data, usage of vertex data)

	// Make buffer active
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer[1]);	// Make vertexBuffer the active array buffer
															// Now that it is active we can copy the vertex data to it

	// Copy vertex data to the active array buffer (currently vertexBuffer)
	glBufferData(GL_ARRAY_BUFFER, sizeof(boundingBoxVertices), boundingBoxVertices, GL_STATIC_DRAW);		// glBufferData(the array to deal with, size in bytes, pointer to data, usage of vertex data)

	// Make buffer active
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer[2]);	// Make vertexBuffer the active array buffer
													// Now that it is active we can copy the vertex data to it

	// Copy vertex data to the active array buffer (currently vertexBuffer)
	glBufferData(GL_ARRAY_BUFFER, sizeof(boundingBoxVertices2), boundingBoxVertices2, GL_DYNAMIC_DRAW);		// glBufferData(the array to deal with, size in bytes, pointer to data, usage of vertex data)
	
	// Rebind first buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	GLuint elements[] = {
		// Bottom
		0, 2, 1,
		3, 0, 2,

		// Top
		4, 6, 5,
		7, 4, 6,

		// Front
		0, 5, 1,
		4, 0, 5,

		// Right
		1, 6, 2,
		5, 1, 6,

		// Back
		2, 7, 3,
		6, 2, 7,

		// Left 
		3, 4, 0,
		7, 3, 4,
	};

	GLuint ebo;
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);
	
	// Create shader program
	GLuint shaderProgram;
	createShaderProgram(shaderProgram);

	// Set shader to graphcics pipeline
	glUseProgram(shaderProgram);

	// Get references to shader vars
	GLint posAttrib = glGetAttribLocation(shaderProgram, "position"); // Get reference to position in vertex shader
	GLint colAttrib = glGetAttribLocation(shaderProgram, "color");	  // Get triangle color attribute

	// Specify the format of the attribute
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), 0);
	glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)(3*sizeof(float)));

	// Enable vertex shader attribute
	glEnableVertexAttribArray(posAttrib);
	glEnableVertexAttribArray(colAttrib);

	// Box 1
	// 2D rotation
	glm::mat4 model;
	glm::fquat orientation(1.0f, 0.0f, 0.0f, 0.0f);
	glm::vec3 position(0.0f, 0.0f, 0.0f);

	const float fps = 200.0f;
	const float dt = 1 / fps;
	glm::vec3 velocity(0.1f, 0.0f, 0.0f);
	glm::vec3 gravity(0.0f, 0.0f, -1.0f);
	glm::vec3 angularVelocity(0.0f, 0.0f, 0.1f);

	position += glm::vec3(0.0f, 0.0f, 0.0f);
	model = glm::translate(model, position);

	glm::mat4 ident;

	AABB BBB1;

	// Set up uniform attribute 
	GLint uniModel = glGetUniformLocation(shaderProgram, "model");
	glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));

	// Create view matrix
	glm::mat4 view = glm::lookAt(			// Looks at specifies XY plane as ground, Z axis is up
		glm::vec3(0.0f, -5.0f, 2.0f),		// Position
		glm::vec3(0.0f, 0.0f, 0.0f),		// Where to look on the screen, forward vector
		glm::vec3(0.0f, 0.0f, 1.0f)			// The up axis
		);

	// Set up uniform for view matrix
	GLint uniView = glGetUniformLocation(shaderProgram, "view");
	glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));

	// Create projection matrix
	glm::mat4 proj = glm::perspective(45.0f, (float)windowWidth / (float)windowHeight, 1.0f, 15.0f);		// perspective(vetical fov, aspect ratio, near, far)
	GLint uniProj = glGetUniformLocation(shaderProgram, "proj");
	glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));

	// Set up uniform attribute 
	GLint uniColor = glGetUniformLocation(shaderProgram, "overrideColor");
	glUniform3f(uniColor, 1.0f, 1.0f, 1.0f);

	// Set up unifrom alpha
	GLint uniAlpha = glGetUniformLocation(shaderProgram, "Alpha");
	glUniform1f(uniAlpha, 1.0f);

	// Box 2
	glm::vec3 box2Pos(3.0f, 0.0f, 0.5f);
	float movSpeed = 4.0f;
	AABB BBB2;

	// While window open
	while (window.isOpen())
	{
		// Set clear colour
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		// Clear back buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Box 1
			//glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer[0]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);		// glBufferData(the array to deal with, size in bytes, pointer to data, usage of vertex data)

			// Advance linearly
			//velocity += gravity * dt;
			//position += velocity * dt;
			model = glm::translate(ident, position);

			// Calculate rotation
			glm::fquat spin = AngularVelocityToSpin(orientation, angularVelocity);
			orientation = orientation + spin * dt;
			orientation = glm::normalize(orientation);
			model = glm::rotate( model, glm::angle(orientation), glm::axis(orientation) );
			glm::mat4 modelCopy = model;
		
			// Upload variables to graphics mem
			glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
			glUniform1f(uniAlpha, 1.0f);

			// Draw cube
			glDrawArrays(GL_TRIANGLES, 0, 36);

		// Box 2
			BBB2.center_position = box2Pos;
			BBB2.radius = glm::vec3(0.5f, 0.5f, 0.5f);

			// Calculate position
			model = glm::translate(ident, box2Pos);

			// Upload variables to graphics mem
			glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
			glUniform1f(uniAlpha, 1.0f);

			// Draw cube
			glDrawArrays(GL_TRIANGLES, 0, 36);

		// AABB Box 1
			//glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer[1]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(boundingBoxVertices2), boundingBoxVertices2, GL_STATIC_DRAW);	
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);// glBufferData(the array to deal with, size in bytes, pointer to data, usage of vertex data)

			// Enable alpha blending 
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_COLOR, GL_ONE_MINUS_SRC_COLOR);

			GLfloat temp[48] = {0.0f};
			extremes boxExtremes;
			calculateBoxExtremes(modelCopy, temp, boxExtremes);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(temp), temp);

			// Calculate position
			//model = glm::translate(ident, position);
			//model = glm::scale(model, glm::vec3(1.5f, 1.5f, 1.5f));

			//glm::vec4 world(1.0f, 1.0f, 1.0f, 1.0f);
			//world = world * model;

			BBB1.center_position = position;
			BBB1.radius = glm::vec3(boxExtremes.minX - position.x, boxExtremes.minY - position.y, boxExtremes.minZ - position.z);
		
			// Upload variables to graphics mem
			glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(ident));
			glUniform1f(uniAlpha, 0.5f);

			// Draw bounding box
			//glDrawArrays(GL_TRIANGLES, 0, 36);
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

			// Disable alpha blending
			glDisable(GL_BLEND);

		// Floor
			//glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer[0]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);		// glBufferData(the array to deal with, size in bytes, pointer to data, usage of vertex data)

			// Calculate position
			model = glm::translate(ident, glm::vec3(0.0f, 0.0f, -0.5f));
			model = glm::scale(model, glm::vec3(10.0f, 10.0f, 1.0f));
		
			// Upload variables to graphics mem
			glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
			glUniform1f(uniAlpha, 1.0f);

			// Draw bounding box
			//glDrawArrays(GL_TRIANGLES, 36, 6);

		
		// Display back buffer
		window.display();

		if(testAABBAABB(BBB1, BBB2))
		{
			std::cout << "Boxes colliding" << std::endl;
		}
		else
		{
			std::cout << "No collision" << std::endl;
		}

		// Handle messages
		sf::Event windowEvent;
		while (window.pollEvent(windowEvent))
		{
			// Handle events
			switch(windowEvent.type)
			{
			case sf::Event::Closed:
				window.close();

			case sf::Event::KeyPressed:
				{
					if(windowEvent.key.code == sf::Keyboard::Escape)
					{
						window.close();
					}

					if(windowEvent.key.code == sf::Keyboard::Up)
					{
						box2Pos.y += movSpeed * dt;
					}

					if(windowEvent.key.code == sf::Keyboard::Down)
					{
						box2Pos.y -= movSpeed * dt;
					}

					if(windowEvent.key.code == sf::Keyboard::Left)
					{
						box2Pos.x -= movSpeed * dt;
					}

					if(windowEvent.key.code == sf::Keyboard::Right)
					{
						box2Pos.x += movSpeed * dt;
					}
				}
			}
		}
	}

	glDeleteProgram(shaderProgram);

    glDeleteBuffers(2, vertexBuffer);

    glDeleteVertexArrays(1, &vao);

	return 0;
}

std::string readTextFromFile(std::string nameOfFile)
{
	// !Add error checking to see if file open correctly
	std::ifstream vertexShaderSource(nameOfFile);
	std::stringstream vertexShaderText;
	vertexShaderText << vertexShaderSource.rdbuf();
	return vertexShaderText.str();
}

void checkShaderForErrors(GLuint shader)
{
	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

	if(status != GL_TRUE)
	{
		char buffer[512];
		glGetShaderInfoLog(shader, 512, NULL, buffer);
		std::cout << "Failed to compile shader, reasons to follow:" << std::endl;
		std::cout << buffer << std::endl;
		std::cin >> buffer;
	}
}

void createShaderProgram(GLuint &shaderProgram_)
{
	// Get vertex buffer 
	std::string vertexShaderText = readTextFromFile("VertexShaders/color3D.vert");
	const char *vertexShaderData = vertexShaderText.c_str();

	// Create vertex shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

	// Load the data into the shader
	glShaderSource(vertexShader, 1, &vertexShaderData, NULL);

	// Compile the vertex shader
	glCompileShader(vertexShader);

	checkShaderForErrors(vertexShader);

	// Get fragment buffer
	std::string fragmentShaderText = readTextFromFile("FragmentShaders/inputColor.frag");
	const char *fragmentShaderData = fragmentShaderText.c_str();

	// Create fragment shader 
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	
	// Load the data into the shader
	glShaderSource(fragmentShader,1, &fragmentShaderData, NULL);

	// Compile the fragment shader
	glCompileShader(fragmentShader);

	checkShaderForErrors(fragmentShader);

	// Create shader program 
	shaderProgram_ = glCreateProgram();
	glAttachShader(shaderProgram_, vertexShader);
	glAttachShader(shaderProgram_, fragmentShader);

	// Bing fragment shader output to buffer
	glBindFragDataLocation(shaderProgram_, 0, "outColor");

	// Finished construction of shader by linking it
	glLinkProgram(shaderProgram_);

	// Delete the shaders as we do not need them anymore
	glDeleteShader(fragmentShader);
    glDeleteShader(vertexShader);
}

inline glm::fquat AngularVelocityToSpin(glm::fquat orientation, glm::vec3 angularVelocity)
{
	const float x = angularVelocity.x;
	const float y = angularVelocity.y;
	const float z = angularVelocity.z;

	return 0.5f * glm::fquat(0, x, y, z) * orientation;
}

bool testAABBAABB(const AABB &a, const AABB &b)
{
	if(glm::abs(a.center_position.x - b.center_position.x) > (a.radius.x + b.radius.x)) return false;
	if(glm::abs(a.center_position.y - b.center_position.y) > (a.radius.y + b.radius.y))	return false;
	if(glm::abs(a.center_position.z - b.center_position.z) > (a.radius.z + b.radius.z)) return false;
				
	return true;
}

void calculateBoxExtremes(glm::mat4 transformMat, GLfloat newBoxCoords[], extremes &boxExtremes)
{
	std::vector<glm::vec4> v;
	
	// Get positions of verts
	v.push_back(glm::vec4(-0.5f, -0.5f, -0.5f, 1.0f));
	v.push_back(glm::vec4(-0.5f,  0.5f, -0.5f, 1.0f));
	v.push_back(glm::vec4( 0.5f, -0.5f, -0.5f, 1.0f));
	v.push_back(glm::vec4( 0.5f,  0.5f, -0.5f, 1.0f));
	v.push_back(glm::vec4(-0.5f, -0.5f,  0.5f, 1.0f));
	v.push_back(glm::vec4(-0.5f,  0.5f,  0.5f, 1.0f));
	v.push_back(glm::vec4( 0.5f, -0.5f,  0.5f, 1.0f));
	v.push_back(glm::vec4( 0.5f,  0.5f,  0.5f, 1.0f));

	std::vector<glm::vec4> updateVerts;
	
	for(std::vector<glm::vec4>::iterator it = v.begin(); it != v.end(); it++)
	{
		updateVerts.push_back((*it) * transformMat);
	}

	for(std::vector<glm::vec4>::iterator it = updateVerts.begin(); it != updateVerts.end(); it++)
	{
		if((*it).x > boxExtremes.minX)
		{
			boxExtremes.minX = (*it).x;
		}

		if((*it).x < boxExtremes.maxX)
		{
			boxExtremes.maxX = (*it).x;
		}

		if((*it).y > boxExtremes.minY)
		{
			boxExtremes.minY = (*it).y;
		}

		if((*it).y < boxExtremes.maxY)
		{
			boxExtremes.maxY = (*it).y;
		}

		if((*it).z > boxExtremes.minZ)
		{
			boxExtremes.minZ = (*it).z;
		}

		if((*it).z < boxExtremes.maxZ)
		{
			boxExtremes.maxZ = (*it).z;
		}
	}

	/*
	std::vector<glm::vec3> newBoxCoords;
	newBoxCoords.push_back(glm::vec3(boxExtremes.minX, boxExtremes.minY, boxExtremes.minZ));
	newBoxCoords.push_back(glm::vec3(boxExtremes.maxX, boxExtremes.minY, boxExtremes.minZ));
	newBoxCoords.push_back(glm::vec3(boxExtremes.maxX, boxExtremes.maxY, boxExtremes.minZ));
	newBoxCoords.push_back(glm::vec3(boxExtremes.minX, boxExtremes.maxY, boxExtremes.minZ));

	newBoxCoords.push_back(glm::vec3(boxExtremes.minX, boxExtremes.minY, boxExtremes.maxZ));
	newBoxCoords.push_back(glm::vec3(boxExtremes.maxX, boxExtremes.minY, boxExtremes.maxZ));
	newBoxCoords.push_back(glm::vec3(boxExtremes.maxX, boxExtremes.maxY, boxExtremes.maxZ));
	newBoxCoords.push_back(glm::vec3(boxExtremes.minX, boxExtremes.maxY, boxExtremes.maxZ));
	*/

	newBoxCoords[0] = boxExtremes.minX;
	newBoxCoords[1] = boxExtremes.minY;
	newBoxCoords[2] = boxExtremes.minZ; 
	newBoxCoords[3] = 0.3f; 
	newBoxCoords[4] = 0.3f; 
	newBoxCoords[5] = 0.3f; 
	newBoxCoords[6] = boxExtremes.maxX; 
	newBoxCoords[7] = boxExtremes.minY; 
	newBoxCoords[8] = boxExtremes.minZ; 
	newBoxCoords[9] = 0.3f; 
	newBoxCoords[10] = 0.3f; 
	newBoxCoords[11] = 0.3f; 
	newBoxCoords[12] = boxExtremes.maxX; 
	newBoxCoords[13] = boxExtremes.maxY; 
	newBoxCoords[14] = boxExtremes.minZ; 
	newBoxCoords[15] = 0.3f; 
	newBoxCoords[16] = 0.3f; 
	newBoxCoords[17] = 0.3f;
	newBoxCoords[18] = boxExtremes.minX; 
	newBoxCoords[19] = boxExtremes.maxY; 
	newBoxCoords[20] = boxExtremes.minZ;
	newBoxCoords[21] = 0.3f; 
	newBoxCoords[22] = 0.3f; 
	newBoxCoords[23] = 0.3f;
	newBoxCoords[24] = boxExtremes.minX; 
	newBoxCoords[25] = boxExtremes.minY; 
	newBoxCoords[26] = boxExtremes.maxZ;
	newBoxCoords[27] = 0.3f; 
	newBoxCoords[28] = 0.3f; 
	newBoxCoords[29] = 0.3f;
	newBoxCoords[30] = boxExtremes.maxX; 
	newBoxCoords[31] = boxExtremes.minY; 
	newBoxCoords[32] = boxExtremes.maxZ;
	newBoxCoords[33] = 0.3f; 
	newBoxCoords[34] = 0.3f; 
	newBoxCoords[35] = 0.3f;
	newBoxCoords[36] = boxExtremes.maxX; 
	newBoxCoords[37] = boxExtremes.maxY; 
	newBoxCoords[38] = boxExtremes.maxZ;
	newBoxCoords[39] = 0.3f; 
	newBoxCoords[40] = 0.3f; 
	newBoxCoords[41] = 0.3f;
	newBoxCoords[42] = boxExtremes.minX; 
	newBoxCoords[43] = boxExtremes.maxY; 
	newBoxCoords[44] = boxExtremes.maxZ;
	newBoxCoords[45] = 0.3f; 
	newBoxCoords[46] = 0.3f; 
	newBoxCoords[47] = 0.3f; 
}