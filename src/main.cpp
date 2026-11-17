#include "game/Game.h"

#include "raylib.h"

int main()
{
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    InitWindow(1280, 720, "NEON RUSH - Outrun the traffic");
    SetWindowMinSize(960, 540);
    SetTargetFPS(60);

    {
        Game game;
        while (!WindowShouldClose() && !game.ShouldQuit())
        {
            game.Update(GetFrameTime());

            BeginDrawing();
            ClearBackground(Color{ 4, 7, 22, 255 });
            game.Draw();
            EndDrawing();
        }
        game.Shutdown();
    }

    CloseWindow();
    return 0;
}
