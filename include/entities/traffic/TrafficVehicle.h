#pragma once

#include "entities/Vehicle.h"

class TrafficVehicle : public Vehicle
{
protected:
    int lane;
    float speedMultiplier;
    int collisionDamage;
    int scoreValue;
    bool overtaken;
    bool nearMiss;

public:
    TrafficVehicle(
        float startY,
        float initialSpeed,
        int initialHealth,
        int startLane,
        int damage = 20,
        int points = 10
    );

    virtual ~TrafficVehicle() = default;

    void Update(float deltaTime) override;
    void Draw() const override = 0;

    void SetLanePosition(int laneIndex, float centerX);
    void SetSpeedMultiplier(float multiplier) { speedMultiplier = multiplier; }
    int GetLane() const { return lane; }
    int GetCollisionDamage() const { return collisionDamage; }
    int GetScoreValue() const { return scoreValue; }
    bool TryMarkOvertaken(float playerY);
    void MarkNearMiss() { nearMiss = true; }
    bool HadNearMiss() const { return nearMiss; }
};
