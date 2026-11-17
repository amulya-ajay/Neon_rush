#include "systems/ParticleSystem.h"

#include <algorithm>
#include <chrono>
#include <cmath>

ParticleSystem::ParticleSystem()
    : randomEngine(static_cast<unsigned int>(std::chrono::high_resolution_clock::now().time_since_epoch().count()))
{
    particles.reserve(220);
}

void ParticleSystem::Burst(float x, float y, Color color, int count, float force)
{
    std::uniform_real_distribution<float> angle(0.0f, 6.2831853f);
    std::uniform_real_distribution<float> speed(force * 0.25f, force);
    std::uniform_real_distribution<float> life(0.28f, 0.82f);
    std::uniform_real_distribution<float> size(2.0f, 5.0f);
    for (int i = 0; i < count && particles.size() < 220; ++i)
    {
        const float a = angle(randomEngine);
        const float v = speed(randomEngine);
        const float particleLife = life(randomEngine);
        particles.push_back(Particle{
            Vector2{ x, y },
            Vector2{ std::cos(a) * v, std::sin(a) * v },
            particleLife,
            particleLife,
            size(randomEngine),
            color
        });
    }
}

void ParticleSystem::Trail(float x, float y, Color color)
{
    if (particles.size() >= 220)
    {
        return;
    }
    std::uniform_real_distribution<float> jitter(-12.0f, 12.0f);
    particles.push_back(Particle{
        Vector2{ x + jitter(randomEngine), y },
        Vector2{ jitter(randomEngine) * 0.25f, 65.0f + jitter(randomEngine) },
        0.3f,
        0.3f,
        4.0f,
        color
    });
}

void ParticleSystem::Update(float deltaTime)
{
    const float dt = std::clamp(deltaTime, 0.0f, 0.05f);
    for (Particle& particle : particles)
    {
        particle.position.x += particle.velocity.x * dt;
        particle.position.y += particle.velocity.y * dt;
        particle.velocity.y += 85.0f * dt;
        particle.velocity.x *= 0.985f;
        particle.life -= dt;
    }
    particles.erase(std::remove_if(particles.begin(), particles.end(), [](const Particle& particle)
    {
        return particle.life <= 0.0f;
    }), particles.end());
}

void ParticleSystem::Draw() const
{
    for (const Particle& particle : particles)
    {
        const float alpha = std::clamp(particle.life / particle.maxLife, 0.0f, 1.0f);
        DrawCircleV(particle.position, particle.size * 2.2f, Fade(particle.color, alpha * 0.12f));
        DrawCircleV(particle.position, particle.size * alpha, Fade(particle.color, alpha));
    }
}

void ParticleSystem::Clear()
{
    particles.clear();
}
