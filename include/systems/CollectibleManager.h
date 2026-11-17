#pragma once

#include "entities/Collectible.h"

#include <array>
#include <memory>
#include <random>
#include <vector>

class CollectibleManager
{
private:
    std::vector<std::unique_ptr<Collectible>> pickups;
    std::mt19937 randomEngine;
    float coinTimer;
    float powerUpTimer;

public:
    CollectibleManager();
    void Reset();
    void Update(float deltaTime, float screenHeight, float scrollSpeed, const std::array<float, 4>& laneCenters, float playerCenterX, float playerCenterY, bool magnetActive, float elapsedSeconds);
    const std::vector<std::unique_ptr<Collectible>>& GetPickups() const { return pickups; }
    void RemovePickup(std::size_t index);
};
