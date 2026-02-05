#pragma once
#include <vector>
#include <GL/glew.h>

struct Vertex {
    float x, y, z;
    float nx, ny, nz;
    float u, v;
};

class Mesh {
public:
    Mesh(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices);
    void Draw();

private:
    unsigned int VAO, VBO, EBO;
};