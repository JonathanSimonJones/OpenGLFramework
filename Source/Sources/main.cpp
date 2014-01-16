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

	// Set up vertex array object
	GLuint vao;
	glGenVertexArrays(1, &vao);

	// Bing the vertex array object
	glBindVertexArray(vao);

	// Set Verts for rectangle
	float vertices[] = {
		-0.5f,  0.5f, 1.0f, 0.0f, 0.0f,	// Top-left
		 0.5f,  0.5f, 0.0f, 1.0f, 0.0f,	// Top-right
		 0.5f, -0.5f, 0.0f, 0.0f, 1.0f,	// Bottom-right
		-0.5f, -0.5f, 1.0f, 1.0f, 1.0f  // Bottom-left
	};

	// Setup vertex buffer
	// This is used to store the vert info that is to be passed to the graphics card
	GLuint vertexBuffer;				// Reference to memory where buffer is
	glGenBuffers(1, &vertexBuffer);		// We only want one buffer

	// Make buffer active
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);	// Make vertexBuffer the active array buffer
													// Now that it is active we can copy the vertex data to it

	// Copy vertex data to the active array buffer (currently vertexBuffer)
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);		// glBufferData(the array to deal with, size in bytes, pointer to data, usage of vertex data)

	// Set up the elements
	GLuint elements[] = { 
		0, 1, 2,
		2, 3, 0
	};

	// Setup element buffer
	GLuint elementBuffer;
	glGenBuffers(1, &elementBuffer);

	// Make buffer active
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
	
	// Copy elements to elemenet buffer
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
	glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), 0);
	glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(2*sizeof(float)));

	// Enable vertex shader attribute
	glEnableVertexAttribArray(posAttrib);
	glEnableVertexAttribArray(colAttrib);

	// While window open
	while (window.isOpen())
	{
		window.clear();
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		window.display();

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
	}

	glDeleteProgram(shaderProgram);

    glDeleteBuffers(1, &elementBuffer);
    glDeleteBuffers(1, &vertexBuffer);

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
	std::string vertexShaderText = readTextFromFile("VertexShaders/triangleDifferentColorCornor.vert");
	const char *vertexShaderData = vertexShaderText.c_str();

	// Create vertex shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

	// Load the data into the shader
	glShaderSource(vertexShader, 1, &vertexShaderData, NULL);

	// Compile the vertex shader
	glCompileShader(vertexShader);

	// Get fragment buffer
	std::string fragmentShaderText = readTextFromFile("FragmentShaders/inputColor.frag");
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

	// Delete the shaders as we do not need them anymore
	glDeleteShader(fragmentShader);
    glDeleteShader(vertexShader);
}

//// Headers
//#include <GL/glew.h>
//#include <SFML/Window.hpp>
//#include <iostream>
//
//// Shader sources
//const GLchar* vertexSource =
//    "#version 150 core\n"
//    "in vec2 position;"
//    "in vec3 color;"
//    "out vec3 Color;"
//    "void main() {"
//    "   Color = color;"
//    "   gl_Position = vec4(position, 0.0, 1.0);"
//    "}";
//const GLchar* fragmentSource =
//    "#version 150 core\n"
//    "in vec3 Color;"
//    "out vec4 outColor;"
//    "void main() {"
//    "   outColor = vec4(Color, 1.0);"
//    "}";
//
//int main()
//{
//    sf::Window window(sf::VideoMode(800, 600, 32), "OpenGL", sf::Style::Titlebar | sf::Style::Close);
//    
//    // Initialize GLEW
//    glewExperimental = GL_TRUE;
//    glewInit();
//
//    // Create Vertex Array Object
//    GLuint vao;
//    glGenVertexArrays(1, &vao);
//    glBindVertexArray(vao);
//
//    // Create a Vertex Buffer Object and copy the vertex data to it
//    GLuint vbo;
//    glGenBuffers(1, &vbo);
//
//    GLfloat vertices[] = {
//        -0.5f,  0.5f, 1.0f, 0.0f, 0.0f, // Top-left
//         0.5f,  0.5f, 0.0f, 1.0f, 0.0f, // Top-right
//         0.5f, -0.5f, 0.0f, 0.0f, 1.0f, // Bottom-right
//        -0.5f, -0.5f, 1.0f, 1.0f, 1.0f  // Bottom-left
//    };
//
//    glBindBuffer(GL_ARRAY_BUFFER, vbo);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
//
//    // Create an element array
//    GLuint ebo;
//    glGenBuffers(1, &ebo);
//
//    GLuint elements[] = {
//        0, 1, 2,
//        2, 3, 0
//    };
//
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
//    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);
//
//    // Create and compile the vertex shader
//    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
//    glShaderSource(vertexShader, 1, &vertexSource, NULL);
//    glCompileShader(vertexShader);
//
//    // Create and compile the fragment shader
//    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
//    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
//    glCompileShader(fragmentShader);
//
//    // Link the vertex and fragment shader into a shader program
//    GLuint shaderProgram = glCreateProgram();
//    glAttachShader(shaderProgram, vertexShader);
//    glAttachShader(shaderProgram, fragmentShader);
//    glBindFragDataLocation(shaderProgram, 0, "outColor");
//    glLinkProgram(shaderProgram);
//    glUseProgram(shaderProgram);
//
//    // Specify the layout of the vertex data
//    GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
//    glEnableVertexAttribArray(posAttrib);
//    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);
//
//    GLint colAttrib = glGetAttribLocation(shaderProgram, "color");
//    glEnableVertexAttribArray(colAttrib);
//    glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));
//
//    while (window.isOpen())
//    {
//        // Clear the screen to black
//        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
//        glClear(GL_COLOR_BUFFER_BIT);
//        
//        // Draw a rectangle from the 2 triangles using 6 indices
//        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
//
//        // Swap buffers
//        window.display();
//
//		sf::Event windowEvent;
//        while (window.pollEvent(windowEvent))
//        {
//            switch (windowEvent.type)
//            {
//            case sf::Event::Closed:
//                window.close();
//                break;
//            }
//        }
//    }
//
//    glDeleteProgram(shaderProgram);
//    glDeleteShader(fragmentShader);
//    glDeleteShader(vertexShader);
//
//    glDeleteBuffers(1, &ebo);
//    glDeleteBuffers(1, &vbo);
//
//    glDeleteVertexArrays(1, &vao);
//}