#pragma once

#ifdef __APPLE__
#include "GL/glew.h"
#include "glm/glm.hpp"
#elif defined _WIN32 || defined _WIN64
#include "Dependencies\glew\glew.h"
#include "Dependencies\glm\glm.hpp"
#endif

#include <string>
#include <iostream>

// a series utilities for setting shader parameters
void setMat4(GLuint const programID, const std::string& name, glm::mat4 value)
{
	glUniformMatrix4fv(glGetUniformLocation(programID, name.c_str()), 1, GL_FALSE, &value[0][0]);
}
void setVec3(GLuint const programID, const std::string& name, glm::vec3 value)
{
	glUniform3fv(glGetUniformLocation(programID, name.c_str()), 1, &value[0]);
}
void setFloat(GLuint const programID, const std::string& name, float value)
{
	glUniform1f(glGetUniformLocation(programID, name.c_str()), value);
}
void setInt(GLuint const programID, const std::string& name, int value)
{
	glUniform1i(glGetUniformLocation(programID, name.c_str()), value);
}
void setBool(GLuint const programID, const std::string& name, bool value)
{
	glUniform1i(glGetUniformLocation(programID, name.c_str()), (int)value);
}

