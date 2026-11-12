#include "raylib.h"

int main()
{
    const int screenWidth = 1280;
    const int screenHeight = 720;

    InitWindow(screenWidth, screenHeight, "NEON RUSH");

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        BeginDrawing();

        ClearBackground(BLACK);

        DrawText("NEON RUSH", 480, 300, 50, SKYBLUE);
        DrawText("Highway Survival", 520, 365, 25, WHITE);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
