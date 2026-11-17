#include "entities/traffic/Truck.h"

#include "raylib.h"

Truck::Truck(float startY, int startLane)
    : TrafficVehicle(startY, 135.0f, 200, startLane, 36, 45)
{
    SetSize(72.0f, 126.0f);
}

void Truck::Draw() const
{
    DrawRectangle(static_cast<int>(x - 5.0f), static_cast<int>(y + 12.0f), 82, 100, Fade(LIME, 0.10f));
    DrawRectangleRounded(Rectangle{ x, y + 30.0f, width, height - 30.0f }, 0.08f, 4, Color{ 21, 113, 91, 255 });
    DrawRectangleRoundedLinesEx(Rectangle{ x + 1.0f, y + 31.0f, width - 2.0f, height - 32.0f }, 0.08f, 4, 2.0f, Fade(LIME, 0.7f));
    DrawRectangle(static_cast<int>(x + 7.0f), static_cast<int>(y + 40.0f), static_cast<int>(width - 14.0f), 3, Fade(WHITE, 0.55f));
    DrawRectangleRounded(Rectangle{ x + 8.0f, y + 3.0f, width - 16.0f, 34.0f }, 0.18f, 6, Color{ 31, 52, 73, 255 });
    DrawRectangleRounded(Rectangle{ x + 13.0f, y + 8.0f, width - 26.0f, 19.0f }, 0.12f, 5, Color{ 11, 30, 54, 255 });
    DrawRectangle(static_cast<int>(x + 7.0f), static_cast<int>(y + 4.0f), 12, 4, WHITE);
    DrawRectangle(static_cast<int>(x + width - 19.0f), static_cast<int>(y + 4.0f), 12, 4, WHITE);
    DrawRectangle(static_cast<int>(x + 8.0f), static_cast<int>(y + height - 9.0f), 14, 5, RED);
    DrawRectangle(static_cast<int>(x + width - 22.0f), static_cast<int>(y + height - 9.0f), 14, 5, RED);
}
