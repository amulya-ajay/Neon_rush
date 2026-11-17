#include "entities/PlayerCar.h"
#include "systems/ScoreManager.h"
#include "systems/SaveManager.h"
#include "systems/TrafficManager.h"
#include "systems/VehicleFactory.h"

#include <array>
#include <chrono>
#include <filesystem>
#include <iostream>

namespace
{
    bool Check(bool condition, const char* message)
    {
        if (!condition)
        {
            std::cerr << "FAILED: " << message << '\n';
            return false;
        }
        return true;
    }
}

int main()
{
    bool passed = true;
    const std::array<float, 4> laneCenters{ 120.0f, 300.0f, 480.0f, 660.0f };

    PlayerCar player(273.0f, 500.0f);
    player.Reset(laneCenters[1] - 27.0f, 500.0f, laneCenters);
    player.ChangeLane(-1);
    passed &= Check(player.GetCurrentLane() == 0, "player moves one lane left");
    player.ChangeLane(-1);
    passed &= Check(player.GetCurrentLane() == 0, "player stays within the left road boundary");
    player.ChangeLane(10);
    passed &= Check(player.GetCurrentLane() == 3, "player stays within the right road boundary");
    player.HandleCollision(35);
    passed &= Check(player.GetHealth() == 65, "vehicle collision reduces health");
    player.HandleCollision(1000);
    passed &= Check(player.GetHealth() == 0, "health clamps at zero");

    const auto sedan = VehicleFactory::Create(TrafficType::Sedan, -100.0f, 1);
    const auto sportsCar = VehicleFactory::Create(TrafficType::SportsCar, -100.0f, 1);
    const auto truck = VehicleFactory::Create(TrafficType::Truck, -100.0f, 1);
    const auto police = VehicleFactory::Create(TrafficType::PoliceCar, -100.0f, 1);
    passed &= Check(sedan->GetWidth() < truck->GetWidth(), "truck has a wider collision body than a sedan");
    passed &= Check(sportsCar->GetSpeed() > sedan->GetSpeed(), "sports car has a higher base speed");
    passed &= Check(police->GetScoreValue() > sedan->GetScoreValue(), "police car awards a larger overtake bonus");

    TrafficManager traffic;
    for (int frame = 0; frame < 20; ++frame)
    {
        traffic.Update(0.05f, 720.0f, 0.0f, 1.0f, 1.4f, laneCenters, 560.0f);
    }
    passed &= Check(!traffic.GetVehicles().empty(), "traffic manager automatically spawns vehicles");
    if (!traffic.GetVehicles().empty())
    {
        const auto& spawned = traffic.GetVehicles().front();
        const float expectedX = laneCenters[static_cast<std::size_t>(spawned->GetLane())] - spawned->GetWidth() * 0.5f;
        passed &= Check(spawned->GetX() == expectedX, "traffic uses the selected lane center");
    }

    ScoreManager score;
    for (int frame = 0; frame < 20; ++frame) score.Update(0.05f, 1.0f);
    passed &= Check(score.GetScore() >= 10, "survival time contributes score");
    score.AddCoin();
    passed &= Check(score.GetCoins() == 1 && score.GetCombo() > 1, "coin pickup adds currency and builds combo");
    score.AddOvertake(30, true);
    passed &= Check(score.GetOvertakes() == 1 && score.GetScore() > 10, "overtake and near miss contribute score");
    score.AddOvertake(10);
    passed &= Check(score.GetOvertakes() == 2, "ordinary overtake overload records a pass");

    const auto savePath = std::filesystem::temp_directory_path() /
        ("neon_rush_test_" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()) + ".txt");
    SaveManager saveManager(savePath);
    SaveData stored;
    stored.highScore = 12500;
    stored.totalCoins = 87;
    stored.settings.soundVolume = 0.35f;
    stored.settings.musicVolume = 0.65f;
    stored.settings.fullscreen = true;
    passed &= Check(saveManager.Save(stored), "save file writes successfully");
    const SaveData loaded = saveManager.Load();
    passed &= Check(loaded.highScore == stored.highScore && loaded.totalCoins == stored.totalCoins, "high score and coin total persist");
    passed &= Check(loaded.settings.fullscreen && loaded.settings.soundVolume > 0.34f && loaded.settings.musicVolume > 0.64f, "settings persist");
    std::error_code cleanupError;
    std::filesystem::remove(savePath, cleanupError);

    if (!passed)
    {
        return 1;
    }
    std::cout << "NEON RUSH logic checks passed.\n";
    return 0;
}
