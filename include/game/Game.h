#pragma once

#include "entities/PlayerCar.h"
#include "systems/AudioManager.h"
#include "systems/CollectibleManager.h"
#include "systems/DifficultyManager.h"
#include "systems/ParticleSystem.h"
#include "systems/SaveManager.h"
#include "systems/ScoreManager.h"
#include "systems/TrafficManager.h"
#include "world/CityBackground.h"
#include "world/Road.h"

class Game
{
private:
    enum class State
    {
        Menu,
        Playing,
        Paused,
        GameOver,
        Settings
    };

    SaveManager saveManager;
    SaveData saveData;
    AudioManager audio;
    CityBackground city;
    Road road;
    PlayerCar player;
    TrafficManager traffic;
    CollectibleManager collectibles;
    ParticleSystem particles;
    ScoreManager score;
    DifficultyManager difficulty;

    State state;
    int menuSelection;
    int pauseSelection;
    int settingsSelection;
    float elapsedSeconds;
    float shakeTimer;
    float shakeStrength;
    bool quitRequested;
    bool shutdown;

    void BeginRun();
    void EndRun();
    void UpdateMenu();
    void UpdatePause();
    void UpdateSettings();
    void UpdateGameOver();
    void HandleCollisions();
    void CollectPickups();
    void SaveProgress();
    void ToggleFullscreen();

    void DrawBackdrop(bool titleMode) const;
    void DrawGameplay(bool includeHud) const;
    void DrawHud() const;
    void DrawMainMenu() const;
    void DrawPauseMenu() const;
    void DrawGameOver() const;
    void DrawSettings() const;

public:
    Game();
    ~Game();
    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;

    void Update(float deltaTime);
    void Draw() const;
    void Shutdown();
    bool ShouldQuit() const { return quitRequested; }
};
