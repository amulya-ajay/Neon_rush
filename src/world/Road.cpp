#include "world/Road.h"

#include "raylib.h"

#include <algorithm>
#include <cmath>

namespace
{
    constexpr float ROAD_WIDTH_RATIO = 0.60f;
    constexpr float DASH_HEIGHT = 52.0f;
    constexpr float DASH_GAP = 46.0f;
}

Road::Road()
    : dashOffset(0.0f), elapsed(0.0f)
{
}

void Road::Update(float deltaTime, float scrollSpeed)
{
    elapsed += deltaTime;
    const float cycle = DASH_HEIGHT + DASH_GAP;
    dashOffset = std::fmod(dashOffset + std::max(0.0f, scrollSpeed) * deltaTime, cycle);
}

float Road::GetWidth() const
{
    return std::min(static_cast<float>(GetScreenWidth()) * ROAD_WIDTH_RATIO, 920.0f);
}

float Road::GetLeft() const
{
    return (static_cast<float>(GetScreenWidth()) - GetWidth()) * 0.5f;
}

float Road::GetRight() const
{
    return GetLeft() + GetWidth();
}

float Road::GetLaneWidth() const
{
    return GetWidth() / 4.0f;
}

std::array<float, 4> Road::GetLaneCenters() const
{
    const float left = GetLeft();
    const float laneWidth = GetLaneWidth();
    return { left + laneWidth * 0.5f, left + laneWidth * 1.5f, left + laneWidth * 2.5f, left + laneWidth * 3.5f };
}

void Road::Draw() const
{
    const float screenHeight = static_cast<float>(GetScreenHeight());
    const float left = GetLeft();
    const float right = GetRight();
    const float laneWidth = GetLaneWidth();

    // Asphalt, shoulders and soft neon spill.
    DrawRectangle(static_cast<int>(left - 22.0f), 0, static_cast<int>(GetWidth() + 44.0f), GetScreenHeight(), Fade(MAGENTA, 0.08f));
    DrawRectangle(static_cast<int>(left), 0, static_cast<int>(GetWidth()), GetScreenHeight(), Color{ 19, 23, 34, 255 });
    DrawRectangle(static_cast<int>(left + 5.0f), 0, static_cast<int>(GetWidth() - 10.0f), GetScreenHeight(), Color{ 24, 28, 39, 255 });

    // A subtle repeating asphalt grain gives the road some depth without assets.
    for (int i = 0; i < 24; ++i)
    {
        const float y = std::fmod(static_cast<float>(i) * 91.0f + dashOffset * 1.6f, screenHeight);
        const float inset = 18.0f + static_cast<float>((i * 37) % 90);
        DrawLineEx(Vector2{ left + inset, y }, Vector2{ std::min(right - 16.0f, left + inset + 64.0f), y }, 1.0f, Fade(Color{ 101, 118, 146, 255 }, 0.10f));
    }

    for (int lane = 1; lane < 4; ++lane)
    {
        const float lineX = left + laneWidth * static_cast<float>(lane);
        for (float y = -DASH_HEIGHT + dashOffset; y < screenHeight; y += DASH_HEIGHT + DASH_GAP)
        {
            DrawRectangle(static_cast<int>(lineX - 2.0f), static_cast<int>(y), 4, static_cast<int>(DASH_HEIGHT), Fade(SKYBLUE, 0.16f));
            DrawRectangle(static_cast<int>(lineX - 1.0f), static_cast<int>(y), 2, static_cast<int>(DASH_HEIGHT), Fade(WHITE, 0.55f));
        }
    }

    DrawRectangle(static_cast<int>(left - 4.0f), 0, 4, GetScreenHeight(), Fade(MAGENTA, 0.85f));
    DrawRectangle(static_cast<int>(left - 11.0f), 0, 2, GetScreenHeight(), Fade(MAGENTA, 0.45f));
    DrawRectangle(static_cast<int>(right), 0, 4, GetScreenHeight(), Fade(SKYBLUE, 0.88f));
    DrawRectangle(static_cast<int>(right + 8.0f), 0, 2, GetScreenHeight(), Fade(SKYBLUE, 0.45f));

    const float lightSpacing = 96.0f;
    const float lightOffset = std::fmod(dashOffset * 1.25f, lightSpacing);
    for (float y = -lightSpacing + lightOffset; y < screenHeight; y += lightSpacing)
    {
        const float pulse = 0.55f + 0.30f * std::sin(elapsed * 3.0f + y * 0.03f);
        DrawCircleV(Vector2{ left - 18.0f, y }, 4.0f, Fade(MAGENTA, pulse));
        DrawCircleV(Vector2{ right + 18.0f, y + lightSpacing * 0.5f }, 4.0f, Fade(SKYBLUE, pulse));
    }
}
