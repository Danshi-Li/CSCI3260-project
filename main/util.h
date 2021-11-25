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
//#include "Texture.h"


#include <iostream>
#include <fstream>
#include <vector>
#include <map>

typedef struct PIPELINE {
	GLuint vao;
	GLuint vbo;
	GLuint uvbo;
	GLuint nbo;
	GLuint tbo;
	GLuint btbo;
	GLuint texture;
	GLuint normalTexture;
	int size;
	bool collision;
	bool normalMapping;
} pipeline;


typedef struct OBJECT {
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec3> tangents;
	std::vector<glm::vec3> bitangents;
} object;

/*
struct Model {
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
};
*/
/*
NOTE: this function is originally given in assignment2 in order to load object into the Model class as
Model loadOBJ(const char* objPath)
But we want to re-implement it to load into the object class, with several features added.
An implementation in https://gitlab.com/cynthia0525/csci3260/-/blob/master/Course%20Project/model.h is accessible. Ned to extend this function.
*/
object loadOBJ(const char* path)
{
    object obj;
    printf("Loading OBJ file %s...\n", path);

    std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
    std::vector<glm::vec3> temp_vertices;
    std::vector<glm::vec2> temp_uvs;
    std::vector<glm::vec3> temp_normals;

    FILE * file = fopen(path, "r");
    if (file == NULL) {
        printf("Impossible to open the file ! Are you in the right path ? See Tutorial 6 for details\n");
        getchar();
        exit(0);
    }

    while (1) {

        char lineHeader[128];
        int res = fscanf(file, "%s", lineHeader);
        if (res == EOF)
            break; // EOF = End Of File. Quit the loop.

                   // else : parse lineHeader

        if (strcmp(lineHeader, "v") == 0) {
            glm::vec3 vertex;
            fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
            temp_vertices.push_back(vertex);
        }
        else if (strcmp(lineHeader, "vt") == 0) {
            glm::vec2 uv;
            fscanf(file, "%f %f\n", &uv.x, &uv.y);
            uv.y = -uv.y;
            temp_uvs.push_back(uv);
        }
        else if (strcmp(lineHeader, "vn") == 0) {
            glm::vec3 normal;
            fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
            temp_normals.push_back(normal);
        }
        else if (strcmp(lineHeader, "f") == 0) {
            std::string vertex1, vertex2, vertex3;
            unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
            int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
            if (matches != 9) {
                printf("File can't be read by our simple parser :-( Try exporting with other options\n");
                exit(0);
            }
            vertexIndices.push_back(vertexIndex[0]);
            vertexIndices.push_back(vertexIndex[1]);
            vertexIndices.push_back(vertexIndex[2]);
            uvIndices.push_back(uvIndex[0]);
            uvIndices.push_back(uvIndex[1]);
            uvIndices.push_back(uvIndex[2]);
            normalIndices.push_back(normalIndex[0]);
            normalIndices.push_back(normalIndex[1]);
            normalIndices.push_back(normalIndex[2]);
        }
        else {
            char stupidBuffer[1000];
            fgets(stupidBuffer, 1000, file);
        }

    }

    glm::vec3 tangent, bitangent;
    // For each vertex of each triangle
    for (unsigned int i = 0; i < vertexIndices.size(); i++) {

        // Get the indices of its attributes
        unsigned int vertexIndex = vertexIndices[i];
        unsigned int uvIndex = uvIndices[i];
        unsigned int normalIndex = normalIndices[i];

        // Get the attributes thanks to the index
        glm::vec3 vertex = temp_vertices[vertexIndex - 1];
        glm::vec2 uv = temp_uvs[uvIndex - 1];
        glm::vec3 normal = temp_normals[normalIndex - 1];

        // Calculate tangent & bitangent
        if (i % 3 == 0) {
            glm::vec3 & v0 = temp_vertices[vertexIndices[i + 0] - 1];
            glm::vec3 & v1 = temp_vertices[vertexIndices[i + 1] - 1];
            glm::vec3 & v2 = temp_vertices[vertexIndices[i + 2] - 1];
            // Position delta
            glm::vec3 deltaPos1 = v1 - v0;
            glm::vec3 deltaPos2 = v2 - v0;

            glm::vec2 & uv0 = temp_uvs[uvIndices[i + 0] - 1];
            glm::vec2 & uv1 = temp_uvs[uvIndices[i + 1] - 1];
            glm::vec2 & uv2 = temp_uvs[uvIndices[i + 2] - 1];
            // UV delta
            glm::vec2 deltaUV1 = uv1 - uv0;
            glm::vec2 deltaUV2 = uv2 - uv0;

            float factor = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
            tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * factor;
            bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * factor;
        }

        // Put the attributes in buffers
        obj.vertices.push_back(vertex);
        obj.uvs.push_back(uv);
        obj.normals.push_back(normal);
        obj.tangents.push_back(tangent);
        obj.bitangents.push_back(bitangent);
    }

    return obj;
}

unsigned char* loadBMP(const char* imagepath, unsigned int* width, unsigned int* height)
{
    printf("Reading image %s\n", imagepath);

    unsigned char header[54];
    unsigned int dataPos;
    unsigned int imageSize;
    unsigned char* data;

    FILE* file = fopen(imagepath, "rb");
    if (!file) { printf("%s could not be opened. Are you in the right directory ? Don't forget to read the FAQ !\n", imagepath); getchar(); return 0; }

    if (fread(header, 1, 54, file) != 54) {
        printf("Not a correct BMP file\n");
        return 0;
    }
    if (header[0] != 'B' || header[1] != 'M') {
        printf("Not a correct BMP file\n");
        return 0;
    }
    if (*(int*)&(header[0x1E]) != 0) { printf("Not a correct BMP file\n");    return 0; }
    if (*(int*)&(header[0x1C]) != 24) { printf("Not a correct BMP file\n");    return 0; }

    dataPos = *(int*)&(header[0x0A]);
    imageSize = *(int*)&(header[0x22]);
    *width = *(int*)&(header[0x12]);
    *height = *(int*)&(header[0x16]);
    if (imageSize == 0)    imageSize = *width * *height * 3;
    if (dataPos == 0)      dataPos = 54;

    data = new unsigned char[imageSize];
    fread(data, 1, imageSize, file);
    fclose(file);

    return data;
}


GLuint loadTexture(const char* path){
    unsigned int width, height;
    unsigned char* image;
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    image = loadBMP(path, &width, &height);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, image);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);
    delete[] image;
    return textureID;

}


bool checkStatus(
	GLuint objectID,
	PFNGLGETSHADERIVPROC objectPropertyGetterFunc,
	PFNGLGETSHADERINFOLOGPROC getInfoLogFunc,
	GLenum statusType)
{
	GLint status;
	objectPropertyGetterFunc(objectID, statusType, &status);
	if (status != GL_TRUE)
	{
		GLint infoLogLength;
		objectPropertyGetterFunc(objectID, GL_INFO_LOG_LENGTH, &infoLogLength);
		GLchar* buffer = new GLchar[infoLogLength];

		GLsizei bufferSize;
		getInfoLogFunc(objectID, infoLogLength, &bufferSize, buffer);
		std::cout << buffer << std::endl;

		delete[] buffer;
		return false;
	}
	return true;
}

bool checkShaderStatus(GLuint shaderID) {
	return checkStatus(shaderID, glGetShaderiv, glGetShaderInfoLog, GL_COMPILE_STATUS);
}

bool checkProgramStatus(GLuint programID) {
	return checkStatus(programID, glGetProgramiv, glGetProgramInfoLog, GL_LINK_STATUS);
}

std::string readShaderCode(const char* fileName) {
	std::ifstream meInput(fileName);
	if (!meInput.good()) {
		std::cout << "File failed to load ... " << fileName << std::endl;
		exit(1);
	}
	return std::string(
		std::istreambuf_iterator<char>(meInput),
		std::istreambuf_iterator<char>()
	);
}

//void installShaders() {
//	GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
//	GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
//
//	const GLchar* adapter[1];
//	//adapter[0] = vertexShaderCode;
//	std::string temp = readShaderCode("VertexShader.glsl");
//	adapter[0] = temp.c_str();
//	glShaderSource(vertexShaderID, 1, adapter, 0);
//	//adapter[0] = fragmentShaderCode;
//	temp = readShaderCode("FragmentShader.glsl");
//	adapter[0] = temp.c_str();
//	glShaderSource(fragmentShaderID, 1, adapter, 0);
//
//	glCompileShader(vertexShaderID);
//	glCompileShader(fragmentShaderID);
//
//	if (!checkShaderStatus(vertexShaderID) || !checkShaderStatus(fragmentShaderID))
//		return;
//
//	programID = glCreateProgram();
//	glAttachShader(programID, vertexShaderID);
//	glAttachShader(programID, fragmentShaderID);
//	glLinkProgram(programID);
//
//	if (!checkProgramStatus(programID))
//		return;
//	glUseProgram(programID);
//
//}

void get_OpenGL_info()
{
	// OpenGL information
	const GLubyte* name = glGetString(GL_VENDOR);
	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* glversion = glGetString(GL_VERSION);
	std::cout << "OpenGL company: " << name << std::endl;
	std::cout << "Renderer name: " << renderer << std::endl;
	std::cout << "OpenGL version: " << glversion << std::endl;
}
//
//void setMat4(GLuint const programID, const std::string& name, glm::mat4 value)
//{
//	glUniformMatrix4fv(glGetUniformLocation(programID, name.c_str()), 1, GL_FALSE, &value[0][0]);
//}
//void setVec3(GLuint const programID, const std::string& name, glm::vec3 value)
//{
//	glUniform3fv(glGetUniformLocation(programID, name.c_str()), 1, &value[0]);
//}
//void setFloat(GLuint const programID, const std::string& name, float value)
//{
//	glUniform1f(glGetUniformLocation(programID, name.c_str()), value);
//}
//void setInt(GLuint const programID, const std::string& name, int value)
//{
//	glUniform1i(glGetUniformLocation(programID, name.c_str()), value);
//}
//void setBool(GLuint const programID, const std::string& name, bool value)
//{
//	glUniform1i(glGetUniformLocation(programID, name.c_str()), (int)value);
//}


void generateBuffer(object obj, pipeline* buffer, GLuint texture) {
	//TODO: initialize the buffers corresponding to an object to be painted.
	//      refer to the function generateVAOVBO in https://gitlab.com/cynthia0525/csci3260/-/blob/master/Course%20Project/model.h to get an idea what we want to implement
    buffer->texture = texture;
    buffer->size = obj.vertices.size();
    buffer->collision = false;
    buffer->normalMapping = false;
    glGenVertexArrays(1, &buffer->vao);
    glBindVertexArray(buffer->vao);
    glGenBuffers(1, &buffer->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, buffer->vbo);
    glBufferData(GL_ARRAY_BUFFER, obj.vertices.size() * sizeof(glm::vec3), &obj.vertices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glGenBuffers(1, &buffer->uvbo);
    glBindBuffer(GL_ARRAY_BUFFER, buffer->uvbo);
    glBufferData(GL_ARRAY_BUFFER, obj.uvs.size() * sizeof(glm::vec2), &obj.uvs[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glGenBuffers(1, &buffer->nbo);
    glBindBuffer(GL_ARRAY_BUFFER, buffer->nbo);
    glBufferData(GL_ARRAY_BUFFER, obj.normals.size() * sizeof(glm::vec3), &obj.normals[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glGenBuffers(1, &buffer->tbo);
    glBindBuffer(GL_ARRAY_BUFFER, buffer->tbo);
    glBufferData(GL_ARRAY_BUFFER, obj.tangents.size() * sizeof(glm::vec3), &obj.tangents[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glGenBuffers(1, &buffer->btbo);
    glBindBuffer(GL_ARRAY_BUFFER, buffer->btbo);
    glBufferData(GL_ARRAY_BUFFER, obj.bitangents.size() * sizeof(glm::vec3), &obj.bitangents[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    

}

void clear(object* obj) {
    obj->vertices.clear();
    obj->uvs.clear();
    obj->normals.clear();
    obj->tangents.clear();
    obj->bitangents.clear();
}
