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

class Shader {
public:
	void setupShader(const char* vertexPath, const char* fragmentPath);
	void use() const;

	// a series utilities for setting shader parameters 
	void setMat4(const std::string& name, glm::mat4& value) const;
	void setVec4(const std::string& name, glm::vec4 value) const;
	void setVec3(const std::string& name, glm::vec3 value) const;
	void setVec3(const std::string& name, float v1, float v2, float v3) const;
	void setFloat(const std::string& name, float value) const;
	void setInt(const std::string& name, int value) const;
    void setBool(const std::string& name, bool value) const;

private:
	GLuint ID = 0;

	std::string readShaderCode(const char* fileName) const;
	bool checkShaderStatus(GLuint shaderID) const;
	bool checkProgramStatus(GLuint programID) const;
	bool checkStatus(
		GLuint objectID,
		PFNGLGETSHADERIVPROC objectPropertyGetterFunc,
		PFNGLGETSHADERINFOLOGPROC getInfoLogFunc,
		GLenum statusType) const;
};


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

