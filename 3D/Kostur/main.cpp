#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <thread>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Util.h"

unsigned int shaderProgram;
unsigned int sandTexture;

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

struct Box {
    glm::vec3 min;
    glm::vec3 max;
};

void generateBoxMesh(Box b, unsigned int& VAO, unsigned int& VBO, unsigned int& EBO) {
    float vertices[] = {
        // Pozicija           // Boja (crna)       // UV
        b.min.x,b.min.y,b.min.z, 0,0,0,1, 0,0,
        b.max.x,b.min.y,b.min.z, 0,0,0,1, 1,0,
        b.max.x,b.max.y,b.min.z, 0,0,0,1, 1,1,
        b.min.x,b.max.y,b.min.z, 0,0,0,1, 0,1,

        b.min.x,b.min.y,b.max.z, 0,0,0,1, 0,0,
        b.max.x,b.min.y,b.max.z, 0,0,0,1, 1,0,
        b.max.x,b.max.y,b.max.z, 0,0,0,1, 1,1,
        b.min.x,b.max.y,b.max.z, 0,0,0,1, 0,1
    };
    unsigned int indices[] = {
        0,1,2,2,3,0, // prednja
        4,5,6,6,7,4, // zadnja
        0,1,5,5,4,0, // donja
        2,3,7,7,6,2, // gornja
        0,3,7,7,4,0, // leva
        1,2,6,6,5,1  // desna
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    unsigned int stride = 9 * sizeof(float);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(7 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

// Funkcija koja generiše nepravilno dno (pesak)
void generateSandMesh(unsigned int& VAO, unsigned int& VBO, unsigned int& EBO,
    std::vector<unsigned int>& indices) {

    const int GRID_SIZE = 50;
    const float SIZE = 9.6f;
    const float OFFSET = -SIZE / 2.0f;

    std::vector<float> vertices;
    srand((unsigned int)time(0));

    for (int z = 0; z <= GRID_SIZE; ++z) {
        for (int x = 0; x <= GRID_SIZE; ++x) {
            float xf = OFFSET + ((float)x / GRID_SIZE) * SIZE;
            float zf = OFFSET + ((float)z / GRID_SIZE) * SIZE;
            float yf = ((rand() % 100) / 1000.0f);

            // Pozicija
            vertices.push_back(xf);
            vertices.push_back(yf);
            vertices.push_back(zf);

            // Boja peska
            vertices.push_back(1.0f);
            vertices.push_back(0.9f);
            vertices.push_back(0.7f);
            vertices.push_back(1.0f);

            // Koordinate teksture
            vertices.push_back((float)x / GRID_SIZE * 5.0f);
            vertices.push_back((float)z / GRID_SIZE * 5.0f);
        }
    }

    for (int z = 0; z < GRID_SIZE; ++z) {
        for (int x = 0; x < GRID_SIZE; ++x) {
            int topLeft = z * (GRID_SIZE + 1) + x;
            int topRight = topLeft + 1;
            int bottomLeft = (z + 1) * (GRID_SIZE + 1) + x;
            int bottomRight = bottomLeft + 1;

            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);

            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
        }
    }

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    unsigned int stride = 9 * sizeof(float);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(7 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

int main() {
    if (!glfwInit()) {
        std::cout << "GLFW nije ucitan!\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);

    GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, "Aquarium 3D", primaryMonitor, nullptr);
    if (!window) {
        std::cout << "Prozor nije kreiran!\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, keyCallback);


    if (glewInit() != GLEW_OK) {
        std::cout << "GLEW nije ucitan!\n";
        return -1;
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);

    GLuint shaderProgram = createShader("basic.vert", "basic.frag");
   
    glUseProgram(shaderProgram);
    glUniform1i(glGetUniformLocation(shaderProgram, "uTex"), 0);

    // --------------------- Dno (plavi kvadar) ---------------------
    float floorVertices[] = {
       -5.0f, 0.0f, -5.0f,  0.0f,0.0f,0.2f,1.0f,   0,0,
        5.0f, 0.0f, -5.0f,  0.0f,0.0f,0.2f,1.0f,   1,0,
        5.0f, 0.0f,  5.0f,  0.0f,0.0f,0.2f,1.0f,   1,1,
       -5.0f, 0.0f,  5.0f,  0.0f,0.0f,0.2f,1.0f,   0,1
    };
    unsigned int floorIndices[] = { 0,1,2,2,3,0 };
    unsigned int floorVAO, floorVBO, floorEBO;
    glGenVertexArrays(1, &floorVAO);
    glGenBuffers(1, &floorVBO);
    glGenBuffers(1, &floorEBO);

    glBindVertexArray(floorVAO);
    glBindBuffer(GL_ARRAY_BUFFER, floorVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(floorVertices), floorVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, floorEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(floorIndices), floorIndices, GL_STATIC_DRAW);

    unsigned int stride = 9 * sizeof(float);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(7 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);

    // --------------------- Pesak ---------------------
    unsigned int sandVAO, sandVBO, sandEBO;
    std::vector<unsigned int> sandIndices;
    generateSandMesh(sandVAO, sandVBO, sandEBO, sandIndices);

    GLuint sandTexture = loadImageToTexture("res/sand.png");
   

    //Akvarijum (stakla + crne ivice)
    unsigned int glassVAO, glassVBO, glassEBO;
    float glassVertices[] = {
        // Prednji panel (Z = -5.0), X se smanji za 0.2 sa obe strane
        -4.8f, 0.0f, -5.0f, 0.8,0.9,1.0,0.2, 0,0,
         4.8f, 0.0f, -5.0f, 0.8,0.9,1.0,0.2, 1,0,
         4.8f, 5.0f, -5.0f, 0.8,0.9,1.0,0.2, 1,1,
        -4.8f, 5.0f, -5.0f, 0.8,0.9,1.0,0.2, 0,1,

        // Leva strana (X = -5.0), Z se smanji za 0.2 sa obe strane
        -5.0f, 0.0f, -4.8f, 0.8,0.9,1.0,0.2, 0,0,
        -5.0f, 0.0f, 4.8f,  0.8,0.9,1.0,0.2, 1,0,
        -5.0f, 5.0f, 4.8f,  0.8,0.9,1.0,0.2, 1,1,
        -5.0f, 5.0f, -4.8f, 0.8,0.9,1.0,0.2, 0,1,

        // Desna strana (X = 5.0), Z se smanji za 0.2 sa obe strane
         5.0f, 0.0f, -4.8f, 0.8,0.9,1.0,0.2,0,0,
         5.0f, 0.0f, 4.8f,  0.8,0.9,1.0,0.2,1,0,
         5.0f, 5.0f, 4.8f,  0.8,0.9,1.0,0.2,1,1,
         5.0f, 5.0f, -4.8f,0.8,0.9,1.0,0.2,0,1,

         // Zadnji panel (Z = 5.0), X se smanji za 0.2 sa obe strane
         -4.8f,0.0f, 5.0f, 0.8,0.9,1.0,0.2,0,0,
          4.8f,0.0f, 5.0f, 0.8,0.9,1.0,0.2,1,0,
          4.8f,5.0f, 5.0f, 0.8,0.9,1.0,0.2,1,1,
         -4.8f,5.0f, 5.0f, 0.8,0.9,1.0,0.2,0,1
    };
    unsigned int glassIndicesArr[] = {
        0,1,2,2,3,0,
        4,5,6,6,7,4,
        8,9,10,10,11,8,
        12,13,14,14,15,12
    };

    glGenVertexArrays(1, &glassVAO);
    glGenBuffers(1, &glassVBO);
    glGenBuffers(1, &glassEBO);

    glBindVertexArray(glassVAO);
    glBindBuffer(GL_ARRAY_BUFFER, glassVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glassVertices), glassVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glassEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(glassIndicesArr), glassIndicesArr, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(7 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);

    Box corners[4] = {
        {{-5.0f, 0.0f, -5.0f}, {-4.8f, 5.0f, -4.8f}}, // prednji levi ugao
        {{4.8f, 0.0f, -5.0f}, {5.0f, 5.0f, -4.8f}},   // prednji desni ugao
        {{-5.0f, 0.0f, 4.8f}, {-4.8f, 5.0f, 5.0f}},   // zadnji levi ugao
        {{4.8f, 0.0f, 4.8f}, {5.0f, 5.0f, 5.0f}}      // zadnji desni ugao
    };

    Box frame[4] = {
        {{-5.0f, 0.0f, -5.0f}, {5.0f, 0.2f, -4.8f}},  // prednja
        {{-5.0f, 0.0f, 4.8f}, {5.0f, 0.2f, 5.0f}},    // zadnja
        {{-5.0f, 0.0f, -5.0f}, {-4.8f, 0.2f, 5.0f}},  // leva
        {{4.8f, 0.0f, -5.0f}, {5.0f, 0.2f, 5.0f}}     // desna
    };

    unsigned int cornerVAOs[4], cornerVBOs[4], cornerEBOs[4];
    for (int i = 0; i < 4; i++) {
        generateBoxMesh(corners[i], cornerVAOs[i], cornerVBOs[i], cornerEBOs[i]);
    }

    unsigned int frameVAOs[4], frameVBOs[4], frameEBOs[4];
    for (int i = 0; i < 4; i++) {
        generateBoxMesh(frame[i], frameVAOs[i], frameVBOs[i], frameEBOs[i]);
    }

    // Kamera
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    float aspect = (float)width / (float)height;

    glm::vec3 camPos = glm::vec3(0.0f, 7.0f, 10.0f); // Kamera unazad i malo gore
    glm::mat4 view = glm::lookAt(camPos, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    glm::mat4 projection = glm::perspective(glm::radians(60.0f), aspect, 0.1f, 100.0f);

    glUseProgram(shaderProgram);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "uV"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "uP"), 1, GL_FALSE, glm::value_ptr(projection));



    // Glavna petlja
    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.3f, 0.5f, 0.7f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);

        // 1) Dno
        glUniform1i(glGetUniformLocation(shaderProgram, "useTex"), 0);
        glm::mat4 model = glm::mat4(1.0f);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "uM"), 1, GL_FALSE, glm::value_ptr(model));
        glBindVertexArray(floorVAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // 2) Pesak
        glUniform1i(glGetUniformLocation(shaderProgram, "useTex"), 1);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, sandTexture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "uM"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
        glBindVertexArray(sandVAO);
        glDrawElements(GL_TRIANGLES, sandIndices.size(), GL_UNSIGNED_INT, 0);

        // 3) Crni stubovi
        glUniform1i(glGetUniformLocation(shaderProgram, "useTex"), 0); // bez teksture
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "uM"), 1, GL_FALSE, glm::value_ptr(model));
        for (int i = 0; i < 4; i++) {
            glBindVertexArray(cornerVAOs[i]);
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        }

        // 4) Okvir oko peska
        for (int i = 0; i < 4; i++) {
            glBindVertexArray(frameVAOs[i]);
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        }

        // 6) Stakla (ako zelis providna)
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glUniform1i(glGetUniformLocation(shaderProgram, "useTex"), 0); // bez teksture
        // model matrica ista
        glBindVertexArray(glassVAO);
        glDrawElements(GL_TRIANGLES, sizeof(glassIndicesArr) / sizeof(unsigned int), GL_UNSIGNED_INT, 0);




        const double targetFPS = 75.0;
        const double frameDuration = 1.0 / targetFPS; // 0.01333s

        static double lastTime = glfwGetTime();
        double currentTime = glfwGetTime();
        double delta = currentTime - lastTime;

        if (delta < frameDuration) {
            std::this_thread::sleep_for(std::chrono::duration<double>(frameDuration - delta));
        }

        lastTime = glfwGetTime();

       glfwSwapBuffers(window);
       glfwPollEvents();

     
    }


    // Cleanup
    glDeleteVertexArrays(1, &floorVAO);
    glDeleteBuffers(1, &floorVBO);
    glDeleteBuffers(1, &floorEBO);
    glDeleteVertexArrays(1, &sandVAO);
    glDeleteBuffers(1, &sandVBO);
    glDeleteBuffers(1, &sandEBO);
    glDeleteVertexArrays(1, &glassVAO);
    glDeleteBuffers(1, &glassVBO);
    glDeleteBuffers(1, &glassEBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}