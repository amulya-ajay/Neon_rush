#pragma once

#include "core/GameObject.h"
#include "entities/Vehicle.h"

enum class PickupKind
{
    Coin,
    Nitro,
    Shield,
    Magnet
};

class Collectible : public GameObject
{
protected:
    float x;
    float y;
    float radius;
    float phase;
    bool collected;

    explicit Collectible(float startX, float startY, float pickupRadius);

public:
    ~Collectible() override = default;
    void Update(float deltaTime) override;
    void Draw() const override = 0;

    virtual PickupKind GetKind() const = 0;
    void Scroll(float deltaTime, float scrollSpeed, float playerCenterX, float playerCenterY, bool magnetActive);
    VehicleBounds GetBounds() const { return { x - radius, y - radius, radius * 2.0f, radius * 2.0f }; }
    bool IsCollected() const { return collected; }
    void MarkCollected() { collected = true; }
    float GetX() const { return x; }
    float GetY() const { return y; }
};

class Coin : public Collectible
{
public:
    Coin(float x, float y);
    void Draw() const override;
    PickupKind GetKind() const override { return PickupKind::Coin; }
};

class PowerUp : public Collectible
{
private:
    PickupKind kind;

public:
    PowerUp(float x, float y, PickupKind type);
    void Draw() const override;
    PickupKind GetKind() const override { return kind; }
};
