#pragma once

#include "raylib.h"

#include <array>

enum class SoundEffect
{
    Ui,
    Start,
    Coin,
    Nitro,
    Collision,
    PowerUp,
    GameOver
};

class AudioManager
{
private:
    static constexpr int MUSIC_SAMPLE_RATE = 22050;
    static constexpr int MUSIC_CHUNK_SIZE = 1024;

    std::array<Sound, 7> effects;
    AudioStream musicStream{};
    bool audioReady;
    bool musicPlaying;
    unsigned long long musicCursor;
    float soundVolume;
    float musicVolume;

    Sound CreateTone(float frequency, float duration, float amplitude) const;
    void ApplyVolumes();

public:
    AudioManager();
    ~AudioManager();
    AudioManager(const AudioManager&) = delete;
    AudioManager& operator=(const AudioManager&) = delete;

    bool Initialize();
    void Shutdown();
    void Update();
    void Play(SoundEffect effect);
    void SetVolumes(float sound, float music);
    bool IsAvailable() const { return audioReady; }
};
