#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "../Header/Util.h"

#include <chrono>
#include <thread>
#include <vector>
#include <iostream>


const double TARGET_FRAME_TIME = 1.0 / 75.0;

static bool shouldClose = false;

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        shouldClose = true;
    }
}

GLuint prepereTexture(const char* filepath) {

    GLuint texture = loadImageToTexture(filepath);
    std::cout << "texSand = " << texture << std::endl;
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    return texture;
}


struct Rect {
    float x, y, w, h; 
};

void drawRectPixels(GLuint program, const Rect& r, int screenW, int screenH)
{
    float vertices[6 * 2] = {
        r.x,       r.y,        
        r.x + r.w, r.y,        
        r.x + r.w, r.y + r.h,  

        r.x,       r.y,       
        r.x + r.w, r.y + r.h, 
        r.x,       r.y + r.h  
    };

    GLuint vao, vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

    GLint posLoc = glGetAttribLocation(program, "aPos");
    glEnableVertexAttribArray(posLoc);
    glVertexAttribPointer(posLoc, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

    GLint resLoc = glGetUniformLocation(program, "u_resolution");
    glUseProgram(program);
    glUniform2f(resLoc, (float)screenW, (float)screenH);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(posLoc);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
}


int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    if (!mode) return endProgram("Could not get monitor mode.");

    GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, "Aquarium", monitor, NULL);
    if (window == NULL) return endProgram("Error in creating window");
    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) return endProgram("Error in initialization GLEW");

    GLFWcursor* anchorCursor = loadImageToCursor("Resources/anchor.png");
    if (anchorCursor != nullptr)
    {
        glfwSetCursor(window, anchorCursor);
    }

    glfwSetKeyCallback(window, keyCallback);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
 
    GLuint textureShader = createShader("Shaders/Texture.vert", "Shaders/Texture.frag");
    GLuint shaderProgram = createShader("Shaders/Aquarium.vert", "Shaders/Aquarium.frag");
 
    GLuint studentTexture = prepereTexture("Resources/student.png"); 
    GLuint textureSand = prepereTexture("Resources/sand.png");
    GLuint seagrassTexture = prepereTexture("Resources/seagrass.png");
    GLuint seagrass2Texture = prepereTexture("Resources/seagrass2.png");
   

    GLint colorLoc = glGetUniformLocation(shaderProgram, "u_color");
    double lastTime = glfwGetTime();

    while (!glfwWindowShouldClose(window) && !shouldClose)
    {
        double frameStart = glfwGetTime();

        int fbW, fbH;
        glfwGetFramebufferSize(window, &fbW, &fbH);

        glViewport(0, 0, fbW, fbH);
        glClear(GL_COLOR_BUFFER_BIT);

        float aquariumHeight = fbH * 0.60f; // 60% visine
        float aquariumX = 0.0f;
        float aquariumY = (float)fbH - aquariumHeight; 
        float aquariumW = (float)fbW;
        float aquariumH = aquariumHeight;

        //Crtanje akvarijuma i preko njega providna bela 
        glUseProgram(shaderProgram);
        glUniform4f(colorLoc, 0.2f, 0.5f, 1.0f, 1.0f); // plava
        Rect blueAquarium = { aquariumX, aquariumY, aquariumW, aquariumH };
        drawRectPixels(shaderProgram, blueAquarium, fbW, fbH);

        glUniform4f(colorLoc, 1.0f, 1.0f, 1.0f, 0.2f); // providna bela
        drawRectPixels(shaderProgram, blueAquarium, fbW, fbH);

        //Crtanje crnih ivica
        float border = 10.0f;
        glUniform4f(colorLoc, 0.0f, 0.0f, 0.0f, 1.0f);

        //Donja ivica
        Rect bottom = { 0.0f, (float)fbH - border, (float)fbW, border };
        drawRectPixels(shaderProgram, bottom, fbW, fbH);

        //Leva ivica
        Rect left = { 0.0f, aquariumY, border, aquariumH };
        drawRectPixels(shaderProgram, left, fbW, fbH);

        //Desna ivica
        Rect right = { aquariumW - border, aquariumY, border, aquariumH };
        drawRectPixels(shaderProgram, right, fbW, fbH);

        //Pesak
        glUseProgram(textureShader);
        drawTexturePixels(textureShader, textureSand, 10, fbH - 260, fbW-20, 250, fbW, fbH);

        //ime prezime
        drawTexturePixels(textureShader, studentTexture, (float)fbW - 200.0f, 0, 200.0f, 50.0f, fbW, fbH);
        
        //trava
        drawTexturePixels(textureShader, seagrass2Texture, 50.0f,(float)fbH - 400.0f, 160.0f, 300.0f, fbW, fbH);
        drawTexturePixels(textureShader, seagrass2Texture, 700.0f, (float)fbH - 300.0f, 160.0f, 250.0f, fbW, fbH);

        drawTexturePixels(textureShader, seagrassTexture, 200.0f, (float)fbH - 320.0f, 160.0f, 300.0f, fbW, fbH);
        drawTexturePixels(textureShader, seagrassTexture, 400.0f, (float)fbH - 580.0f, 160.0f, 500.0f, fbW, fbH);
        drawTexturePixels(textureShader, seagrassTexture, 900.0f, (float)fbH - 400.0f, 160.0f, 300.0f, fbW, fbH);

        glfwSwapBuffers(window);
        glfwPollEvents();

        // Frame limiter
        double frameEnd = glfwGetTime();
        double elapsed = frameEnd - frameStart;
        double sleepTime = TARGET_FRAME_TIME - elapsed;
        if (sleepTime > 0.0005) {
            std::this_thread::sleep_for(std::chrono::duration<double>(sleepTime));
        }
    }

    glDeleteProgram(shaderProgram);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}