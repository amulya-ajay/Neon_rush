#include "world/CityBackground.h"

#include "raylib.h"

#include <algorithm>
#include <cmath>

CityBackground::CityBackground()
    : elapsed(0.0f)
{
}

void CityBackground::Update(float deltaTime)
{
    elapsed += deltaTime;
}

void CityBackground::Draw(bool titleMode) const
{
    const int screenWidth = GetScreenWidth();
    const int screenHeight = GetScreenHeight();
    DrawRectangleGradientV(0, 0, screenWidth, screenHeight, Color{ 4, 7, 22, 255 }, Color{ 12, 9, 29, 255 });

    for (int i = 0; i < 90; ++i)
    {
        const int x = (i * 97 + 31) % screenWidth;
        const int y = (i * 53 + 19) % std::max(1, screenHeight / 2);
        const float twinkle = 0.18f + 0.30f * (0.5f + 0.5f * std::sin(elapsed * (1.5f + (i % 4)) + static_cast<float>(i)));
        DrawCircleV(Vector2{ static_cast<float>(x), static_cast<float>(y) }, i % 13 == 0 ? 2.0f : 1.0f, Fade(i % 3 == 0 ? MAGENTA : SKYBLUE, twinkle));
    }

    const int buildingCount = 24;
    const float cellWidth = static_cast<float>(screenWidth) / buildingCount;
    const float skylineBase = static_cast<float>(screenHeight) * 0.77f;
    for (int i = 0; i < buildingCount; ++i)
    {
        const int seed = i * 7919 + 17;
        const float height = static_cast<float>(screenHeight) * (0.12f + static_cast<float>((seed * 37) % 230) / 1000.0f);
        const float x = static_cast<float>(i) * cellWidth;
        const float width = cellWidth + 2.0f;
        const float top = skylineBase - height;
        DrawRectangle(static_cast<int>(x), static_cast<int>(top), static_cast<int>(width), static_cast<int>(height + screenHeight * 0.25f), Color{ 7, 13, 32, 255 });
        DrawRectangle(static_cast<int>(x), static_cast<int>(top), static_cast<int>(width), 2, Fade(i % 4 == 0 ? MAGENTA : SKYBLUE, 0.34f));

        const int rows = std::max(1, static_cast<int>(height / 22.0f));
        for (int row = 0; row < rows; ++row)
        {
            for (int column = 0; column < 3; ++column)
            {
                const int on = (seed + row * 13 + column * 7) % 5;
                if (on < 3)
                {
                    const float flicker = 0.24f + 0.22f * (0.5f + 0.5f * std::sin(elapsed * 1.7f + static_cast<float>(seed + row * 3 + column)));
                    const Color windowColor = (seed + row + column) % 4 == 0 ? MAGENTA : SKYBLUE;
                    DrawRectangle(static_cast<int>(x + 7.0f + column * 12.0f), static_cast<int>(top + 12.0f + row * 19.0f), 5, 8, Fade(windowColor, flicker));
                }
            }
        }
    }

    const float horizonY = static_cast<float>(screenHeight) * 0.77f;
    DrawRectangleGradientV(0, static_cast<int>(horizonY - 20.0f), screenWidth, static_cast<int>(screenHeight - horizonY + 20.0f), Fade(MAGENTA, titleMode ? 0.12f : 0.07f), Fade(BLACK, 0.0f));
}
