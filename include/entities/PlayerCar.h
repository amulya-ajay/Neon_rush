#pragma once

#include "entities/Vehicle.h"

#include <array>

class PlayerCar : public Vehicle
{
private:
    int currentLane;
    float targetX;
    float nitroTimer;
    float shieldTimer;
    float magnetTimer;
    float invulnerabilityTimer;
    int nitroCharges;
    std::array<float, 4> laneCenters;

public:
    PlayerCar(float startX, float startY);

    void Update(float deltaTime) override;
    void Draw() const override;

    void ChangeLane(int direction);
    void SetLaneCenters(const std::array<float, 4>& centers);
    void SetY(float newY) { y = newY; }
    void Reset(float startX, float startY, const std::array<float, 4>& centers);
    void ActivateNitro();
    void AddNitroCharge();
    void ActivateShield();
    void ActivateMagnet();
    void StartInvulnerability(float duration);

    bool IsShieldActive() const;
    bool IsNitroActive() const;
    bool IsMagnetActive() const;
    bool IsInvulnerable() const;
    int GetNitroCharges() const { return nitroCharges; }
    int GetCurrentLane() const { return currentLane; }
    float GetNitroTimeRemaining() const { return nitroTimer; }
    float GetShieldTimeRemaining() const { return shieldTimer; }
    float GetMagnetTimeRemaining() const { return magnetTimer; }
    float GetSpeedMultiplier() const { return IsNitroActive() ? 1.65f : 1.0f; }
};
