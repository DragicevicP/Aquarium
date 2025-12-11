#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "../Header/Util.h"

#include <chrono>
#include <thread>
#include <vector>
#include <iostream>



const double TARGET_FRAME_TIME = 1.0 / 75.0;
static bool shouldClose = false;
static bool isChestOpen = false;
float deltaTime = 5.0f;

struct Rect {
    float x, y, w, h;
};

struct Bubble {
    float x, y;
    float size;
    float speed;
    bool alive;
};

struct Fish {
    float x, y;
    float w, h;
    float speed;
    bool facingRight;
    GLuint texture;
    float nextBubbleTime;
};

struct Food {
    float x, y;
    float size;
    bool alive;
    float speed; 
};

Fish goldfish = { 500.0f, 600.0f, 180.0f, 100.0f, 15000.0f, true, 0 , 0.0f};
Fish clownfish = { 1000.0f, 500.0f, 170.0f, 90.0f, 25000.0f, true, 0 , 0.0f};


std::vector<Bubble> bubbles;
int goldfishBubbleCount = 0;
int clownfishBubbleCount = 0;

std::vector<Food> foods;
bool spawnFood = false;

float calculateBubbleX(Fish fish) {
    float bubbleX = 0;
    if (fish.facingRight == true) {
        bubbleX = fish.x + fish.w;
    }
    else {
        bubbleX = fish.x;
    }
    return bubbleX;
}

Bubble createBubble(const Fish& fish)
{
    Bubble b;
    b.x = calculateBubbleX(fish);
    b.y = fish.y + fish.h / 2;
    b.size = 30;
    b.speed = 7000.0f;
    b.alive = true;

    return b;
     
}

Food createFood(float x)
{
    Food f;
    f.x = x;
    f.y = 0.0f;          
    f.size = 40.0f;      
    f.alive = true;
    f.speed = 2000.0f;    
    return f;
}



void updateBubbleSpawner(Fish& fish, int& bubbleCount, std::vector<Bubble>& bubbles)
{
    float now = glfwGetTime();

    if (bubbleCount > 0 && now >= fish.nextBubbleTime)
        {
            bubbles.push_back(createBubble(fish));
            bubbleCount--;
            fish.nextBubbleTime = now + 0.3f;
        }
    
}


void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{

    int fbW, fbH;
    glfwGetFramebufferSize(window, &fbW, &fbH);

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) 
        shouldClose = true;
    
    if (key == GLFW_KEY_C && action == GLFW_PRESS) 
        isChestOpen = !isChestOpen;

    if (key == GLFW_KEY_Z && action == GLFW_PRESS)
    {
        goldfishBubbleCount = 3;    
        if (goldfish.nextBubbleTime < glfwGetTime()) {
            goldfish.nextBubbleTime = glfwGetTime();
        }
       
    }

    if (key == GLFW_KEY_K && action == GLFW_PRESS)
    {
        clownfishBubbleCount = 3;
        if (clownfish.nextBubbleTime < glfwGetTime()) {
            clownfish.nextBubbleTime = glfwGetTime();
        }
    }

    if (key == GLFW_KEY_ENTER && action == GLFW_PRESS)
    {
        for (int i = 0; i < 5; i++)
        {
            int minX = static_cast<int>(0.1f * fbW); 
            int maxX = static_cast<int>(0.7f * fbW); 
            float x = static_cast<float>(rand() % (maxX - minX) + minX);
            foods.push_back(createFood(x));
        }
    }

       
    
}

GLuint prepereTexture(const char* filepath) {

    GLuint texture = loadImageToTexture(filepath);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    return texture;
}




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
    GLuint chestOpenTexture = prepereTexture("Resources/chest_open.png");
    GLuint chestClosedTexture = prepereTexture("Resources/chest_closed.png");
    GLuint jewelTexture = prepereTexture("Resources/jewel.png");
    GLuint goldTexture = prepereTexture("Resources/gold.png");
    GLuint goldFishTexture = prepereTexture("Resources/gold_fish.png");
    GLuint clownFishTexture = prepereTexture("Resources/clown_fish.png");
    GLuint bubbleTexture = prepereTexture("Resources/bubble.png");
    GLuint foodTexture = prepereTexture("Resources/food.png");

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

        //kovceg
        float chestWidth  = 350.0f;
        float chestHeight = 350.0f;
        float chestX = fbW - 400.0f;                
        float chestY = fbH - 400.0f;

        GLuint chestTex = isChestOpen ? chestOpenTexture : chestClosedTexture;
        drawTexturePixels(textureShader, chestTex, chestX, chestY, chestWidth, chestHeight, fbW, fbH);

        //blago
        float goldWidth = 300.0f;
        float goldHeight = 250.0f;
        float goldX = fbW - 370.0f;
        float goldY = chestY;

        float jewelWidth = 80.0f;
        float jewelHeight = 100.0f;
        float jewelX = goldX + 70.0f;
        float jewelY = goldY + 100.0f;

        if (isChestOpen) {
            drawTexturePixels(textureShader, goldTexture, goldX, goldY, goldWidth, goldHeight, fbW, fbH);
            drawTexturePixels(textureShader, jewelTexture, jewelX, jewelY, jewelWidth, jewelHeight, fbW, fbH);
        }
  
        goldfish.texture = goldFishTexture;
        clownfish.texture = clownFishTexture;

        // --- Pomeranje riba ---
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) goldfish.y -= goldfish.speed * deltaTime;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) goldfish.y += goldfish.speed * deltaTime;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) { goldfish.x -= goldfish.speed * deltaTime; goldfish.facingRight = false; }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) { goldfish.x += goldfish.speed * deltaTime; goldfish.facingRight = true; }

        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) clownfish.y -= clownfish.speed * deltaTime;
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) clownfish.y += clownfish.speed * deltaTime;
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) { clownfish.x -= clownfish.speed * deltaTime; clownfish.facingRight = false; }
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) { clownfish.x += clownfish.speed * deltaTime; clownfish.facingRight = true; }

        auto clampFish = [&](Fish& fish) { 
            if (fish.x < 0) fish.x = 10.0f;
            if (fish.x + fish.w > fbW) fish.x = fbW  - fish.w;
            if (fish.y > fbH) fish.y = fbH - fish.h;
            if (fish.y < fbH * 0.40)
                fish.y = fbH * 0.40;
        };

        clampFish(goldfish);
        clampFish(clownfish);

        drawTextureFlipped(textureShader, goldfish.texture, goldfish.x, goldfish.y, goldfish.w, goldfish.h, goldfish.facingRight, fbW, fbH);
        drawTextureFlipped(textureShader, clownfish.texture, clownfish.x, clownfish.y, clownfish.w, clownfish.h, !clownfish.facingRight, fbW, fbH);


        //Mehurici
        updateBubbleSpawner(goldfish, goldfishBubbleCount, bubbles);
        updateBubbleSpawner(clownfish, clownfishBubbleCount, bubbles);

        for (auto& b : bubbles) {

            if (!b.alive) continue;

            b.y -= b.speed * deltaTime; 

            if (b.y < fbH * 0.40)
                b.alive = false;
        }

        for (auto& b : bubbles) {
            if (b.alive)
                drawTexturePixels(textureShader, bubbleTexture, b.x, b.y, b.size, b.size, fbW, fbH);
        }


        //hrana
        float topSandY = fbH - 100.0f;

        for (auto& f : foods)
        {
            if (!f.alive) continue;

            f.y += f.speed * deltaTime;

            if (f.y + f.size >= topSandY)
                f.y = topSandY - f.size;
     
            if (f.x + f.size > goldfish.x && f.x < goldfish.x + goldfish.w &&
                f.y + f.size > goldfish.y && f.y < goldfish.y + goldfish.h)
            {
                f.alive = false;
                goldfish.h += 0.01f;
                //goldfish.h += 0.5f;
            }
            if (f.x + f.size > clownfish.x && f.x < clownfish.x + clownfish.w &&
                f.y + f.size > clownfish.y && f.y < clownfish.y + clownfish.h)
            {
                f.alive = false;
                clownfish.h += 0.01f;
            }
        }


        for (auto& f : foods)
        {
            if (f.alive)
                drawTexturePixels(textureShader, foodTexture, f.x, f.y, f.size, f.size, fbW, fbH);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();

        // Frame limiter
        double frameEnd = glfwGetTime();
        deltaTime = (float)(frameEnd - frameStart);
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