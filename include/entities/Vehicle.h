#pragma once

#include "core/GameObject.h"

struct VehicleBounds
{
    float x = 0.0f;
    float y = 0.0f;
    float width = 0.0f;
    float height = 0.0f;
};

class Vehicle : public GameObject
{
protected:
    float x;
    float y;
    float speed;
    float width;
    float height;
    int health;

    void SetSize(float newWidth, float newHeight);

public:
    Vehicle(float startX, float startY, float initialSpeed, int initialHealth);
    ~Vehicle() override = default;

    void Update(float deltaTime) override = 0;
    void Draw() const override = 0;

    virtual void HandleCollision(int damage);

    float GetX() const { return x; }
    float GetY() const { return y; }
    float GetWidth() const { return width; }
    float GetHeight() const { return height; }
    float GetSpeed() const { return speed; }
    int GetHealth() const { return health; }
    VehicleBounds GetBounds() const { return { x, y, width, height }; }

};
