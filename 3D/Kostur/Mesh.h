#pragma once
#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>

struct Vertex {
    float x, y, z;     
    float r, g, b, a;   
    float u, v;         
};

class Mesh {
public:
    Mesh(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices);
    void Draw();

private:
    unsigned int VAO, VBO, EBO;
    unsigned int indexCount;
};