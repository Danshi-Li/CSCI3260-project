#ifdef __APPLE__
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#elif defined _WIN32 || defined _WIN64
#include "Dependencies\glew\glew.h"
#include "Dependencies\freeglut\include\GL\freeglut.h"
#include "Dependencies\glm\glm.hpp"
#include "Dependencies\glm\gtc\matrix_transform.hpp"
#include "Dependencies\glm\gtc\type_ptr.hpp"
#endif

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
#include "Shader.h"




//********** Utility classes should be defined inseparate .h/.cpp flies.
//           Include external classes below                             **********




//
// 
//********** Define utility global variables below **********
const int SCR_WIDTH = 800;
const int SCR_HEIGHT = 600;
GLfloat lastX = SCR_WIDTH / 2.0f;
GLfloat lastY = SCR_HEIGHT / 2.0f;
#define ASTEROID 200
#define CRAFTS 3

GLint programID, skyboxID;
pipeline planet,asteroids[ASTEROID], spacecraft, crafts[CRAFTS];

//camera setting
glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f,  5.0f);
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

//skybox params
GLuint skybox_vao, skybox_vbo, earth_cubemapTexture, spacecraftTexture, localCraftTexture, alertTexture;

//planet rotation
float timer, planetRotation, asteroidRotation, craftRotation;
float threshold = 1.0;

//light parameters
float ambient = 0.15;
float diffuse = 0.65;
float specular = 0.35;

bool flag = false;


//
//
//********** Define utility functions below **********

// incapsulate the matrix definition for OpenGL objects here.
// refer to the codebase https://github.com/sinyiwsy/CSCI-3260-Assignment2/blob/master/main.cpp, Line 744
void drawVAO(pipeline buffer) {
    setInt(programID, "mapping", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, buffer.texture);
    if (buffer.normalMapping) {
        setBool(programID, "normalMapping", true);
        setInt(programID, "mapping_N", 1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, buffer.normalTexture);
    }
    else {
        setBool(programID, "normalMapping", false);
    }
    glBindVertexArray(buffer.vao);
    glDrawArrays(GL_TRIANGLES, 0, buffer.size);
}

bool collisionTest(glm::mat4 matrixA, glm::mat4 matrixB, int threshold)
{
    vec4 vectorA = matrixA * vec4(0, 0, 0, 1);
    vec4 vectorB = matrixB * vec4(0, 0, 0, 1);
    if (length(vectorA - vectorB) <= threshold)
        return true;
    else return false;
}

void checkCollision(pipeline* buffer, glm::mat4 model, glm::mat4 spacecraft, int threshold)
{
    if (buffer->collision) return;
    if (collisionTest(model, spacecraft, threshold)) {
        buffer->collision = true;
        return;
    }
    setMat4(programID, "model", model);
    drawVAO(*buffer);
}


//
//
//********** The OpenGL pipeline functions **********
void sendDataToOpenGL() {
    object obj;
    GLuint tex;
    // define skybox
    GLfloat skybox_vertices[] = {
        // Front
        +50.0f, +50.0f, -50.0f,
        -50.0f, +50.0f, -50.0f,
        +50.0f, -50.0f, -50.0f,
        -50.0f, -50.0f, -50.0f,
        +50.0f, -50.0f, -50.0f,
        -50.0f, +50.0f, -50.0f,
        // Back
        +50.0f, +50.0f, +50.0f,
        -50.0f, +50.0f, +50.0f,
        +50.0f, -50.0f, +50.0f,
        -50.0f, -50.0f, +50.0f,
        +50.0f, -50.0f, +50.0f,
        -50.0f, +50.0f, +50.0f,
        // Bottom
        +50.0f, -50.0f, +50.0f,
        -50.0f, -50.0f, +50.0f,
        +50.0f, -50.0f, -50.0f,
        -50.0f, -50.0f, -50.0f,
        +50.0f, -50.0f, -50.0f,
        -50.0f, -50.0f, +50.0f,
        // Top
        +50.0f, +50.0f, +50.0f,
        -50.0f, +50.0f, +50.0f,
        +50.0f, +50.0f, -50.0f,
        -50.0f, +50.0f, -50.0f,
        +50.0f, +50.0f, -50.0f,
        -50.0f, +50.0f, +50.0f,
        // Left
        -50.0f, +50.0f, +50.0f,
        -50.0f, +50.0f, -50.0f,
        -50.0f, -50.0f, +50.0f,
        -50.0f, -50.0f, -50.0f,
        -50.0f, -50.0f, +50.0f,
        -50.0f, +50.0f, -50.0f,
        // Right
        +50.0f, +50.0f, +50.0f,
        +50.0f, +50.0f, -50.0f,
        +50.0f, -50.0f, +50.0f,
        +50.0f, -50.0f, -50.0f,
        +50.0f, -50.0f, +50.0f,
        +50.0f, +50.0f, -50.0f,

    };
    vector<const GLchar*> earth_faces;
    earth_faces.push_back("skybox/right.bmp");
    earth_faces.push_back("skybox/left.bmp");
    earth_faces.push_back("skybox/top.bmp");
    earth_faces.push_back("skybox/bottom.bmp");
    earth_faces.push_back("skybox/back.bmp");
    earth_faces.push_back("skybox/front.bmp");
    earth_cubemapTexture = loadCubeMap(earth_faces);
    // create skybox vao & vbo
    glGenVertexArrays(1, &skybox_vao);
    glBindVertexArray(skybox_vao);
    glGenBuffers(1, &skybox_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, skybox_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skybox_vertices), &skybox_vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);


    
    // define planet
    loadOBJ("object/planet.obj", &obj);
    tex = loadTexture("texture/earthTexture.bmp");
    generateBuffer(obj, &planet, tex);
    planet.normalTexture = loadTexture("texture/earthNormal.bmp");
    planet.normalMapping = true;
    clear(&obj);

    // define our own spacecraft
    loadOBJ("object/spacecraft.obj", &obj);
    spacecraftTexture = loadTexture("texture/spacecraftTexture.bmp");
    generateBuffer(obj, &spacecraft, spacecraftTexture);
    clear(&obj);


    // define asteroid ring
    loadOBJ("object/rock.obj", &obj);
    tex = loadTexture("texture/rockTexture.bmp");
    for (int i = 0; i < ASTEROID; i++)
        generateBuffer(obj, &asteroids[i], tex);
    clear(&obj);

    // define local vehicles
    loadOBJ("object/craft.obj", &obj);
    localCraftTexture = loadTexture("texture/ringTexture.bmp");
    for (int i = 0; i < CRAFTS; i++)
        generateBuffer(obj, &crafts[i], tex);
    clear(&obj);

    // define gold object

    alertTexture = loadTexture("texture/red.bmp");
}


void paintGL(void) {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glUseProgram(programID);
    
    //glm::mat4 viewMatrix = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    //setMat4(programID, "view", viewMatrix);
    //
    //glm::mat4 projectionMatrix = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH/(float)SCR_HEIGHT, 0.1f, 20.0f);
    //setMat4(programID, "projection", projectionMatrix);

    glm::mat4 modelTransformMatrix;

    //// spacecraft modelling
    glm::mat4 translateMatrix = glm::translate(mat4(1.0f), vec3(right_key_num, 0.5, 20 - up_key_num));
    glm::mat4 rotateMatrix = glm::rotate(mat4(1.0f), glm::radians(0.0f), vec3(0, 1, 0));
    glm::mat4 scaleMatrix = glm::scale(mat4(1.0f), vec3(0.0005f, 0.0005f, 0.0005f));
    glm::mat4 spacecraftModel = translateMatrix * rotateMatrix;
    // world space modelling
    glm::vec4 camera = spacecraftModel * vec4(0.0f, 0.5f, 0.8f, 1.0f);
    glm::vec4 viewport = spacecraftModel * vec4(0.0f, 0.0f, -0.8f, 1.0f);

    glm::mat4 viewMatrix = glm::lookAt(glm::vec3(camera), glm::vec3(viewport), glm::vec3(0, 1, 0));
    setMat4(programID,"view", viewMatrix);
    glm::mat4 projectionMatrix = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH/(float)SCR_HEIGHT, 0.1f, 500.0f);
    setMat4(programID,"projection", projectionMatrix);
    spacecraftModel = spacecraftModel * glm::rotate(mat4(1.0f), glm::radians(180.0f), vec3(0, 1, 0)) * scaleMatrix;

    //lights
    setFloat(programID,"ambientControl", ambient);
    setFloat(programID,"diffuseControl", diffuse);
    setFloat(programID,"specularControl", specular);
    setVec3(programID,"viewPort", glm::vec3(right_key_num, 1.25, 23 -up_key_num));
    setVec3(programID,"lightSource[0]", glm::vec3(-10.0f, 15.0f, 25.0f));
    setVec3(programID,"lightColor[0]", glm::vec3(1.0f, 1.0f, 1.0f));
    setVec3(programID,"lightSource[1]", glm::vec3(0.0f, 15.0f, 0.0f));
    setVec3(programID,"lightColor[1]", glm::vec3(0.5f, 0.5f, 0.5f));
    setVec3(programID,"lightSource[2]", glm::vec3(10.0f, 15.0f, -25.0f));
    setVec3(programID,"lightColor[2]", glm::vec3(1.0f, 0.5f, 0.0f));

    //skybox
    glDepthMask(GL_FALSE);
    glUseProgram(skyboxID);
    setMat4(skyboxID, "projection", projectionMatrix);
    setMat4(skyboxID, "view", viewMatrix);

    // skybox
    setInt(skyboxID, "mapping", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, earth_cubemapTexture);
    glBindVertexArray(skybox_vao);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthMask(GL_TRUE);

    glEnable(GL_DEPTH_TEST);
    glUseProgram(programID);
    setMat4(programID, "projection", projectionMatrix);
    setMat4(programID, "view", viewMatrix);



    //timer for rotation
    timer = (float)glutGet(GLUT_ELAPSED_TIME) / 25;
    planetRotation = timer;
    asteroidRotation = timer / 100;
    craftRotation = timer * 2.7;

    
    
    //the planet object
    translateMatrix = glm::translate(mat4(1.0f), vec3(0, 0, -20));
    rotateMatrix = glm::rotate(mat4(1.0f), glm::radians(planetRotation), vec3(0, 1, 0));
    scaleMatrix = glm::scale(mat4(1.0f), vec3(0.5f, 0.5f, 0.5f));
    modelTransformMatrix = translateMatrix * rotateMatrix * scaleMatrix;
    setMat4(programID,"model", modelTransformMatrix);
    drawVAO(planet);

   //the asteroids
   srand(3260);
    GLfloat radius = 4;
    for (int i = 0; i < ASTEROID; i++) {
        GLfloat offset = rand() % 10 / 30.0f;
        GLfloat theta = (float)i / (float)ASTEROID * 360 + asteroidRotation;
        translateMatrix = glm::translate(mat4(1.0f), vec3(sin(theta) * radius + offset, 0.55 - offset, -20 + cos(theta) * radius * 0.5 + offset));
        GLfloat phi = rand() % 360 + planetRotation;
        rotateMatrix = glm::rotate(mat4(1.0f), glm::radians(phi), vec3(offset, offset, offset));
        GLfloat scale = rand() % 10 / 200.0f;
        scaleMatrix = glm::scale(mat4(1.0f), vec3(scale, scale, scale));
        modelTransformMatrix = translateMatrix * rotateMatrix * scaleMatrix;
        setMat4(programID, "model", modelTransformMatrix);
        drawVAO(asteroids[i]);
    }

    //gold

    // spacecraft
    setMat4(programID, "model", spacecraftModel);
    spacecraft.texture = spacecraftTexture;
    drawVAO(spacecraft);

    //local space veicles
    for (int i = 0; i <CRAFTS; i++) {
        translateMatrix = glm::translate(mat4(1.0f), vec3(i * i * i - 1.2 * i * i + 0.5, 0.0, 5 * i - 15));
        rotateMatrix = glm::rotate(mat4(1.0f), glm::radians(craftRotation), vec3(0, 1, 0));
        scaleMatrix = glm::scale(mat4(1.0f), vec3(0.2, 0.2, 0.2));
        modelTransformMatrix = translateMatrix * rotateMatrix * scaleMatrix;
        setMat4(programID, "model", modelTransformMatrix);
        if (collisionTest(modelTransformMatrix, spacecraftModel, 5.0)) {
            flag = true;
            crafts[i].texture = alertTexture;
        }
        else {
            flag = false;
            crafts[i].texture = localCraftTexture;
        }

        drawVAO(crafts[i]);
    }




//    std::cout << "Finished paintGL" << std::endl;
    glFlush();
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void initializedGL(void)
{
    if (glewInit() != GLEW_OK) {
        std::cout << "GLEW not OK." << std::endl;
    }
    get_OpenGL_info();
    programID = installShaders("VertexShaderCode.glsl", "FragmentShaderCode.glsl");
    if (programID == 0)     std::cout << "fuck" << std::endl;
        
    skyboxID = installShaders("SkyboxVertexShaderCode.glsl", "SkyboxFragmentShaderCode.glsl");
    sendDataToOpenGL();
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glDepthFunc(GL_LESS);
}

void cursor_position_callback(GLFWwindow* window, double x, double y) {
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
    if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS){
        click = true;
    }
    if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE){
        click = false;
//        std::cout << cameraFront.x << std::endl;
//        std::cout << cameraFront.y << std::endl;
//        std::cout << cameraFront.z << std::endl;
    }
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
