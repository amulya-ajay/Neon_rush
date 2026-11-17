#include "entities/traffic/SportsCar.h"

#include "raylib.h"

SportsCar::SportsCar(float startY, int startLane)
    : TrafficVehicle(startY, 255.0f, 80, startLane, 24, 55)
{
    SetSize(52.0f, 88.0f);
}

void SportsCar::Draw() const
{
    DrawRectangle(static_cast<int>(x - 6.0f), static_cast<int>(y + 14.0f), 64, 65, Fade(GOLD, 0.12f));
    DrawTriangle(Vector2{ x + width * 0.5f, y }, Vector2{ x + width - 2.0f, y + 28.0f }, Vector2{ x + 2.0f, y + 28.0f }, Color{ 232, 176, 54, 255 });
    DrawRectangleRounded(Rectangle{ x + 2.0f, y + 18.0f, width - 4.0f, height - 22.0f }, 0.28f, 8, Color{ 236, 173, 44, 255 });
    DrawRectangleRoundedLinesEx(Rectangle{ x + 3.0f, y + 19.0f, width - 6.0f, height - 24.0f }, 0.26f, 8, 2.0f, Fade(WHITE, 0.65f));
    DrawRectangleRounded(Rectangle{ x + 8.0f, y + 26.0f, width - 16.0f, 21.0f }, 0.24f, 6, Color{ 11, 29, 55, 255 });
    DrawRectangle(static_cast<int>(x + 6.0f), static_cast<int>(y + height - 12.0f), static_cast<int>(width - 12.0f), 3, Fade(ORANGE, 0.9f));
    DrawRectangle(static_cast<int>(x + 5.0f), static_cast<int>(y + 8.0f), 9, 4, WHITE);
    DrawRectangle(static_cast<int>(x + width - 14.0f), static_cast<int>(y + 8.0f), 9, 4, WHITE);
    DrawRectangle(static_cast<int>(x + 4.0f), static_cast<int>(y + height - 8.0f), 10, 4, RED);
    DrawRectangle(static_cast<int>(x + width - 14.0f), static_cast<int>(y + height - 8.0f), 10, 4, RED);
}
