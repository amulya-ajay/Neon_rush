#include "entities/traffic/Sedan.h"

#include "raylib.h"

Sedan::Sedan(float startY, int startLane)
    : TrafficVehicle(startY, 185.0f, 100, startLane, 20, 30)
{
    SetSize(58.0f, 96.0f);
}

void Sedan::Draw() const
{
    DrawRectangle(static_cast<int>(x - 4.0f), static_cast<int>(y + 10.0f), 66, 74, Fade(RED, 0.12f));
    DrawRectangleRounded(Rectangle{ x, y, width, height }, 0.22f, 8, Color{ 194, 35, 78, 255 });
    DrawRectangleRoundedLinesEx(Rectangle{ x + 1.0f, y + 1.0f, width - 2.0f, height - 2.0f }, 0.2f, 8, 2.0f, Fade(PINK, 0.75f));
    DrawRectangleRounded(Rectangle{ x + 8.0f, y + 22.0f, width - 16.0f, 25.0f }, 0.18f, 6, Color{ 14, 31, 57, 255 });
    DrawRectangle(static_cast<int>(x + 11.0f), static_cast<int>(y + 25.0f), static_cast<int>(width - 22.0f), 2, Fade(SKYBLUE, 0.8f));
    DrawRectangle(static_cast<int>(x + 4.0f), static_cast<int>(y + 5.0f), 12, 4, Fade(WHITE, 0.9f));
    DrawRectangle(static_cast<int>(x + width - 16.0f), static_cast<int>(y + 5.0f), 12, 4, Fade(WHITE, 0.9f));
    DrawRectangle(static_cast<int>(x + 5.0f), static_cast<int>(y + height - 8.0f), 12, 4, RED);
    DrawRectangle(static_cast<int>(x + width - 17.0f), static_cast<int>(y + height - 8.0f), 12, 4, RED);
}
