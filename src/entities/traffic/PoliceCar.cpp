#include "entities/traffic/PoliceCar.h"

#include "raylib.h"

#include <cmath>

PoliceCar::PoliceCar(float startY, int startLane)
    : TrafficVehicle(startY, 220.0f, 120, startLane, 28, 65)
{
    SetSize(60.0f, 102.0f);
}

void PoliceCar::Draw() const
{
    const float pulse = 0.4f + 0.5f * std::sin(static_cast<float>(GetTime() * 12.0));
    DrawRectangle(static_cast<int>(x - 5.0f), static_cast<int>(y + 12.0f), 70, 80, Fade(SKYBLUE, 0.12f));
    DrawRectangleRounded(Rectangle{ x, y, width, height }, 0.22f, 8, Color{ 31, 42, 66, 255 });
    DrawRectangleRoundedLinesEx(Rectangle{ x + 1.0f, y + 1.0f, width - 2.0f, height - 2.0f }, 0.2f, 8, 2.0f, Fade(WHITE, 0.75f));
    DrawRectangleRounded(Rectangle{ x + 8.0f, y + 22.0f, width - 16.0f, 24.0f }, 0.18f, 6, Color{ 9, 25, 46, 255 });
    DrawRectangle(static_cast<int>(x + 6.0f), static_cast<int>(y + 50.0f), static_cast<int>(width - 12.0f), 3, Fade(WHITE, 0.75f));
    DrawRectangle(static_cast<int>(x + 12.0f), static_cast<int>(y + 7.0f), 15, 6, Fade(RED, pulse));
    DrawRectangle(static_cast<int>(x + width - 27.0f), static_cast<int>(y + 7.0f), 15, 6, Fade(SKYBLUE, 1.0f - pulse * 0.4f));
    DrawRectangle(static_cast<int>(x + 4.0f), static_cast<int>(y + 4.0f), 11, 4, WHITE);
    DrawRectangle(static_cast<int>(x + width - 15.0f), static_cast<int>(y + 4.0f), 11, 4, WHITE);
    DrawRectangle(static_cast<int>(x + 5.0f), static_cast<int>(y + height - 8.0f), 12, 4, RED);
    DrawRectangle(static_cast<int>(x + width - 17.0f), static_cast<int>(y + height - 8.0f), 12, 4, RED);
}
