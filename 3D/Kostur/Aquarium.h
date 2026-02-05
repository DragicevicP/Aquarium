#pragma once
#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "Mesh.h"

class Aquarium {
public:
    Aquarium(unsigned int shaderProgram, unsigned int sandTexture);
    ~Aquarium();

    void Draw();

private:
    Mesh* glass;
    Mesh* borders;
    Mesh* bottom;
    Mesh* sand;

    unsigned int shader;
    unsigned int sandTex;

    void createGlass();
    void createBorders();
    void createBottom();
    void createSand();
};