#include "Bubble.h"
#include <cstdlib>
#include <algorithm>

BubbleSystem::BubbleSystem(float speed, float topY)
    : bubbleSpeed(speed), waterTopY(topY) {
}

void BubbleSystem::Spawn3(const glm::vec3& fishPos) {
    for (int i = 0; i < 3; i++) {
        Bubble b;
        b.active = true;
        b.pos = fishPos + glm::vec3(0.0f, 0.2f, 0.0f);
        b.delay = i * 0.30f;

        float rx = ((rand() % 100) / 100.0f - 0.5f) * 0.15f;
        float rz = ((rand() % 100) / 100.0f - 0.5f) * 0.15f;

        b.pos.x += rx;
        b.pos.z += rz;

        bubbles.push_back(b);
    }
}

void BubbleSystem::Update(float dt) {
    for (auto& b : bubbles) {
        if (!b.active) continue;

        if (b.delay > 0.0f) {
            b.delay -= dt;
            continue;
        }

        b.pos.y += bubbleSpeed * dt;

        if (b.pos.y > waterTopY) {
            b.active = false;
        }
    }
}

void BubbleSystem::RemoveInactive() {
    bubbles.erase(
        std::remove_if(bubbles.begin(), bubbles.end(),
            [](const Bubble& b) { return !b.active; }),
        bubbles.end());
}

const std::vector<Bubble>& BubbleSystem::GetBubbles() const {
    return bubbles;
}