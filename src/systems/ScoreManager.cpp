#include "systems/ScoreManager.h"

#include <algorithm>

ScoreManager::ScoreManager()
    : score(0), coins(0), overtakes(0), combo(1), comboTimer(0.0f), survivalRemainder(0.0f), distanceKm(0.0f)
{
}

void ScoreManager::Reset()
{
    score = 0;
    coins = 0;
    overtakes = 0;
    combo = 1;
    comboTimer = 0.0f;
    survivalRemainder = 0.0f;
    distanceKm = 0.0f;
}

ScoreManager& ScoreManager::operator+=(int points)
{
    score = std::max(0, score + points);
    return *this;
}

void ScoreManager::BuildCombo()
{
    combo = std::min(6, combo + 1);
    comboTimer = 4.0f;
}

void ScoreManager::Update(float deltaTime, float speedMultiplier)
{
    const float dt = std::clamp(deltaTime, 0.0f, 0.05f);
    survivalRemainder += dt;
    distanceKm += dt * (0.075f * std::max(0.5f, speedMultiplier));
    while (survivalRemainder >= 1.0f)
    {
        *this += 10 * combo;
        survivalRemainder -= 1.0f;
    }
    comboTimer = std::max(0.0f, comboTimer - dt);
    if (comboTimer <= 0.0f)
    {
        combo = 1;
    }
}

void ScoreManager::AddCoin()
{
    ++coins;
    BuildCombo();
    *this += 50 * combo;
}

void ScoreManager::AddOvertake(int points)
{
    AddOvertake(points, false);
}

void ScoreManager::AddOvertake(int points, bool nearMiss)
{
    ++overtakes;
    BuildCombo();
    *this += std::max(0, points) * combo;
    if (nearMiss)
    {
        *this += 75 * combo;
        comboTimer = 5.0f;
    }
}

void ScoreManager::AddNearMiss()
{
    *this += 75 * combo;
    comboTimer = 5.0f;
}
