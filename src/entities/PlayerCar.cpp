#include "entities/PlayerCar.h"

#include "raylib.h"

#include <algorithm>
#include <cmath>

namespace
{
    constexpr float CAR_WIDTH = 54.0f;
    constexpr float CAR_HEIGHT = 90.0f;

    constexpr float NITRO_DURATION = 3.0f;
    constexpr float SHIELD_DURATION = 6.0f;
    constexpr float MAGNET_DURATION = 8.0f;
    constexpr float LANE_CHANGE_SPEED = 13.0f;
}

PlayerCar::PlayerCar(float startX, float startY)
    : Vehicle(startX, startY, 400.0f, 100),
    currentLane(1),
    targetX(startX),
    nitroTimer(0.0f),
    shieldTimer(0.0f),
    magnetTimer(0.0f),
    invulnerabilityTimer(0.0f),
    nitroCharges(1),
    laneCenters{ 352.0f, 544.0f, 736.0f, 928.0f }
{
    SetSize(CAR_WIDTH, CAR_HEIGHT);
}

void PlayerCar::Update(float deltaTime)
{
    if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A))
    {
        ChangeLane(-1);
    }
    else if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D))
    {
        ChangeLane(1);
    }

    if (IsKeyPressed(KEY_SPACE))
    {
        ActivateNitro();
    }

    nitroTimer = std::max(0.0f, nitroTimer - deltaTime);
    shieldTimer = std::max(0.0f, shieldTimer - deltaTime);
    magnetTimer = std::max(0.0f, magnetTimer - deltaTime);
    invulnerabilityTimer = std::max(0.0f, invulnerabilityTimer - deltaTime);

    const float blend = std::min(1.0f, std::max(0.0f, deltaTime) * LANE_CHANGE_SPEED);
    x += (targetX - x) * blend;
    if (std::abs(targetX - x) < 0.5f)
    {
        x = targetX;
    }
}

void PlayerCar::Draw() const
{
    if (IsShieldActive())
    {
        DrawCircleV(Vector2{ x + CAR_WIDTH * 0.5f, y + CAR_HEIGHT * 0.5f }, 62.0f, Fade(SKYBLUE, 0.10f));
        DrawCircleLines(static_cast<int>(x + CAR_WIDTH * 0.5f), static_cast<int>(y + CAR_HEIGHT * 0.5f), 60.0f, Fade(SKYBLUE, 0.68f));
    }

    if (IsNitroActive())
    {
        const float flicker = static_cast<float>(GetTime() * 30.0) - static_cast<int>(GetTime() * 30.0);
        DrawTriangle(
            Vector2{ x + 12.0f, y + CAR_HEIGHT - 5.0f },
            Vector2{ x + CAR_WIDTH * 0.5f, y + CAR_HEIGHT + 18.0f + flicker * 13.0f },
            Vector2{ x + CAR_WIDTH - 12.0f, y + CAR_HEIGHT - 5.0f },
            Fade(ORANGE, 0.78f)
        );
        DrawTriangle(
            Vector2{ x + 21.0f, y + CAR_HEIGHT - 3.0f },
            Vector2{ x + CAR_WIDTH * 0.5f, y + CAR_HEIGHT + 10.0f + flicker * 8.0f },
            Vector2{ x + CAR_WIDTH - 21.0f, y + CAR_HEIGHT - 3.0f },
            Fade(SKYBLUE, 0.9f)
        );
    }

    // A short flash communicates the temporary collision grace period.
    if (IsInvulnerable() && static_cast<int>(GetTime() * 12.0) % 2 == 0)
    {
        return;
    }

    DrawRectangle(static_cast<int>(x + 4.0f), static_cast<int>(y + 8.0f), static_cast<int>(CAR_WIDTH - 8.0f), static_cast<int>(CAR_HEIGHT - 12.0f), Fade(BLACK, 0.55f));
    DrawRectangleRounded(Rectangle{ x, y, CAR_WIDTH, CAR_HEIGHT }, 0.28f, 8, IsNitroActive() ? SKYBLUE : MAGENTA);
    DrawRectangleRoundedLinesEx(Rectangle{ x + 1.0f, y + 1.0f, CAR_WIDTH - 2.0f, CAR_HEIGHT - 2.0f }, 0.26f, 8, 2.0f, Fade(WHITE, 0.72f));

    // Windshield and cabin.
    DrawRectangleRounded(Rectangle{ x + 8.0f, y + 18.0f, CAR_WIDTH - 16.0f, 24.0f }, 0.25f, 6, DARKBLUE);
    DrawRectangle(static_cast<int>(x + 10.0f), static_cast<int>(y + 20.0f), static_cast<int>(CAR_WIDTH - 20.0f), 2, Fade(SKYBLUE, 0.85f));
    DrawRectangle(static_cast<int>(x + 9.0f), static_cast<int>(y + 48.0f), static_cast<int>(CAR_WIDTH - 18.0f), 3, Fade(WHITE, 0.58f));

    // Headlights and tail lamps.
    DrawRectangle(static_cast<int>(x + 5.0f), static_cast<int>(y + 4.0f), 11, 4, Fade(SKYBLUE, 0.95f));
    DrawRectangle(static_cast<int>(x + CAR_WIDTH - 16.0f), static_cast<int>(y + 4.0f), 11, 4, Fade(SKYBLUE, 0.95f));
    DrawRectangle(static_cast<int>(x + 6.0f), static_cast<int>(y + CAR_HEIGHT - 8.0f), 10, 4, RED);
    DrawRectangle(static_cast<int>(x + CAR_WIDTH - 16.0f), static_cast<int>(y + CAR_HEIGHT - 8.0f), 10, 4, RED);

    if (IsMagnetActive())
    {
        const float pulse = 0.35f + 0.25f * std::sin(static_cast<float>(GetTime() * 5.0));
        DrawCircleLines(static_cast<int>(x + CAR_WIDTH * 0.5f), static_cast<int>(y + CAR_HEIGHT * 0.5f), 50.0f + pulse * 10.0f, Fade(GOLD, pulse));
    }
}

void PlayerCar::ChangeLane(int direction)
{
    currentLane += direction;
    currentLane = std::clamp(currentLane, 0, 3);
    targetX = laneCenters[static_cast<std::size_t>(currentLane)] - CAR_WIDTH * 0.5f;
}

void PlayerCar::SetLaneCenters(const std::array<float, 4>& centers)
{
    laneCenters = centers;
    targetX = laneCenters[static_cast<std::size_t>(currentLane)] - CAR_WIDTH * 0.5f;
}

void PlayerCar::Reset(float startX, float startY, const std::array<float, 4>& centers)
{
    currentLane = 1;
    laneCenters = centers;
    x = startX;
    y = startY;
    targetX = laneCenters[1] - CAR_WIDTH * 0.5f;
    speed = 400.0f;
    health = 100;
    nitroTimer = 0.0f;
    shieldTimer = 0.0f;
    magnetTimer = 0.0f;
    invulnerabilityTimer = 0.0f;
    nitroCharges = 1;
}

void PlayerCar::ActivateNitro()
{
    if (nitroCharges > 0 && !IsNitroActive())
    {
        --nitroCharges;
        nitroTimer = NITRO_DURATION;
    }
}

void PlayerCar::AddNitroCharge()
{
    nitroCharges = std::min(3, nitroCharges + 1);
}

void PlayerCar::ActivateShield()
{
    shieldTimer = SHIELD_DURATION;
}

void PlayerCar::ActivateMagnet()
{
    magnetTimer = MAGNET_DURATION;
}

void PlayerCar::StartInvulnerability(float duration)
{
    invulnerabilityTimer = std::max(invulnerabilityTimer, std::max(0.0f, duration));
}

bool PlayerCar::IsShieldActive() const
{
    return shieldTimer > 0.0f;
}

bool PlayerCar::IsNitroActive() const
{
    return nitroTimer > 0.0f;
}

bool PlayerCar::IsMagnetActive() const
{
    return magnetTimer > 0.0f;
}

bool PlayerCar::IsInvulnerable() const
{
    return invulnerabilityTimer > 0.0f;
}
