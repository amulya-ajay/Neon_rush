#pragma once

#include "entities/traffic/TrafficVehicle.h"

#include <array>
#include <memory>
#include <random>
#include <vector>

class TrafficManager
{
private:
    std::vector<std::unique_ptr<TrafficVehicle>> vehicles;
    std::mt19937 randomEngine;
    float spawnTimer;

    int ChooseLane();
    int ChooseTypeRoll(float elapsedSeconds);

public:
    TrafficManager();
    void Reset();
    void Update(float deltaTime, float screenHeight, float elapsedSeconds, float difficultyMultiplier, float spawnInterval, const std::array<float, 4>& laneCenters, float playerY);

    const std::vector<std::unique_ptr<TrafficVehicle>>& GetVehicles() const { return vehicles; }
    void RemoveVehicle(std::size_t index);
};
