#include "FishController.h"
#include <algorithm>
#include <cmath>

FishController::FishController(glm::vec3 startPos, float startYawDeg, float modelYawOffsetDeg, float sXZ, float sY, glm::vec3 minBounds, glm::vec3 maxBounds, bool invertPitch)
    : pos(startPos), yawDeg(startYawDeg), pitchDeg(0.0f), yawOffsetDeg(modelYawOffsetDeg), speedXZ(sXZ), speedY(sY), minB(minBounds), maxB(maxBounds), invertPitch(invertPitch) {
    clampToBounds();
}

void FishController::clampToBounds() {
    if (pos.x < minB.x) pos.x = minB.x;
    if (pos.x > maxB.x) pos.x = maxB.x;

    if (pos.y < minB.y) pos.y = minB.y;
    if (pos.y > maxB.y) pos.y = maxB.y;

    if (pos.z < minB.z) pos.z = minB.z;
    if (pos.z > maxB.z) pos.z = maxB.z;
}

void FishController::Update(GLFWwindow* window, float dt, int keyForward, int keyBackward, int keyLeft, int keyRight, int keyDown, int keyUp) {
    glm::vec3 move(0.0f);
    if (glfwGetKey(window, keyForward) == GLFW_PRESS)  move.z -= 1.0f;
    if (glfwGetKey(window, keyBackward) == GLFW_PRESS) move.z += 1.0f;
    if (glfwGetKey(window, keyLeft) == GLFW_PRESS)     move.x -= 1.0f;
    if (glfwGetKey(window, keyRight) == GLFW_PRESS)    move.x += 1.0f;
    if (glfwGetKey(window, keyUp) == GLFW_PRESS)       move.y -= 1.0f;
    if (glfwGetKey(window, keyDown) == GLFW_PRESS)     move.y += 1.0f;

    if (move == glm::vec3(0.0f)) return;

    glm::vec2 xz(move.x, move.z);
    if (xz.x != 0.0f || xz.y != 0.0f) {
        float len = std::sqrt(xz.x * xz.x + xz.y * xz.y);
        xz /= len;
        pos.x += xz.x * speedXZ * dt;
        pos.z += xz.y * speedXZ * dt;

        yawDeg = glm::degrees(std::atan2(xz.y, -xz.x));
    }

    if (move.y != 0.0f) {
        pos.y += (move.y > 0.0f ? 1.0f : -1.0f) * speedY * dt;
        pitchDeg = (move.y > 0.0f) ? -25.0f : 25.0f;
    }
    else {
        pitchDeg = 0.0f;
    }

    clampToBounds();
}

glm::mat4 FishController::ModelMatrix(float uniformScale) const {
    glm::mat4 m(1.0f);
    float finalPitch = invertPitch ? -pitchDeg : pitchDeg;
    m = glm::translate(m, pos);
    m = glm::rotate(m, glm::radians(yawDeg + yawOffsetDeg), glm::vec3(0, 1, 0));
    m = glm::rotate(m, glm::radians(finalPitch), glm::vec3(1, 0, 0));
    m = glm::scale(m, glm::vec3(uniformScale));
    return m;
}