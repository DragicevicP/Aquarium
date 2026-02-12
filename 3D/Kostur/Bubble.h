#pragma once
#include <vector>
#include <glm/glm.hpp>

struct Bubble {
    glm::vec3 pos;
    float delay;
    bool active;
};

class BubbleSystem {
public:
    BubbleSystem(float speed, float waterTopY);

    void Spawn3(const glm::vec3& fishPos);
    void Update(float dt);
    void RemoveInactive();

    const std::vector<Bubble>& GetBubbles() const;

private:
    std::vector<Bubble> bubbles;
    float bubbleSpeed;
    float waterTopY;
};