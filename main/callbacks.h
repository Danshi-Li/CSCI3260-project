#ifndef INTERACTION_H
#define INTERACTION_H

#include "Dependencies\glew\glew.h"
#include "Dependencies\freeglut\freeglut.h"
#include "Dependencies\glm\glm.hpp"
#include "Dependencies\glm\gtc\matrix_transform.hpp"
#include "Dependencies\glm\gtc\type_ptr.hpp"

#include <iostream>
#include <fstream>
#include <vector>

using namespace std;
using glm::vec3;
using glm::vec4;
using glm::mat4;


//
// 
//********** Event specific callbacks **********
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {

}


void cursor_position_callback(GLFWwindow* window, double x, double y) {

}


void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {

}


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {

}