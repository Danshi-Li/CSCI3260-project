//#ifndef INTERACTION_H
//#define INTERACTION_H

#ifdef __APPLE__
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#elif defined _WIN32 || defined _WIN64
#include "Dependencies/glew/glew.h"
#include "Dependencies/GLFW/glfw3.h"
#include "Dependencies/glm/glm.hpp"
#include "Dependencies/glm/gtc/matrix_transform.hpp"
#include "Dependencies\glm\gtc\type_ptr.hpp"
#endif


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





void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {

}
