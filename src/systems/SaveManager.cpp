#include "systems/SaveManager.h"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <string>

SaveManager::SaveManager()
    : SaveManager(std::filesystem::current_path() / "saves" / "neon_rush_save.txt")
{
}

SaveManager::SaveManager(const std::filesystem::path& path)
    : savePath(path.string())
{
}

SaveData SaveManager::Load() const
{
    SaveData data;
    std::ifstream input(savePath);
    if (!input)
    {
        return data;
    }

    std::string key;
    while (input >> key)
    {
        if (key == "high_score") input >> data.highScore;
        else if (key == "total_coins") input >> data.totalCoins;
        else if (key == "sound_volume") input >> data.settings.soundVolume;
        else if (key == "music_volume") input >> data.settings.musicVolume;
        else if (key == "fullscreen") input >> data.settings.fullscreen;
        else
        {
            std::string ignored;
            input >> ignored;
        }
        if (!input)
        {
            return SaveData{};
        }
    }

    data.highScore = std::max(0, data.highScore);
    data.totalCoins = std::max(0, data.totalCoins);
    data.settings.soundVolume = std::clamp(data.settings.soundVolume, 0.0f, 1.0f);
    data.settings.musicVolume = std::clamp(data.settings.musicVolume, 0.0f, 1.0f);
    return data;
}

bool SaveManager::Save(const SaveData& data) const
{
    std::error_code error;
    const std::filesystem::path path(savePath);
    std::filesystem::create_directories(path.parent_path(), error);
    if (error)
    {
        return false;
    }

    std::ofstream output(savePath, std::ios::trunc);
    if (!output)
    {
        return false;
    }
    output << "high_score " << std::max(0, data.highScore) << '\n'
        << "total_coins " << std::max(0, data.totalCoins) << '\n'
        << "sound_volume " << std::clamp(data.settings.soundVolume, 0.0f, 1.0f) << '\n'
        << "music_volume " << std::clamp(data.settings.musicVolume, 0.0f, 1.0f) << '\n'
        << "fullscreen " << data.settings.fullscreen << '\n';
    return static_cast<bool>(output);
}
