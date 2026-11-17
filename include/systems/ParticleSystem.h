#pragma once

#include "raylib.h"

#include <random>
#include <vector>

class ParticleSystem
{
private:
    struct Particle
    {
        Vector2 position;
        Vector2 velocity;
        float life;
        float maxLife;
        float size;
        Color color;
    };

    std::vector<Particle> particles;
    std::mt19937 randomEngine;

public:
    ParticleSystem();
    void Update(float deltaTime);
    void Draw() const;
    void Burst(float x, float y, Color color, int count, float force = 190.0f);
    void Trail(float x, float y, Color color);
    void Clear();
};
