//#ifdef __APPLE__
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
//#elif defined _WIN32 || defined _WIN64
//#include "Dependencies/glew/glew.h"
//#include "Dependencies/GLFW/glfw3.h"
//#include "Dependencies/glm/glm.hpp"
//#include "Dependencies/glm/gtc/matrix_transform.hpp"
//#endif

#define STB_IMAGE_IMPLEMENTATION
#include "Shader.h"
#include "Texture.h"


#include <iostream>
#include <fstream>
#include <vector>
#include <map>

using namespace std;
using glm::vec3;
using glm::vec4;
using glm::mat4;

//********** pre-provided utility functions are written in util.h **********
#include "util.h"
#include "callbacks.h"




//********** Utility classes should be defined inseparate .h/.cpp flies.
//           Include external classes below                             **********




//
// 
//********** Define utility global variables below **********
const int SCR_WIDTH = 800;
const int SCR_HEIGHT = 600;
Shader generalShader;




//
//
//********** Define utility functions below **********

// incapsulate the matrix definition for OpenGL objects here.
// refer to the codebase https://github.com/sinyiwsy/CSCI-3260-Assignment2/blob/master/main.cpp, Line 744
void setupTransformMatrix(string obj) {

}




//
//
//********** The OpenGL pipeline functions **********
void sendDataToOpenGL() {

}


void paintGL(void) {

}


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}


void initializedGL(void) //run only once
{
	if (glewInit() != GLEW_OK) {
		std::cout << "GLEW not OK." << std::endl;
	}

	get_OpenGL_info();
	sendDataToOpenGL();

	installShaders();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glDepthFunc(GL_LESS);
}



//
//
//********** The main function
int main(int argc, char* argv[])
{
	GLFWwindow* window;

	/* Initialize the glfw */
	if (!glfwInit()) {
		std::cout << "Failed to initialize GLFW" << std::endl;
		return -1;
	}
	/* glfw: configure; necessary for MAC */
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Assignment 2", NULL, NULL);
	if (!window) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	/*register callback functions*/
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetKeyCallback(window, key_callback);                                                                  //    
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	initializedGL();

	while (!glfwWindowShouldClose(window)) {
		/* Render here */
		paintGL();

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	glfwTerminate();

	return 0;
}
