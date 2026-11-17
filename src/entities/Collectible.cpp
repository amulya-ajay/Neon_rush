#include "entities/Collectible.h"

#include "raylib.h"

#include <algorithm>
#include <cmath>

Collectible::Collectible(float startX, float startY, float pickupRadius)
    : x(startX), y(startY), radius(pickupRadius), phase(0.0f), collected(false)
{
}

void Collectible::Update(float deltaTime)
{
    phase += deltaTime;
}

void Collectible::Scroll(float deltaTime, float scrollSpeed, float playerCenterX, float playerCenterY, bool magnetActive)
{
    const float dt = std::clamp(deltaTime, 0.0f, 0.05f);
    y += scrollSpeed * dt;
    if (magnetActive)
    {
        const float dx = playerCenterX - x;
        const float dy = playerCenterY - y;
        const float distanceSquared = dx * dx + dy * dy;
        if (distanceSquared < 220.0f * 220.0f)
        {
            const float pull = std::min(1.0f, dt * 4.8f);
            x += dx * pull;
            y += dy * pull;
        }
    }
    Update(dt);
}

Coin::Coin(float startX, float startY)
    : Collectible(startX, startY, 13.0f)
{
}

void Coin::Draw() const
{
    const float pulse = 0.86f + 0.14f * std::sin(phase * 5.0f);
    DrawCircleV(Vector2{ x, y }, radius * 1.8f, Fade(GOLD, 0.12f));
    DrawCircleV(Vector2{ x, y }, radius * pulse, Color{ 246, 190, 61, 255 });
    DrawCircleLines(static_cast<int>(x), static_cast<int>(y), radius * pulse, Fade(WHITE, 0.85f));
    DrawCircleV(Vector2{ x, y }, radius * 0.38f, Color{ 167, 93, 33, 255 });
    DrawText("$", static_cast<int>(x - 4.0f), static_cast<int>(y - 8.0f), 16, WHITE);
}

PowerUp::PowerUp(float startX, float startY, PickupKind type)
    : Collectible(startX, startY, 16.0f), kind(type)
{
}

void PowerUp::Draw() const
{
    const Color color = kind == PickupKind::Nitro ? SKYBLUE : (kind == PickupKind::Shield ? MAGENTA : LIME);
    const float pulse = 0.90f + 0.10f * std::sin(phase * 4.0f);
    DrawCircleV(Vector2{ x, y }, radius * 2.0f, Fade(color, 0.13f));
    DrawCircleV(Vector2{ x, y }, radius * pulse, Fade(color, 0.94f));
    DrawCircleLines(static_cast<int>(x), static_cast<int>(y), radius * pulse, Fade(WHITE, 0.85f));
    const char* label = kind == PickupKind::Nitro ? "N" : (kind == PickupKind::Shield ? "S" : "M");
    DrawText(label, static_cast<int>(x - 6.0f), static_cast<int>(y - 10.0f), 20, Color{ 8, 16, 31, 255 });
}
