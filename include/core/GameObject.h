#pragma once

class GameObject
{
public:
    virtual ~GameObject() = default;

    virtual void Update(float deltaTime) = 0;
    virtual void Draw() const = 0;
};