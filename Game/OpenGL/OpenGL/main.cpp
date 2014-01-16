/*
	Name:			main.cpp
	Project:		OpenGL
	Description:	Contains entry point for OpenGL project
	Doc Version:	1.4
	Author:			Jonathan Simon Jones
	Date(D/M/Y):	16-01-2014
	To do:			Error check readTextFromFile
*/

// OpenGL includes
#include <GL\glew.h>
#include <SFML\Graphics.hpp>
#include <Windows.h>

// STL includes
#include <fstream>
#include <string>
#include <sstream>
#include <iostream>

// Prototypes
std::string readTextFromFile(std::string nameOfFile);
void checkShaderForErrors(GLuint shader);
void createShaderProgram(GLuint &shaderProgram_);

// Main
int main()
{
	// Create a window, set size, give name, set style
	sf::RenderWindow window(sf::VideoMode(800, 600), "OpenGL", sf::Style::Close);

	// OpenGL vars
	glewExperimental = GL_TRUE;
	glewInit();

	// Set Verts for triangle
	float vertices[] = {
		 0.0f,  0.5f, 1.0f, 0.0f, 0.0f,	// Vertex 1: Red
		 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,	// Vertex 2: Green
		-0.5f, -0.5f, 0.0f, 0.0f, 1.0f	// Vertex 3: Blue
	};

	// Setup vertex buffer
	// This is used to store the vert info that is to be passed to the graphics card
	GLuint vertexBuffer;				// Reference to memory where buffer is
	glGenBuffers(1, &vertexBuffer);		// We only want one buffer
	
	printf("%u\n", vertexBuffer);		// Print out the number of buffers

	// Make buffer active
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);	// Make vertexBuffer the active array buffer
													// Now that it is active we can copy the vertex data to it

	// Copy vertex data to the active array buffer (currently vertexBuffer)
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);		// glBufferData(the array to deal with, size in bytes, pointer to data, usage of vertex data)

	// Create shader program
	GLuint shaderProgram;
	createShaderProgram(shaderProgram);

	// Set shader to graphcics pipeline
	glUseProgram(shaderProgram);

	// Set up vertex array object
	GLuint vao;
	glGenVertexArrays(1, &vao);

	// Bing the vertex array object
	glBindVertexArray(vao);
	
	// Get references to shader vars
	GLint posAttrib = glGetAttribLocation(shaderProgram, "position");		// Get reference to position in vertex shader
	GLint colAttrib = glGetAttribLocation(shaderProgram, "color");	// Get triangle color attribute

	// Specify the format of the attribute
	glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), 0);
	glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(2*sizeof(float)));

	// Enable vertex shader attribute
	glEnableVertexAttribArray(posAttrib);
	glEnableVertexAttribArray(colAttrib);

	// While window open
	while (window.isOpen())
	{
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
				if(windowEvent.key.code == sf::Keyboard::Escape)
					window.close();
			}
		}

		window.clear();
		glDrawArrays(GL_TRIANGLES, 0, 3);
		window.display();
	}

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
	std::string vertexShaderText = readTextFromFile("triangleDifferentColorCornor.vert");
	const char *vertexShaderData = vertexShaderText.c_str();

	// Create vertex shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

	// Load the data into the shader
	glShaderSource(vertexShader, 1, &vertexShaderData, NULL);

	// Compile the vertex shader
	glCompileShader(vertexShader);

	// Get fragment buffer
	std::string fragmentShaderText = readTextFromFile("inputColor.frag");
	const char *fragmentShaderData = fragmentShaderText.c_str();

	// Create fragment shader 
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	
	// Load the data into the shader
	glShaderSource(fragmentShader,1, &fragmentShaderData, NULL);

	// Compile the fragment shader
	glCompileShader(fragmentShader);

	// Create shader program 
	shaderProgram_ = glCreateProgram();
	glAttachShader(shaderProgram_, vertexShader);
	glAttachShader(shaderProgram_, fragmentShader);

	// Bing fragment shader output to buffer
	glBindFragDataLocation(shaderProgram_, 0, "outColor");

	// Finished construction of shader by linking it
	glLinkProgram(shaderProgram_);
}
