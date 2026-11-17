#include "systems/TrafficManager.h"

#include "systems/VehicleFactory.h"

#include <algorithm>
#include <chrono>

TrafficManager::TrafficManager()
    : randomEngine(static_cast<unsigned int>(std::chrono::high_resolution_clock::now().time_since_epoch().count())),
    spawnTimer(0.65f)
{
}

void TrafficManager::Reset()
{
    vehicles.clear();
    spawnTimer = 0.65f;
}

int TrafficManager::ChooseLane()
{
    std::array<int, 4> lanes{ 0, 1, 2, 3 };
    std::shuffle(lanes.begin(), lanes.end(), randomEngine);
    for (int lane : lanes)
    {
        const bool blockedNearSpawn = std::any_of(vehicles.begin(), vehicles.end(), [lane](const auto& vehicle)
        {
            return vehicle->GetLane() == lane && vehicle->GetY() < 190.0f && vehicle->GetY() > -300.0f;
        });
        if (!blockedNearSpawn)
        {
            return lane;
        }
    }
    return -1;
}

int TrafficManager::ChooseTypeRoll(float elapsedSeconds)
{
    std::uniform_int_distribution<int> distribution(0, 99);
    int roll = distribution(randomEngine);
    if (elapsedSeconds > 45.0f && roll >= 90)
    {
        return 3;
    }
    if (roll < 45)
    {
        return 0;
    }
    if (roll < 70)
    {
        return 1;
    }
    return 2;
}

void TrafficManager::Update(
    float deltaTime,
    float screenHeight,
    float elapsedSeconds,
    float difficultyMultiplier,
    float spawnInterval,
    const std::array<float, 4>& laneCenters,
    float playerY
)
{
    const float dt = std::clamp(deltaTime, 0.0f, 0.05f);
    spawnTimer -= dt;
    if (spawnTimer <= 0.0f)
    {
        const int lane = ChooseLane();
        if (lane >= 0)
        {
            const int roll = ChooseTypeRoll(elapsedSeconds);
            TrafficType type = TrafficType::Sedan;
            if (roll == 1) type = TrafficType::SportsCar;
            if (roll == 2) type = TrafficType::Truck;
            if (roll == 3) type = TrafficType::PoliceCar;

            std::uniform_real_distribution<float> gap(0.0f, 90.0f);
            const float spawnY = -145.0f - gap(randomEngine);
            auto vehicle = VehicleFactory::Create(type, spawnY, lane);
            vehicle->SetLanePosition(lane, laneCenters[static_cast<std::size_t>(lane)]);
            vehicle->SetSpeedMultiplier(difficultyMultiplier);
            vehicles.push_back(std::move(vehicle));
            spawnTimer = spawnInterval;
        }
        else
        {
            // Retry soon when all spawn lanes are temporarily occupied.
            spawnTimer = 0.18f;
        }
    }

    for (auto& vehicle : vehicles)
    {
        vehicle->SetLanePosition(vehicle->GetLane(), laneCenters[static_cast<std::size_t>(vehicle->GetLane())]);
        vehicle->SetSpeedMultiplier(difficultyMultiplier);
        vehicle->Update(dt);
    }

    vehicles.erase(std::remove_if(vehicles.begin(), vehicles.end(), [screenHeight, playerY](const auto& vehicle)
    {
        return vehicle->GetY() > screenHeight + vehicle->GetHeight() || vehicle->GetY() > playerY + 260.0f;
    }), vehicles.end());
}

void TrafficManager::RemoveVehicle(std::size_t index)
{
    if (index < vehicles.size())
    {
        vehicles.erase(vehicles.begin() + static_cast<std::ptrdiff_t>(index));
    }
}
