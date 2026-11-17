#pragma once

#include <filesystem>
#include <string>

struct GameSettings
{
    float soundVolume = 0.55f;
    float musicVolume = 0.22f;
    bool fullscreen = false;
};

struct SaveData
{
    int highScore = 0;
    int totalCoins = 0;
    GameSettings settings;
};

class SaveManager
{
private:
    std::string savePath;

public:
    SaveManager();
    explicit SaveManager(const std::filesystem::path& path);
    SaveData Load() const;
    bool Save(const SaveData& data) const;
    const std::string& GetPath() const { return savePath; }
};
