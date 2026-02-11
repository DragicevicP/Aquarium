#pragma once
#include <vector>
#include <glm/glm.hpp>

struct FoodParticle {
    glm::vec3 pos;
    float delay;
    bool active;
    bool stopped;
};

class FoodSystem {
public:
    FoodSystem(glm::vec3 minBounds, glm::vec3 maxBounds, float spawnY, float fallSpeed, float sandTopY);

    void SpawnBurst(int count);                
    void Update(float dt);                      
    int TryEat(const glm::vec3& fishPos, float eatRadiusXZ, float eatRadiusY);

    const std::vector<FoodParticle>& Particles() const { return particles; }

private:
    std::vector<FoodParticle> particles;
    glm::vec3 minB, maxB;
    float spawnY;
    float fallSpeed;
    float sandTopY;

    float rand01();
    float randRange(float a, float b);
};