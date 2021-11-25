#ifdef __APPLE__
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#elif defined _WIN32 || defined _WIN64
#include "Dependencies/glew/glew.h"
#include "Dependencies/GLFW/glfw3.h"
#include "Dependencies/glm/glm.hpp"
#include "Dependencies/glm/gtc/matrix_transform.hpp"
#endif

#define STB_IMAGE_IMPLEMENTATION
#include "Shader.h"
#include "Shader.cpp"
#include "Texture.h"
#include "Texture.cpp"


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
GLfloat lastX = SCR_WIDTH / 2.0f;
GLfloat lastY = SCR_HEIGHT / 2.0f;
Shader generalShader;
object obj;
GLuint tex;

//camera setting
glm::vec3 cameraPos   = glm::vec3(0.0f, 2.0f,  5.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f,  0.0f);
glm::vec3 cameraDirection = glm::normalize(cameraTarget - cameraPos);
glm::vec3 up = glm::vec3(0.0f, 1.0f,  0.0f);
glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));
glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);

// mouse input
bool firstMouse = true;
GLfloat yaw = 0;
GLfloat pitch = 0;
int left_press_num = 0;
int right_press_num = 0;
bool click = false;
// key input
int up_key_num = 0;
int right_key_num = 0;
int w_press_num = 0;
int s_press_num = 0;

//light parameters
float ambient = 0.15;
float diffuse = 0.65;
float specular = 0.35;


// pipelines (buffers) for objects to draw
pipeline planet;



//
//
//********** Define utility functions below **********

// incapsulate the matrix definition for OpenGL objects here.
// refer to the codebase https://github.com/sinyiwsy/CSCI-3260-Assignment2/blob/master/main.cpp, Line 744
void setupTransformMatrix(string obj) {

}

void drawVAO(pipeline buffer) {
    generalShader.setInt("mapping", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, buffer.texture);
    if (buffer.normalMapping) {
        generalShader.setBool("normalMapping", true);
        generalShader.setInt("mapping_N", 1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, buffer.normalTexture);
    }
    else generalShader.setBool("normalMapping", false);
    glBindVertexArray(buffer.vao);
    glDrawArrays(GL_TRIANGLES, 0, buffer.size);
}





//
//
//********** The OpenGL pipeline functions **********
void sendDataToOpenGL() {
    obj = loadOBJ("object/planet.obj");
    tex = loadTexture("texture/earthTexture.bmp");
    generateBuffer(obj, &planet, tex);
    planet.normalTexture = loadTexture("texture/earthNormal.bmp");
    planet.normalMapping = true;
    clear(&obj);
    std::cout << "Finished sendDataToOpenGL" << std::endl;
}


void paintGL(void) {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearDepth(1.0f);

    // spacecraft modelling
    glm::mat4 translateMatrix = glm::translate(mat4(), vec3(up_key_num, 0.5, 20 + right_key_num));
    glm::mat4 rotateMatrix = glm::rotate(mat4(), glm::radians(45.0f), vec3(0, 1, 0));
    glm::mat4 scaleMatrix = glm::scale(mat4(), vec3(0.0005f, 0.0005f, 0.0005f));
    glm::mat4 spacecraftModel = translateMatrix * rotateMatrix;
    // world space modelling
    glm::vec4 camera = spacecraftModel * vec4(0.0f, 0.5f, 0.8f, 1.0f);
    glm::vec4 viewport = spacecraftModel * vec4(0.0f, 0.0f, -0.8f, 1.0f);

    glm::mat4 viewMatrix = glm::lookAt(glm::vec3(camera), glm::vec3(viewport), glm::vec3(0, 1, 0));
    generalShader.setMat4("view", viewMatrix);
    
    glm::mat4 projectionMatrix = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH/(float)SCR_HEIGHT, 0.1f, 500.0f);
    generalShader.setMat4("projection", projectionMatrix);
    
    //lights
    generalShader.setFloat("ambientControl", ambient);
    generalShader.setFloat("diffuseControl", diffuse);
    generalShader.setFloat("specularControl", specular);
    generalShader.setVec3("viewPort", glm::vec3(up_key_num, 1.25, 23 + right_key_num));
    generalShader.setVec3("lightSource[0]", glm::vec3(-10.0f, 15.0f, 25.0f));
    generalShader.setVec3("lightColor[0]", glm::vec3(1.0f, 1.0f, 1.0f));
    generalShader.setVec3("lightSource[1]", glm::vec3(0.0f, 15.0f, 0.0f));
    generalShader.setVec3("lightColor[1]", glm::vec3(0.5f, 0.5f, 0.5f));
    generalShader.setVec3("lightSource[2]", glm::vec3(10.0f, 15.0f, -25.0f));
    generalShader.setVec3("lightColor[2]", glm::vec3(1.0f, 0.5f, 0.0f));



    //camera
    
    
    //the planet object
    glm::mat4 modelTransformMatrix;
    translateMatrix = glm::translate(mat4(), vec3(0, 0, -20));
    rotateMatrix = glm::rotate(mat4(), glm::radians(45.0f), vec3(0, 1, 0));
    scaleMatrix = glm::scale(mat4(), vec3(0.5f, 0.5f, 0.5f));

    
    modelTransformMatrix = translateMatrix * rotateMatrix * scaleMatrix;
    generalShader.setMat4("model", modelTransformMatrix);
    drawVAO(planet);
    std::cout << "Finished paintGL" << std::endl;
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
    generalShader.setupShader("VertexShaderCode.glsl", "FragmentShaderCode.glsl");
    generalShader.use();
//    installShaders();
    
	sendDataToOpenGL();



	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glDepthFunc(GL_LESS);
}

void cursor_position_callback(GLFWwindow* window, double x, double y) {
    /*
    float xoffset = 0.0f;
    float yoffset = 0.0f;
    if (click){
        // Sets the cursor position callback for the current window
        xoffset = x - lastX;
        yoffset = lastY - y;
    }
    lastX = x;
    lastY = y;
    if (click){
        float sensitivity = 0.2f;
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        yaw   += xoffset;
        pitch += yoffset;

        if(pitch > 89.0f)
            pitch = 89.0f;
        if(pitch < -89.0f)
            pitch = -89.0f;

        glm::vec3 direction;
        direction.x = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        direction.y = sin(glm::radians(pitch));
        direction.z = -cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        cameraFront = glm::normalize(direction);
    }
    */
}



void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    // Sets the Keyboard callback for the current window.
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS){
        glfwSetWindowShouldClose(window, true);
    }
    if(key == GLFW_KEY_LEFT && action == GLFW_PRESS) right_key_num -= 1;
    if(key == GLFW_KEY_RIGHT && action == GLFW_PRESS) right_key_num += 1;
    if (key == GLFW_KEY_UP && action == GLFW_PRESS) up_key_num += 1;
    if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) up_key_num -= 1;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {

}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {

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

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "project", NULL, NULL);
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
