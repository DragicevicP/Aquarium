#include "Food.h"
#include <random>
#include <cmath>

static std::mt19937& rng() {
    static std::mt19937 gen((unsigned)std::random_device{}());
    return gen;
}

FoodSystem::FoodSystem(glm::vec3 minBounds, glm::vec3 maxBounds, float spawnY_, float fallSpeed_, float sandTopY_)
    : minB(minBounds), maxB(maxBounds), spawnY(spawnY_), fallSpeed(fallSpeed_), sandTopY(sandTopY_) {
}

float FoodSystem::rand01() {
    static std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    return dist(rng());
}

float FoodSystem::randRange(float a, float b) {
    return a + (b - a) * rand01();
}

void FoodSystem::SpawnBurst(int count) {
    for (int i = 0; i < count; i++) {
        FoodParticle p;
        p.active = true;
        p.stopped = false;

        float x = randRange(minB.x, maxB.x);
        float z = randRange(minB.z, maxB.z);

        p.pos = glm::vec3(x, spawnY, z);
        p.delay = i * 0.12f + randRange(0.0f, 0.08f);

        particles.push_back(p);
    }
}

void FoodSystem::Update(float dt) {
    for (auto& p : particles) {
        if (!p.active) continue;

        if (p.delay > 0.0f) {
            p.delay -= dt;
            continue;
        }

        if (!p.stopped) {
            p.pos.y -= fallSpeed * dt;
            if (p.pos.y <= sandTopY) {
                p.pos.y = sandTopY;
                p.stopped = true; 
            }
        }
    }
}

int FoodSystem::TryEat(const glm::vec3& fishPos, float eatRadiusXZ, float eatRadiusY) {
    int eaten = 0;

    for (auto& p : particles) {
        if (!p.active) continue;

        float dx = fishPos.x - p.pos.x;
        float dz = fishPos.z - p.pos.z;
        float distXZ = std::sqrt(dx * dx + dz * dz);

        float dy = std::fabs(fishPos.y - p.pos.y);

        if (distXZ <= eatRadiusXZ && dy <= eatRadiusY) {
            p.active = false;
            eaten++;
        }
    }

    return eaten;
}