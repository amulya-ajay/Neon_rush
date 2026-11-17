#pragma once

#include <algorithm>

class DifficultyManager
{
public:
    float GetTrafficMultiplier(float elapsedSeconds) const
    {
        return 1.0f + std::min(0.82f, std::max(0.0f, elapsedSeconds) * 0.0082f);
    }

    float GetSpawnInterval(float elapsedSeconds) const
    {
        return std::max(0.62f, 1.65f - std::max(0.0f, elapsedSeconds) * 0.008f);
    }
};
