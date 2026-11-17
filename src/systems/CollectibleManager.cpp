#include "systems/CollectibleManager.h"

#include <algorithm>
#include <chrono>
#include <cstddef>

CollectibleManager::CollectibleManager()
    : randomEngine(static_cast<unsigned int>(std::chrono::high_resolution_clock::now().time_since_epoch().count() + 41)),
    coinTimer(1.0f),
    powerUpTimer(7.5f)
{
}

void CollectibleManager::Reset()
{
    pickups.clear();
    coinTimer = 1.0f;
    powerUpTimer = 7.5f;
}

void CollectibleManager::Update(
    float deltaTime,
    float screenHeight,
    float scrollSpeed,
    const std::array<float, 4>& laneCenters,
    float playerCenterX,
    float playerCenterY,
    bool magnetActive,
    float elapsedSeconds
)
{
    const float dt = std::clamp(deltaTime, 0.0f, 0.05f);
    coinTimer -= dt;
    powerUpTimer -= dt;
    if (coinTimer <= 0.0f)
    {
        std::uniform_int_distribution<int> lanePick(0, 3);
        const float x = laneCenters[static_cast<std::size_t>(lanePick(randomEngine))];
        for (int coin = 0; coin < 3; ++coin)
        {
            pickups.push_back(std::make_unique<Coin>(x, -24.0f - coin * 54.0f));
        }
        coinTimer = 1.75f;
    }
    if (powerUpTimer <= 0.0f && elapsedSeconds > 5.0f)
    {
        std::uniform_int_distribution<int> lanePick(0, 3);
        std::uniform_int_distribution<int> typePick(0, 2);
        const int lane = lanePick(randomEngine);
        const int type = typePick(randomEngine);
        const PickupKind kind = type == 0 ? PickupKind::Nitro : (type == 1 ? PickupKind::Shield : PickupKind::Magnet);
        pickups.push_back(std::make_unique<PowerUp>(laneCenters[static_cast<std::size_t>(lane)], -35.0f, kind));
        powerUpTimer = 10.0f;
    }

    for (auto& pickup : pickups)
    {
        pickup->Scroll(dt, scrollSpeed, playerCenterX, playerCenterY, magnetActive);
    }
    pickups.erase(std::remove_if(pickups.begin(), pickups.end(), [screenHeight](const auto& pickup)
    {
        return pickup->GetY() > screenHeight + 40.0f || pickup->IsCollected();
    }), pickups.end());
}

void CollectibleManager::RemovePickup(std::size_t index)
{
    if (index < pickups.size())
    {
        pickups.erase(pickups.begin() + static_cast<std::ptrdiff_t>(index));
    }
}
