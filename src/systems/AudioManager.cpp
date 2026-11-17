#include "systems/AudioManager.h"

#include <algorithm>
#include <cmath>

namespace
{
    constexpr float kPi = 3.14159265358979323846f;
}

AudioManager::AudioManager()
    : audioReady(false), musicPlaying(false), musicCursor(0), soundVolume(0.55f), musicVolume(0.22f)
{
}

AudioManager::~AudioManager()
{
    Shutdown();
}

Sound AudioManager::CreateTone(float frequency, float duration, float amplitude) const
{
    const unsigned int frameCount = static_cast<unsigned int>(MUSIC_SAMPLE_RATE * duration);
    Wave wave{};
    wave.frameCount = frameCount;
    wave.sampleRate = MUSIC_SAMPLE_RATE;
    wave.sampleSize = 16;
    wave.channels = 1;
    wave.data = MemAlloc(frameCount * sizeof(short));
    if (wave.data == nullptr)
    {
        return Sound{};
    }

    auto* samples = static_cast<short*>(wave.data);
    for (unsigned int i = 0; i < frameCount; ++i)
    {
        const float t = static_cast<float>(i) / MUSIC_SAMPLE_RATE;
        const float fadeOut = 1.0f - static_cast<float>(i) / std::max(1u, frameCount);
        const float envelope = std::min(1.0f, t * 35.0f) * fadeOut;
        const float value = std::sin(2.0f * kPi * frequency * t) * amplitude * envelope;
        samples[i] = static_cast<short>(std::clamp(value, -1.0f, 1.0f) * 32767.0f);
    }
    const Sound sound = LoadSoundFromWave(wave);
    UnloadWave(wave);
    return sound;
}

bool AudioManager::Initialize()
{
    InitAudioDevice();
    audioReady = IsAudioDeviceReady();
    if (!audioReady)
    {
        return false;
    }

    effects[static_cast<std::size_t>(SoundEffect::Ui)] = CreateTone(640.0f, 0.11f, 0.18f);
    effects[static_cast<std::size_t>(SoundEffect::Start)] = CreateTone(440.0f, 0.27f, 0.22f);
    effects[static_cast<std::size_t>(SoundEffect::Coin)] = CreateTone(1040.0f, 0.15f, 0.24f);
    effects[static_cast<std::size_t>(SoundEffect::Nitro)] = CreateTone(150.0f, 0.38f, 0.22f);
    effects[static_cast<std::size_t>(SoundEffect::Collision)] = CreateTone(92.0f, 0.32f, 0.30f);
    effects[static_cast<std::size_t>(SoundEffect::PowerUp)] = CreateTone(820.0f, 0.24f, 0.22f);
    effects[static_cast<std::size_t>(SoundEffect::GameOver)] = CreateTone(180.0f, 0.56f, 0.23f);

    musicStream = LoadAudioStream(MUSIC_SAMPLE_RATE, 16, 1);
    musicPlaying = musicStream.buffer != nullptr;
    if (musicPlaying)
    {
        SetAudioStreamVolume(musicStream, musicVolume);
        PlayAudioStream(musicStream);
    }
    ApplyVolumes();
    return true;
}

void AudioManager::ApplyVolumes()
{
    if (!audioReady)
    {
        return;
    }
    for (Sound sound : effects)
    {
        if (sound.stream.buffer != nullptr)
        {
            SetSoundVolume(sound, soundVolume);
        }
    }
    if (musicPlaying)
    {
        SetAudioStreamVolume(musicStream, musicVolume);
    }
}

void AudioManager::SetVolumes(float sound, float music)
{
    soundVolume = std::clamp(sound, 0.0f, 1.0f);
    musicVolume = std::clamp(music, 0.0f, 1.0f);
    ApplyVolumes();
}

void AudioManager::Play(SoundEffect effect)
{
    if (!audioReady)
    {
        return;
    }
    const Sound sound = effects[static_cast<std::size_t>(effect)];
    if (sound.stream.buffer != nullptr)
    {
        PlaySound(sound);
    }
}

void AudioManager::Update()
{
    if (!audioReady || !musicPlaying || musicVolume <= 0.001f || !IsAudioStreamProcessed(musicStream))
    {
        return;
    }

    std::array<short, MUSIC_CHUNK_SIZE> samples{};
    constexpr std::array<float, 8> notes{ 55.0f, 65.41f, 73.42f, 82.41f, 73.42f, 65.41f, 61.74f, 73.42f };
    for (int i = 0; i < MUSIC_CHUNK_SIZE; ++i)
    {
        const unsigned long long sample = musicCursor++;
        const float time = static_cast<float>(sample) / MUSIC_SAMPLE_RATE;
        const std::size_t noteIndex = static_cast<std::size_t>((sample / (MUSIC_SAMPLE_RATE * 2)) % notes.size());
        const float root = notes[noteIndex];
        const float pad = std::sin(2.0f * kPi * root * time) * 0.10f;
        const float fifth = std::sin(2.0f * kPi * root * 1.5f * time) * 0.035f;
        const float shimmer = std::sin(2.0f * kPi * root * 4.0f * time) * 0.012f;
        const float beat = 0.70f + 0.30f * std::sin(2.0f * kPi * 0.5f * time);
        samples[static_cast<std::size_t>(i)] = static_cast<short>(std::clamp((pad + fifth + shimmer) * beat, -1.0f, 1.0f) * 32767.0f);
    }
    UpdateAudioStream(musicStream, samples.data(), MUSIC_CHUNK_SIZE);
}

void AudioManager::Shutdown()
{
    if (!audioReady)
    {
        return;
    }
    if (musicPlaying)
    {
        StopAudioStream(musicStream);
        UnloadAudioStream(musicStream);
        musicPlaying = false;
    }
    for (Sound sound : effects)
    {
        if (sound.stream.buffer != nullptr)
        {
            UnloadSound(sound);
        }
    }
    CloseAudioDevice();
    audioReady = false;
}
