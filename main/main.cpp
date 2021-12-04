#ifdef __APPLE__
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#elif defined _WIN32 || defined _WIN64
#include "Dependencies\glew\glew.h"
//#include "Dependencies\freeglut\include\GL\freeglut.h"
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
vec3 position;
int left_press_num = 0;
int right_press_num = 0;
bool click = false;
// key input
bool up_key = false;
bool right_key = false;
bool down_key = false;
bool left_key = false;
bool w_key = false;
bool s_key = false;
float speed = 0.1f;

//skybox params
GLuint skybox_vao, skybox_vbo, earth_cubemapTexture, spacecraftTexture, localCraftTexture, alertTexture, goldTexture, rockTexture;

//planet rotation
float timer, planetRotation, asteroidRotation, craftRotation;
float threshold = 1.0;

//light parameters
float ambient = 0.15;
float diffuse = 0.65;
float specular = 0.35;

bool flag = false;
bool alert[CRAFTS];


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

    // define gold object

    alertTexture = loadTexture("texture/red.bmp");
    goldTexture = loadTexture("texture/gold.bmp");
    rockTexture = loadTexture("texture/rockTexture.bmp");
    
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
}


void paintGL(void) {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glUseProgram(programID);
    
    glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraFront));
    if(up_key) cameraPos += cameraFront*speed;
    if(down_key) cameraPos -= cameraFront*speed;
    if(left_key) cameraPos += cameraRight*speed;
    if(right_key) cameraPos -= cameraRight*speed;
    
    glm::mat4 viewMatrix = glm::lookAt(cameraPos + vec3(0.0f, 0.2f, 0.0f) -cameraFront, cameraPos + vec3(0.0f, 0.2f, 0.0f), cameraUp);
    setMat4(programID, "view", viewMatrix);
    glm::mat4 projectionMatrix = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH/(float)SCR_HEIGHT, 0.1f, 500.0f);
    setMat4(programID, "projection", projectionMatrix);

    glm::mat4 modelTransformMatrix;

    //// spacecraft modelling
    ///
    modelTransformMatrix = mat4(1.0f);
    modelTransformMatrix = glm::translate(modelTransformMatrix, cameraPos);
    modelTransformMatrix = glm::rotate(modelTransformMatrix, glm::radians(180.0f), vec3(0, 1, 0));
    modelTransformMatrix = glm::rotate(modelTransformMatrix, -glm::radians(yaw), vec3(0, 1, 0));
    modelTransformMatrix = glm::scale(modelTransformMatrix, vec3(0.0005f, 0.0005f, 0.0005f));
    glm::mat4 spacecraftModel = modelTransformMatrix;

    //lights
    setFloat(programID,"ambientControl", ambient);
    setFloat(programID,"diffuseControl", diffuse);
    setFloat(programID,"specularControl", specular);
    setVec3(programID,"viewPort", cameraPos);
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
//    timer = (float)Get(GLUT_ELAPSED_TIME) / 25;
    timer = glfwGetTime()*10;
    planetRotation = timer;
    asteroidRotation = timer / 100;
    craftRotation = timer * 2.7;

    glm::mat4 rotateMatrix;
    glm::mat4 scaleMatrix;
    glm::mat4 translateMatrix;
    
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
        if (collisionTest(modelTransformMatrix, spacecraftModel, 2.0)){
            flag = true;
        }
        if (flag == false){
            asteroids[i].texture = goldTexture;
        }else{
            asteroids[i].texture = rockTexture;
        }
        setMat4(programID, "model", modelTransformMatrix);
        drawVAO(asteroids[i]);
    }



    // spacecraft
    if (flag) spacecraft.texture = goldTexture;
    else spacecraft.texture = spacecraftTexture;
    setMat4(programID, "model", spacecraftModel);
    drawVAO(spacecraft);

    //local space veicles
    for (int i = 0; i <CRAFTS; i++) {
        translateMatrix = glm::translate(mat4(1.0f), vec3(i * i * i - 1.2 * i * i + 0.5, 0.0, 5 * i - 15));
        rotateMatrix = glm::rotate(mat4(1.0f), glm::radians(craftRotation), vec3(0, 1, 0));
        scaleMatrix = glm::scale(mat4(1.0f), vec3(0.2, 0.2, 0.2));
        modelTransformMatrix = translateMatrix * rotateMatrix * scaleMatrix;
        setMat4(programID, "model", modelTransformMatrix);
        if (collisionTest(modelTransformMatrix, spacecraftModel, 5.0)) {
            alert[i] = true;
        }
        if (alert[i]) {
            crafts[i].texture = alertTexture;
        }
        else {
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
    float xoffset = x - lastX;
    float yoffset = lastY - y;
    lastX = x;
    lastY = y;
    
    float sensitivity = 0.2f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw   += xoffset;
//        pitch += yoffset;
//
//        if(pitch > 89.0f)
//            pitch = 89.0f;
//        if(pitch < -89.0f)
//            pitch = -89.0f;

    glm::vec3 direction;
    direction.x = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = -cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(direction);

}



void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    // Sets the Keyboard callback for the current window.
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS){
        glfwSetWindowShouldClose(window, true);
    }
    if(key == GLFW_KEY_LEFT && action == GLFW_PRESS){
        left_key = true;
    }
    if(key == GLFW_KEY_RIGHT && action == GLFW_PRESS){
        right_key = true;
    }
    if (key == GLFW_KEY_UP && action == GLFW_PRESS){
        up_key = true;
    }
    if (key == GLFW_KEY_DOWN && action == GLFW_PRESS){
        down_key = true;
    }
    if(key == GLFW_KEY_LEFT && action == GLFW_RELEASE) left_key = false;
    if(key == GLFW_KEY_RIGHT && action == GLFW_RELEASE) right_key = false;
    if(key == GLFW_KEY_UP && action == GLFW_RELEASE) up_key = false;
    if(key == GLFW_KEY_DOWN && action == GLFW_RELEASE) down_key = false;
    if (key == GLFW_KEY_N && action == GLFW_PRESS) planet.normalMapping = !planet.normalMapping;
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
int main(int argc, char** argv)
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
    
    //tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
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
