#pragma once
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class FishController {
public:
    FishController(glm::vec3 startPos, float startYawDeg, float modelYawOffsetDeg, float speedXZ, float speedY, glm::vec3 minBounds, glm::vec3 maxBounds, bool invertPitch);
    void Update(GLFWwindow* window, float dt, int keyForward, int keyBackward, int keyLeft, int keyRight, int keyDown, int keyUp);
    glm::mat4 ModelMatrix(float uniformScale) const;
    const glm::vec3& Position() const { return pos; }

private:
    glm::vec3 pos;
    float yawDeg;
    float pitchDeg;
    float yawOffsetDeg;
    float speedXZ;
    float speedY;
    glm::vec3 minB;
    glm::vec3 maxB;
    bool invertPitch;

    void clampToBounds();
};