#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>

int endProgram(std::string message);
unsigned int createShader(const char* vsSource, const char* fsSource);
unsigned loadImageToTexture(const char* filePath);
GLFWcursor* loadImageToCursor(const char* filePath);
void drawTexturePixels(GLuint shader, GLuint texture, float x, float y, float w, float h, int fbW, int fbH);
void drawTextureFlipped(GLuint shader, GLuint texture, float x, float y, float w, float h, bool flip, int fbW, int fbH);