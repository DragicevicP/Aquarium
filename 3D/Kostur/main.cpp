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
#include "Model.h"
#include "Food.h"
#include "FishController.h"
#include "Bubble.h"

bool chestOpen = false;
float chestLidAngle = 0.0f;

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

GLuint prepareTexture(const char* filepath) {
    GLuint tex = loadImageToTexture(filepath);

    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    return tex;
}

void createMesh(unsigned int& VAO, unsigned int& VBO, unsigned int& EBO, const float* vertices, size_t verticesSize, const unsigned int* indices, size_t indicesSize) {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, verticesSize, vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSize, indices, GL_STATIC_DRAW);

    const GLsizei stride = 9 * sizeof(float);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(7 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}


void generateSandMesh(unsigned int& VAO, unsigned int& VBO, unsigned int& EBO,std::vector<unsigned int>& indices) {

    const int GRID_SIZE = 50;
    const float SIZE_X = 9.6f;  
    const float SIZE_Z = 6.0f;
    const float OFFSET_X = -SIZE_X / 2.0f;
    const float OFFSET_Z = -SIZE_Z / 2.0f;

    std::vector<float> vertices;
    indices.clear();
    srand((unsigned int)time(0));

    for (int z = 0; z <= GRID_SIZE; ++z) {
        for (int x = 0; x <= GRID_SIZE; ++x) {

            float xf = OFFSET_X + ((float)x / GRID_SIZE) * SIZE_X;
            float zf = OFFSET_Z + ((float)z / GRID_SIZE) * SIZE_Z;
            float yf = ((rand() % 100) / 1000.0f);

            vertices.push_back(xf);
            vertices.push_back(yf);
            vertices.push_back(zf);

            vertices.push_back(1.0f);
            vertices.push_back(0.9f);
            vertices.push_back(0.7f);
            vertices.push_back(1.0f);

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

    createMesh(VAO, VBO, EBO, vertices.data(), vertices.size() * sizeof(float), indices.data(), indices.size() * sizeof(unsigned int));
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
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    bool depthEnabled = true;
    bool cullEnabled = false;

    GLuint shaderProgram = createShader("basic.vert", "basic.frag");
    GLuint hudShader = createShader("hud.vert", "hud.frag");

    GLuint sandTexture = prepareTexture("res/sand.png");
    GLuint studentTexture = prepareTexture("res/student.png");

    Model seagrassModel("res/grass/Seaweed.obj");
    GLuint seagrassTexture = prepareTexture("res/grass/sw02mat.png");

    Model goldfishModel("res/goldfish/GoldFish.obj");
    GLuint goldfishTexture = prepareTexture("res/goldfish/goldfish.png");

    Model clownfishModel("res/clownfish/fishClown.obj");
    GLuint clownfishTexture = prepareTexture("res/clownfish/fishclown.png");

    Model foodModel("res/food/food.obj");               
    GLuint foodTexture = prepareTexture("res/food/food.png");

    Model bubbleModel("res/bubble/bubble.obj");             
    GLuint bubbleTexture = prepareTexture("res/bubble/bubble.png"); 
    BubbleSystem bubbles(1.2f, 5.0f);

    Model chestBase("res/chest/chest_base.obj");
    Model chestLid("res/chest/chest_lid.obj");
    GLuint chestTexture = prepareTexture("res/chest/wood.jpg");

    Model coinModel("res/coins/coins.obj");
    GLuint coinTexture = prepareTexture("res/coins/coins.jpg");

    Model gemModel("res/gem/gem.obj");
    GLuint gemTexture = prepareTexture("res/gem/gem.jpg");

    glUseProgram(shaderProgram);
    glUniform1i(glGetUniformLocation(shaderProgram, "uTex"), 0);

  
    //Dno
    float floorVertices[] = {
       -5.0f, 0.0f, -3.0f,  0.0f,0.0f,0.2f,1.0f,   0,0,
        5.0f, 0.0f, -3.0f,  0.0f,0.0f,0.2f,1.0f,   1,0,
        5.0f, 0.0f,  3.0f,  0.0f,0.0f,0.2f,1.0f,   1,1,
       -5.0f, 0.0f,  3.0f,  0.0f,0.0f,0.2f,1.0f,   0,1
    };
    unsigned int floorIndices[] = { 0,1,2,2,3,0 };
    unsigned int floorVAO, floorVBO, floorEBO;
    createMesh(floorVAO, floorVBO, floorEBO,floorVertices, sizeof(floorVertices),floorIndices, sizeof(floorIndices));

    unsigned int sandVAO, sandVBO, sandEBO;
    std::vector<unsigned int> sandIndices;
    generateSandMesh(sandVAO, sandVBO, sandEBO, sandIndices);

    //Staklo
    float glassVertices[] = {
        -4.8f, 0.0f, -2.9f, 0.8,0.9,1.0,0.2, 0,0,
         4.8f, 0.0f, -2.9f, 0.8,0.9,1.0,0.2, 1,0,
         4.8f, 5.0f, -2.9f, 0.8,0.9,1.0,0.2, 1,1,
        -4.8f, 5.0f, -2.9f, 0.8,0.9,1.0,0.2, 0,1,

        -4.9f, 0.0f, -2.8f, 0.8,0.9,1.0,0.2, 0,0,
        -4.9f, 0.0f, 2.8f,  0.8,0.9,1.0,0.2, 1,0,
        -4.9f, 5.0f, 2.8f,  0.8,0.9,1.0,0.2, 1,1,
        -4.9f, 5.0f, -2.8f, 0.8,0.9,1.0,0.2, 0,1,

         4.9f, 0.0f, -2.8f, 0.8,0.9,1.0,0.2,0,0,
         4.9f, 0.0f, 2.8f,  0.8,0.9,1.0,0.2,1,0,
         4.9f, 5.0f, 2.8f,  0.8,0.9,1.0,0.2,1,1,
         4.9f, 5.0f, -2.8f,0.8,0.9,1.0,0.2,0,1,

         -4.8f,0.0f, 2.9f, 0.8,0.9,1.0,0.2,0,0,
          4.8f,0.0f, 2.9f, 0.8,0.9,1.0,0.2,1,0,
          4.8f,5.0f, 2.9f, 0.8,0.9,1.0,0.2,1,1,
         -4.8f,5.0f, 2.9f, 0.8,0.9,1.0,0.2,0,1
    };
    unsigned int glassIndicesArr[] = {
        0,1,2,2,3,0,
        4,5,6,6,7,4,
        8,9,10,10,11,8,
        12,13,14,14,15,12
    };
    unsigned int glassVAO, glassVBO, glassEBO;
    createMesh(glassVAO, glassVBO, glassEBO, glassVertices, sizeof(glassVertices), glassIndicesArr, sizeof(glassIndicesArr));

    //Okviri
    float pillarVertices[] = {
        -0.1f, 0.0f, -0.1f,  0,0,0,1, 0,0,
         0.1f, 0.0f, -0.1f,  0,0,0,1, 1,0,
         0.1f, 5.0f, -0.1f,  0,0,0,1, 1,1,
        -0.1f, 5.0f, -0.1f,  0,0,0,1, 0,1,

        -0.1f, 0.0f, 0.1f,   0,0,0,1, 0,0,
         0.1f, 0.0f, 0.1f,   0,0,0,1, 1,0,
         0.1f, 5.0f, 0.1f,   0,0,0,1, 1,1,
        -0.1f, 5.0f, 0.1f,   0,0,0,1, 0,1
    };
    unsigned int pillarIndices[] = {
        0,1,2,2,3,0,
        4,5,6,6,7,4,
        0,1,5,5,4,0,
        2,3,7,7,6,2,
        0,3,7,7,4,0,
        1,2,6,6,5,1
    };
    glm::vec3 pillarPositions[] = {
           {-4.9f, 0.0f, -2.9f},
           { 4.9f, 0.0f, -2.9f},
           {-4.9f, 0.0f,  2.9f},
           { 4.9f, 0.0f,  2.9f}
    };
    glm::vec3 framePos[] = {
         {0.0f, 0.0f, -2.9f},
         {0.0f, 0.0f,  2.9f},
         {-4.9f, 0.0f, 0.0f},
         { 4.9f, 0.0f, 0.0f}
    };
    glm::vec3 frameScale[] = {
        {50.0f, 0.04f, 1.0f},
        {50.0f, 0.04f, 1.0f},
        {1.0f, 0.04f, 30.0f},
        {1.0f, 0.04f, 30.0f}
    };
    unsigned int pillarVAO, pillarVBO, pillarEBO;
    createMesh(pillarVAO, pillarVBO, pillarEBO, pillarVertices, sizeof(pillarVertices), pillarIndices, sizeof(pillarIndices));


    //student
    float hudVertices[] = {
        -0.95f,  0.95f, 0.0f, 1,1,1,0.8f,  0,1,
        -0.60f,  0.95f, 0.0f, 1,1,1,0.8f,  1,1,
        -0.60f,  0.80f, 0.0f, 1,1,1,0.8f,  1,0,
        -0.95f,  0.80f, 0.0f, 1,1,1,0.8f,  0,0
    };
    unsigned int hudIndices[] = { 0,1,2,2,3,0 };
    unsigned int hudVAO, hudVBO, hudEBO;
    createMesh(hudVAO, hudVBO, hudEBO, hudVertices, sizeof(hudVertices), hudIndices, sizeof(hudIndices));

    //trava
    glm::vec3 grassPos[] = {
           {-3.0f, 0.0f,  0.0f},
           {-1.0f, 0.0f,  2.0f},
           { 2.5f, 0.0f,  1.5f}
    };

    //ribe
    glm::vec3 minB(-4.3f, 0.3f, -2.4f);
    glm::vec3 maxB(4.3f, 4.6f, 2.4f);
    FishController goldFish(glm::vec3(0.0f, 3.0f, 0.0f),0.0f,90.0f, 2.5f, 2.0f,minB, maxB, true);
    FishController clownFish( glm::vec3(3.0f, 2.0f, 0.0f),0.0f, 270.0f, 2.0f, 1.5f, minB, maxB, false);
    double prevTime = glfwGetTime();

    //hrana
    FoodSystem food(minB, maxB, 6.5f, 1.8f, 0.15f);
    float goldThickness = 0.0f;
    float clownThickness = 0.0f;

    // Kamera
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    float aspect = (float)width / (float)height;

    glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 7.0f, 8.0f), glm::vec3(0, 2.0f, 0), glm::vec3(0, 1, 0));
    glm::mat4 projection = glm::perspective(glm::radians(60.0f), aspect, 0.1f, 100.0f);

    glUseProgram(shaderProgram);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "uV"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "uP"), 1, GL_FALSE, glm::value_ptr(projection));


    GLFWcursor* anchorCursor = loadImageToCursor("res/anchor.png");
    if (anchorCursor != nullptr)
    {
        glfwSetCursor(window, anchorCursor);
    }


    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.3f, 0.5f, 0.7f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        if (depthEnabled) glEnable(GL_DEPTH_TEST);
        else              glDisable(GL_DEPTH_TEST);

        if (cullEnabled)  glEnable(GL_CULL_FACE);
        else              glDisable(GL_CULL_FACE);

        double now = glfwGetTime();
        float dt = (float)(now - prevTime);
        prevTime = now;


        goldFish.Update(window, dt, GLFW_KEY_W, GLFW_KEY_S, GLFW_KEY_A, GLFW_KEY_D,GLFW_KEY_Q, GLFW_KEY_E);
        clownFish.Update(window, dt, GLFW_KEY_UP, GLFW_KEY_DOWN, GLFW_KEY_LEFT, GLFW_KEY_RIGHT,GLFW_KEY_K, GLFW_KEY_L);


        static bool enterPrev = false;
        bool enterNow = glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS;
        if (enterNow && !enterPrev) {
            food.SpawnBurst(4);
        }
        enterPrev = enterNow;
        food.Update(dt);
        int eatenGold = food.TryEat(goldFish.Position(), 0.35f, 0.40f);
        if (eatenGold > 0) goldThickness += 0.1f * eatenGold;

        int eatenClown = food.TryEat(clownFish.Position(), 0.30f, 0.35f);
        if (eatenClown > 0) clownThickness += 0.01f * eatenClown;



        static bool zPrev = false;
        bool zNow = glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS;
        if (zNow && !zPrev)
            bubbles.Spawn3(goldFish.Position());
        zPrev = zNow;

        static bool kPrev = false;
        bool kNow = glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS;
        if (kNow && !kPrev)
            bubbles.Spawn3(clownFish.Position());
        kPrev = kNow;

        bubbles.Update(dt);
        bubbles.RemoveInactive();


        static bool cPrev = false;
        bool cNow = glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS;
        if (cNow && !cPrev)
            chestOpen = !chestOpen;
        cPrev = cNow;

        float targetAngle = chestOpen ? glm::radians(100.0f) : 0.0f;
        float speed = 4.0f;
        chestLidAngle += (targetAngle - chestLidAngle) * (1.0f - expf(-speed * dt));



        static bool key1Prev = false;
        bool key1Now = glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS;
        if (key1Now && !key1Prev)
        {
            depthEnabled = !depthEnabled;

            if (depthEnabled)
                glEnable(GL_DEPTH_TEST);
            else
                glDisable(GL_DEPTH_TEST);
        }
        key1Prev = key1Now;

        static bool key2Prev = false;
        bool key2Now = glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS;
        if (key2Now && !key2Prev)
        {
            cullEnabled = !cullEnabled;

            if (cullEnabled)
                glEnable(GL_CULL_FACE);
            else
                glDisable(GL_CULL_FACE);
        }
        key2Prev = key2Now;


        glUseProgram(shaderProgram);

        // Dno
        glUniform1i(glGetUniformLocation(shaderProgram, "useTex"), 0);
        glm::mat4 model = glm::mat4(1.0f);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "uM"), 1, GL_FALSE, glm::value_ptr(model));
        glBindVertexArray(floorVAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // Pesak
        glUniform1i(glGetUniformLocation(shaderProgram, "useTex"), 1);
        glUniform1i(glGetUniformLocation(shaderProgram, "transparent"), 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, sandTexture);
        glUniform1i(glGetUniformLocation(shaderProgram, "uTex"), 0);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "uM"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
        glBindVertexArray(sandVAO);
        glDrawElements(GL_TRIANGLES, sandIndices.size(), GL_UNSIGNED_INT, 0);

        // Crni stubovi i okvir oko peska
        GLboolean wasCull = glIsEnabled(GL_CULL_FACE);
        glDisable(GL_CULL_FACE);

        glUniform1i(glGetUniformLocation(shaderProgram, "useTex"), 0); 
        glBindVertexArray(pillarVAO);

        for (int i = 0; i < 4; i++) {
            glm::mat4 m = glm::translate(glm::mat4(1.0f), pillarPositions[i]);
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "uM"), 1, GL_FALSE, glm::value_ptr(m));
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        }
        for (int i = 0; i < 4; i++) {
            glm::mat4 m = glm::translate(glm::mat4(1.0f), framePos[i]);
            m = glm::scale(m, frameScale[i]);
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "uM"), 1, GL_FALSE, glm::value_ptr(m));
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        }
       

        // Trava
        glUniform1i(glGetUniformLocation(shaderProgram, "useTex"), 1);
        glUniform1i(glGetUniformLocation(shaderProgram, "transparent"), 1);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, seagrassTexture);
        glUniform1i(glGetUniformLocation(shaderProgram, "uTex"), 0);
       
        for (int i = 0; i < 4; i++) {
            glm::mat4 m = glm::mat4(1.0f);
            m = glm::translate(m, grassPos[i]);
            m = glm::scale(m, glm::vec3(0.015f));
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "uM"), 1, GL_FALSE, glm::value_ptr(m));
            seagrassModel.Draw();
        }
        glUniform1i(glGetUniformLocation(shaderProgram, "transparent"), 0);
        if (wasCull) glEnable(GL_CULL_FACE);
        else         glDisable(GL_CULL_FACE);

        // Ribice
        glUniform1i(glGetUniformLocation(shaderProgram, "useTex"), 1);
        glUniform1i(glGetUniformLocation(shaderProgram, "transparent"), 0);
        glActiveTexture(GL_TEXTURE0);
        glUniform1i(glGetUniformLocation(shaderProgram, "uTex"), 0);

        glBindTexture(GL_TEXTURE_2D, goldfishTexture);
        glm::mat4 mg = goldFish.ModelMatrix(1.0f);
        mg = glm::scale(mg, glm::vec3(0.35f, 0.35f * (1.0f + goldThickness), 0.35f));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "uM"), 1, GL_FALSE, glm::value_ptr(mg));
        goldfishModel.Draw();

        glBindTexture(GL_TEXTURE_2D, clownfishTexture);
        glm::mat4 mc = clownFish.ModelMatrix(1.0f);
        mc = glm::scale(mc, glm::vec3(0.1f, 0.1f * (1.0f + clownThickness), 0.1f));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "uM"), 1, GL_FALSE, glm::value_ptr(mc));
        clownfishModel.Draw();

        // Baloni
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glUniform1i(glGetUniformLocation(shaderProgram, "useTex"), 1);
        glUniform1i(glGetUniformLocation(shaderProgram, "transparent"), 1);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, bubbleTexture);
        glUniform1i(glGetUniformLocation(shaderProgram, "uTex"), 0);

        for (const auto& b : bubbles.GetBubbles()) {
            if (b.delay > 0.0f) continue;
            glm::mat4 m(1.0f);
            m = glm::translate(m, b.pos);
            m = glm::scale(m, glm::vec3(0.002f));
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "uM"), 1, GL_FALSE, glm::value_ptr(m));
            bubbleModel.Draw();
        }
        glUniform1i(glGetUniformLocation(shaderProgram, "transparent"), 0);

        // Stakla
        glDepthMask(GL_FALSE);
        glDisable(GL_CULL_FACE);
        glUniform1i(glGetUniformLocation(shaderProgram, "useTex"), 0);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "uM"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
        glBindVertexArray(glassVAO);
        glDrawElements(GL_TRIANGLES, sizeof(glassIndicesArr) / sizeof(unsigned int), GL_UNSIGNED_INT, 0);
        glDepthMask(GL_TRUE);
        if (cullEnabled) glEnable(GL_CULL_FACE);
        else             glDisable(GL_CULL_FACE);

        // Hrana 
        glUniform1i(glGetUniformLocation(shaderProgram, "useTex"), 1);
        glUniform1i(glGetUniformLocation(shaderProgram, "transparent"), 1);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, foodTexture);
        glUniform1i(glGetUniformLocation(shaderProgram, "uTex"), 0);

        for (const auto& p : food.Particles()) {
            if (!p.active) continue;
            glm::mat4 fm(1.0f);
            fm = glm::translate(fm, p.pos);
            fm = glm::scale(fm, glm::vec3(0.001f));
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "uM"), 1, GL_FALSE, glm::value_ptr(fm));

            foodModel.Draw();
        }
        glUniform1i(glGetUniformLocation(shaderProgram, "transparent"), 0);

        //Kovceg
        glm::vec3 chestPos(2.0f, 0.0f, -2.0f);
        float chestScale = 0.8f;             
        float chestYawDeg = 210.0f;
        glUniform1i(glGetUniformLocation(shaderProgram, "useTex"), 1);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, chestTexture);
        glUniform1i(glGetUniformLocation(shaderProgram, "uTex"), 0);
        glUniform1i(glGetUniformLocation(shaderProgram, "transparent"), 0);
      

        glm::mat4 Mbase(1.0f);
        Mbase = glm::translate(Mbase, chestPos);
        Mbase = glm::rotate(Mbase, glm::radians(-180.0f), glm::vec3(1, 0, 0));
        Mbase = glm::scale(Mbase, glm::vec3(chestScale));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "uM"), 1, GL_FALSE, glm::value_ptr(Mbase));
        chestBase.Draw();
        glm::mat4 Mlid = Mbase;
        glm::vec3 hingeOffset = glm::vec3(0.0f, -1.0f, 0.20f); 
        Mlid = glm::translate(Mlid, hingeOffset);
        Mlid = glm::rotate(Mlid, -chestLidAngle, glm::vec3(1, 0, 0));
        Mlid = glm::translate(Mlid, -hingeOffset);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "uM"), 1, GL_FALSE, glm::value_ptr(Mlid));
        chestLid.Draw();

        glm::vec3 treasureLightPos = glm::vec3(Mbase * glm::vec4(0.0f, -0.3f, 0.1f, 1.0f));
        glUniform1i(glGetUniformLocation(shaderProgram, "uTreasureLightOn"), (chestOpen && chestLidAngle > glm::radians(10.0f)));
        glUniform3fv(glGetUniformLocation(shaderProgram, "uTreasureLightPos"), 1, glm::value_ptr(treasureLightPos));

        // blago
        if (chestOpen && chestLidAngle > glm::radians(10.0f))
        {
            glUniform1i(glGetUniformLocation(shaderProgram, "useTex"), 1);
            glUniform1i(glGetUniformLocation(shaderProgram, "transparent"), 0);
            glActiveTexture(GL_TEXTURE0);
            glUniform1i(glGetUniformLocation(shaderProgram, "uTex"), 0);

            glm::vec3 insideLocal(-0.2f, -1.0f, 0.0f);
            glBindTexture(GL_TEXTURE_2D, coinTexture);
            glm::mat4 Mc = Mbase;
            Mc = glm::translate(Mc, insideLocal);
            Mc = glm::rotate(Mc, glm::radians(90.0f), glm::vec3(1, 0, 0));
            Mc = glm::rotate(Mc, glm::radians(90.0f), glm::vec3(0, 0, 1));
            Mc = glm::scale(Mc, glm::vec3(7.0f));   
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "uM"), 1, GL_FALSE, glm::value_ptr(Mc));
            coinModel.Draw();

            glBindTexture(GL_TEXTURE_2D, gemTexture);
            glm::mat4 Mg = Mbase;
            Mg = glm::translate(Mg, insideLocal + glm::vec3(0.0f, -0.2f, 0.0f));
            Mg = glm::scale(Mg, glm::vec3(0.35f));  
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "uM"), 1, GL_FALSE, glm::value_ptr(Mg));
            gemModel.Draw();
        }


        // Student
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);

      
        glUseProgram(hudShader);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, studentTexture);
        glUniform1i(glGetUniformLocation(hudShader, "uTex"), 0);
        glUniform1i(glGetUniformLocation(hudShader, "transparent"), 1);
        glBindVertexArray(hudVAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        if (depthEnabled) glEnable(GL_DEPTH_TEST);
        else              glDisable(GL_DEPTH_TEST);
        if (cullEnabled)  glEnable(GL_CULL_FACE);
        else              glDisable(GL_CULL_FACE);

      
        const double targetFPS = 75.0;
        const double frameDuration = 1.0 / targetFPS;
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
