#pragma once

class ScoreManager
{
private:
    int score;
    int coins;
    int overtakes;
    int combo;
    float comboTimer;
    float survivalRemainder;
    float distanceKm;

    void BuildCombo();

public:
    ScoreManager();
    void Reset();
    void Update(float deltaTime, float speedMultiplier);
    void AddCoin();
    void AddOvertake(int points);
    void AddOvertake(int points, bool nearMiss);
    void AddNearMiss();

    // Points compose naturally with the running total and keep score changes centralized.
    ScoreManager& operator+=(int points);

    int GetScore() const { return score; }
    int GetCoins() const { return coins; }
    int GetOvertakes() const { return overtakes; }
    int GetCombo() const { return combo; }
    float GetDistanceKm() const { return distanceKm; }
};
