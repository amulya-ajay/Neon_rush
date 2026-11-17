#include "entities/traffic/TrafficVehicle.h"

#include <algorithm>

TrafficVehicle::TrafficVehicle(
    float startY,
    float initialSpeed,
    int initialHealth,
    int startLane,
    int damage,
    int points
)
    : Vehicle(340.0f + startLane * 150.0f, startY, initialSpeed, initialHealth),
    lane(startLane),
    speedMultiplier(1.0f),
    collisionDamage(std::max(1, damage)),
    scoreValue(std::max(0, points)),
    overtaken(false),
    nearMiss(false)
{
}

void TrafficVehicle::Update(float deltaTime)
{
    y += speed * speedMultiplier * deltaTime;
}

void TrafficVehicle::SetLanePosition(int laneIndex, float centerX)
{
    lane = std::clamp(laneIndex, 0, 3);
    x = centerX - width * 0.5f;
}

bool TrafficVehicle::TryMarkOvertaken(float playerY)
{
    if (!overtaken && y > playerY + height)
    {
        overtaken = true;
        return true;
    }
    return false;
}
