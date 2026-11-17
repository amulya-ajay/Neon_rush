#include "entities/Vehicle.h"

#include <algorithm>

Vehicle::Vehicle(float startX, float startY, float initialSpeed, int initialHealth)
    : x(startX),
    y(startY),
    speed(initialSpeed),
    width(54.0f),
    height(90.0f),
    health(std::max(0, initialHealth))
{
}

void Vehicle::SetSize(float newWidth, float newHeight)
{
    width = std::max(1.0f, newWidth);
    height = std::max(1.0f, newHeight);
}

void Vehicle::HandleCollision(int damage)
{
    health = std::max(0, health - std::max(0, damage));
}
