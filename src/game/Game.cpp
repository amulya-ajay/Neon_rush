#include "game/Game.h"

#include "raylib.h"

#include <algorithm>
#include <cmath>
#include <string>

namespace
{
    constexpr float PLAYER_INVULNERABILITY = 1.25f;

    Rectangle ToRectangle(const VehicleBounds& bounds)
    {
        return Rectangle{ bounds.x, bounds.y, bounds.width, bounds.height };
    }

    void DrawCentered(const char* text, int centerX, int y, int fontSize, Color color)
    {
        DrawText(text, centerX - MeasureText(text, fontSize) / 2, y, fontSize, color);
    }

    void DrawPanel(Rectangle rectangle, Color tint = Color{ 8, 13, 31, 224 })
    {
        DrawRectangleRounded(rectangle, 0.12f, 8, tint);
        DrawRectangleRoundedLinesEx(rectangle, 0.12f, 8, 2.0f, Fade(SKYBLUE, 0.42f));
    }

    Rectangle ButtonBounds(float centerX, float centerY, float width = 300.0f, float height = 56.0f)
    {
        return Rectangle{ centerX - width * 0.5f, centerY - height * 0.5f, width, height };
    }

    bool MouseOver(Rectangle bounds)
    {
        return CheckCollisionPointRec(GetMousePosition(), bounds);
    }

    bool MouseClicked(Rectangle bounds)
    {
        return MouseOver(bounds) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
    }

    void DrawButton(Rectangle bounds, const char* label, bool selected)
    {
        const Color fill = selected ? Color{ 17, 61, 86, 242 } : Color{ 10, 18, 38, 226 };
        const Color border = selected ? SKYBLUE : Fade(MAGENTA, 0.48f);
        DrawRectangleRounded(bounds, 0.18f, 8, selected ? Fade(SKYBLUE, 0.10f) : Fade(MAGENTA, 0.07f));
        Rectangle inner{ bounds.x + 3.0f, bounds.y + 3.0f, bounds.width - 6.0f, bounds.height - 6.0f };
        DrawRectangleRounded(inner, 0.16f, 8, fill);
        DrawRectangleRoundedLinesEx(bounds, 0.18f, 8, selected ? 2.5f : 1.5f, border);
        DrawCentered(label, static_cast<int>(bounds.x + bounds.width * 0.5f), static_cast<int>(bounds.y + bounds.height * 0.5f - 10.0f), 21, selected ? RAYWHITE : Fade(RAYWHITE, 0.82f));
    }

    bool NearMiss(const VehicleBounds& first, const VehicleBounds& second)
    {
        constexpr float margin = 21.0f;
        Rectangle expanded{ first.x - margin, first.y - margin, first.width + margin * 2.0f, first.height + margin * 2.0f };
        return CheckCollisionRecs(expanded, ToRectangle(second)) && !CheckCollisionRecs(ToRectangle(first), ToRectangle(second));
    }
}

Game::Game()
    : saveData(),
    player(0.0f, 0.0f),
    state(State::Menu),
    menuSelection(0),
    pauseSelection(0),
    settingsSelection(0),
    elapsedSeconds(0.0f),
    shakeTimer(0.0f),
    shakeStrength(0.0f),
    quitRequested(false),
    shutdown(false)
{
    saveData = saveManager.Load();
    audio.Initialize();
    audio.SetVolumes(saveData.settings.soundVolume, saveData.settings.musicVolume);
    const auto laneCenters = road.GetLaneCenters();
    player.Reset(laneCenters[1] - player.GetWidth() * 0.5f, static_cast<float>(GetScreenHeight()) * 0.78f, laneCenters);
    if (saveData.settings.fullscreen != IsWindowFullscreen())
    {
        ToggleFullscreen();
    }
}

Game::~Game()
{
    Shutdown();
}

void Game::BeginRun()
{
    elapsedSeconds = 0.0f;
    shakeTimer = 0.0f;
    shakeStrength = 0.0f;
    score.Reset();
    traffic.Reset();
    collectibles.Reset();
    particles.Clear();
    const auto centers = road.GetLaneCenters();
    player.Reset(centers[1] - player.GetWidth() * 0.5f, static_cast<float>(GetScreenHeight()) * 0.78f, centers);
    state = State::Playing;
    audio.Play(SoundEffect::Start);
}

void Game::EndRun()
{
    saveData.highScore = std::max(saveData.highScore, score.GetScore());
    SaveProgress();
    state = State::GameOver;
    audio.Play(SoundEffect::GameOver);
}

void Game::SaveProgress()
{
    saveData.settings.fullscreen = IsWindowFullscreen();
    saveManager.Save(saveData);
}

void Game::ToggleFullscreen()
{
    ::ToggleFullscreen();
    saveData.settings.fullscreen = IsWindowFullscreen();
    SaveProgress();
}

void Game::UpdateMenu()
{
    const float centerX = static_cast<float>(GetScreenWidth()) * 0.5f;
    const float startY = static_cast<float>(GetScreenHeight()) * 0.55f;
    const char* labels[] = { "START RUN", "SETTINGS", "QUIT" };
    for (int i = 0; i < 3; ++i)
    {
        if (MouseOver(ButtonBounds(centerX, startY + i * 72.0f)))
        {
            menuSelection = i;
            if (MouseClicked(ButtonBounds(centerX, startY + i * 72.0f)))
            {
                audio.Play(SoundEffect::Ui);
                if (i == 0) BeginRun();
                if (i == 1) state = State::Settings;
                if (i == 2) quitRequested = true;
                return;
            }
        }
    }

    const int oldSelection = menuSelection;
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) menuSelection = (menuSelection + 2) % 3;
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) menuSelection = (menuSelection + 1) % 3;
    if (menuSelection != oldSelection) audio.Play(SoundEffect::Ui);
    if (IsKeyPressed(KEY_ENTER))
    {
        audio.Play(SoundEffect::Ui);
        if (menuSelection == 0) BeginRun();
        else if (menuSelection == 1) state = State::Settings;
        else quitRequested = true;
    }
}

void Game::UpdatePause()
{
    const float centerX = static_cast<float>(GetScreenWidth()) * 0.5f;
    const float startY = static_cast<float>(GetScreenHeight()) * 0.48f;
    const char* labels[] = { "RESUME", "RESTART RUN", "MAIN MENU" };
    for (int i = 0; i < 3; ++i)
    {
        const Rectangle bounds = ButtonBounds(centerX, startY + i * 72.0f);
        if (MouseOver(bounds))
        {
            pauseSelection = i;
            if (MouseClicked(bounds))
            {
                audio.Play(SoundEffect::Ui);
                if (i == 0) state = State::Playing;
                if (i == 1) BeginRun();
                if (i == 2) state = State::Menu;
                return;
            }
        }
    }

    const int oldSelection = pauseSelection;
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) pauseSelection = (pauseSelection + 2) % 3;
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) pauseSelection = (pauseSelection + 1) % 3;
    if (pauseSelection != oldSelection) audio.Play(SoundEffect::Ui);
    if (IsKeyPressed(KEY_P) || IsKeyPressed(KEY_ESCAPE))
    {
        state = State::Playing;
        audio.Play(SoundEffect::Ui);
        return;
    }
    if (IsKeyPressed(KEY_ENTER))
    {
        audio.Play(SoundEffect::Ui);
        if (pauseSelection == 0) state = State::Playing;
        else if (pauseSelection == 1) BeginRun();
        else state = State::Menu;
    }
}

void Game::UpdateSettings()
{
    const int oldSelection = settingsSelection;
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) settingsSelection = (settingsSelection + 3) % 4;
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) settingsSelection = (settingsSelection + 1) % 4;
    if (settingsSelection != oldSelection) audio.Play(SoundEffect::Ui);

    bool changed = false;
    if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_RIGHT))
    {
        const float step = 0.05f;
        const float direction = IsKeyPressed(KEY_RIGHT) ? 1.0f : -1.0f;
        if (settingsSelection == 0) saveData.settings.soundVolume = std::clamp(saveData.settings.soundVolume + step * direction, 0.0f, 1.0f);
        else if (settingsSelection == 1) saveData.settings.musicVolume = std::clamp(saveData.settings.musicVolume + step * direction, 0.0f, 1.0f);
        else if (settingsSelection == 2) ToggleFullscreen();
        changed = true;
    }
    if (IsKeyPressed(KEY_F))
    {
        ToggleFullscreen();
        changed = true;
    }

    const float centerX = static_cast<float>(GetScreenWidth()) * 0.5f;
    const float startY = static_cast<float>(GetScreenHeight()) * 0.37f;
    const float rowGap = 88.0f;
    Rectangle soundBar{ centerX - 130.0f, startY - 6.0f, 260.0f, 22.0f };
    Rectangle musicBar{ centerX - 130.0f, startY + rowGap - 6.0f, 260.0f, 22.0f };
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && MouseOver(soundBar))
    {
        saveData.settings.soundVolume = std::clamp((GetMousePosition().x - soundBar.x) / soundBar.width, 0.0f, 1.0f);
        settingsSelection = 0;
        changed = true;
    }
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && MouseOver(musicBar))
    {
        saveData.settings.musicVolume = std::clamp((GetMousePosition().x - musicBar.x) / musicBar.width, 0.0f, 1.0f);
        settingsSelection = 1;
        changed = true;
    }
    Rectangle fullscreenButton = ButtonBounds(centerX, startY + rowGap * 2.0f, 300.0f, 52.0f);
    Rectangle backButton = ButtonBounds(centerX, startY + rowGap * 3.0f, 300.0f, 52.0f);
    if (MouseClicked(fullscreenButton))
    {
        settingsSelection = 2;
        ToggleFullscreen();
        audio.Play(SoundEffect::Ui);
    }
    if (MouseClicked(backButton))
    {
        state = State::Menu;
        SaveProgress();
        audio.Play(SoundEffect::Ui);
        return;
    }

    if (IsKeyPressed(KEY_ENTER))
    {
        if (settingsSelection == 2) ToggleFullscreen();
        if (settingsSelection == 3) state = State::Menu;
        changed = true;
    }
    if (changed)
    {
        audio.SetVolumes(saveData.settings.soundVolume, saveData.settings.musicVolume);
        SaveProgress();
    }
    if (IsKeyPressed(KEY_ESCAPE))
    {
        state = State::Menu;
        SaveProgress();
    }
}

void Game::UpdateGameOver()
{
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_R))
    {
        BeginRun();
        return;
    }
    if (IsKeyPressed(KEY_M) || IsKeyPressed(KEY_ESCAPE))
    {
        state = State::Menu;
        return;
    }

    const float centerX = static_cast<float>(GetScreenWidth()) * 0.5f;
    const float centerY = static_cast<float>(GetScreenHeight()) * 0.73f;
    if (MouseClicked(ButtonBounds(centerX - 165.0f, centerY, 270.0f, 54.0f)))
    {
        BeginRun();
    }
    else if (MouseClicked(ButtonBounds(centerX + 165.0f, centerY, 270.0f, 54.0f)))
    {
        state = State::Menu;
    }
}

void Game::Update(float deltaTime)
{
    const float dt = std::clamp(deltaTime, 0.0f, 0.05f);
    city.Update(dt);
    audio.Update();
    if (shakeTimer > 0.0f)
    {
        shakeTimer = std::max(0.0f, shakeTimer - dt);
    }

    if (state == State::Menu || state == State::Settings || state == State::GameOver)
    {
        road.Update(dt, 92.0f);
    }
    if (state == State::Menu)
    {
        UpdateMenu();
        return;
    }
    if (state == State::Settings)
    {
        UpdateSettings();
        return;
    }
    if (state == State::Paused)
    {
        UpdatePause();
        return;
    }
    if (state == State::GameOver)
    {
        UpdateGameOver();
        return;
    }

    if (IsKeyPressed(KEY_P) || IsKeyPressed(KEY_ESCAPE))
    {
        state = State::Paused;
        audio.Play(SoundEffect::Ui);
        return;
    }

    elapsedSeconds += dt;
    const auto laneCenters = road.GetLaneCenters();
    player.SetLaneCenters(laneCenters);
    player.SetY(static_cast<float>(GetScreenHeight()) * 0.78f);
    const bool wasNitroActive = player.IsNitroActive();
    player.Update(dt);
    if (!wasNitroActive && player.IsNitroActive())
    {
        audio.Play(SoundEffect::Nitro);
    }

    const float worldMultiplier = difficulty.GetTrafficMultiplier(elapsedSeconds) * player.GetSpeedMultiplier();
    const float roadSpeed = 205.0f * player.GetSpeedMultiplier();
    road.Update(dt, roadSpeed);
    traffic.Update(dt, static_cast<float>(GetScreenHeight()), elapsedSeconds, worldMultiplier, difficulty.GetSpawnInterval(elapsedSeconds), laneCenters, player.GetY());
    collectibles.Update(dt, static_cast<float>(GetScreenHeight()), 178.0f * player.GetSpeedMultiplier(), laneCenters,
        player.GetX() + player.GetWidth() * 0.5f, player.GetY() + player.GetHeight() * 0.5f, player.IsMagnetActive(), elapsedSeconds);
    particles.Update(dt);
    if (player.IsNitroActive())
    {
        particles.Trail(player.GetX() + player.GetWidth() * 0.5f, player.GetY() + player.GetHeight() + 3.0f, SKYBLUE);
    }
    score.Update(dt, player.GetSpeedMultiplier());
    HandleCollisions();
    if (state != State::Playing)
    {
        return;
    }
    CollectPickups();
}

void Game::HandleCollisions()
{
    auto& vehicles = traffic.GetVehicles();
    for (std::size_t i = 0; i < vehicles.size();)
    {
        TrafficVehicle& vehicle = *vehicles[i];
        const VehicleBounds playerBounds = player.GetBounds();
        const VehicleBounds vehicleBounds = vehicle.GetBounds();
        const bool collision = CheckCollisionRecs(ToRectangle(playerBounds), ToRectangle(vehicleBounds));
        if (collision)
        {
            if (player.IsShieldActive())
            {
                particles.Burst(vehicle.GetX() + vehicle.GetWidth() * 0.5f, vehicle.GetY() + vehicle.GetHeight() * 0.5f, SKYBLUE, 22, 240.0f);
                audio.Play(SoundEffect::PowerUp);
            }
            else if (!player.IsInvulnerable())
            {
                player.HandleCollision(vehicle.GetCollisionDamage());
                player.StartInvulnerability(PLAYER_INVULNERABILITY);
                shakeTimer = 0.28f;
                shakeStrength = 8.0f;
                particles.Burst(player.GetX() + player.GetWidth() * 0.5f, player.GetY() + player.GetHeight() * 0.5f, ORANGE, 26, 260.0f);
                audio.Play(SoundEffect::Collision);
            }
            else
            {
                particles.Burst(vehicle.GetX() + vehicle.GetWidth() * 0.5f, vehicle.GetY() + vehicle.GetHeight() * 0.5f, SKYBLUE, 12, 150.0f);
            }
            traffic.RemoveVehicle(i);
            if (player.GetHealth() <= 0)
            {
                EndRun();
                return;
            }
            continue;
        }

        if (NearMiss(playerBounds, vehicleBounds))
        {
            vehicle.MarkNearMiss();
        }
        if (vehicle.TryMarkOvertaken(player.GetY()))
        {
            score.AddOvertake(vehicle.GetScoreValue(), vehicle.HadNearMiss());
            if (vehicle.HadNearMiss())
            {
                audio.Play(SoundEffect::PowerUp);
                particles.Burst(player.GetX() + player.GetWidth() * 0.5f, player.GetY() + 18.0f, GOLD, 8, 95.0f);
            }
        }
        ++i;
    }
}

void Game::CollectPickups()
{
    auto& pickups = collectibles.GetPickups();
    for (std::size_t i = 0; i < pickups.size();)
    {
        Collectible& pickup = *pickups[i];
        if (!CheckCollisionRecs(ToRectangle(player.GetBounds()), ToRectangle(pickup.GetBounds())))
        {
            ++i;
            continue;
        }

        const float px = pickup.GetX();
        const float py = pickup.GetY();
        switch (pickup.GetKind())
        {
        case PickupKind::Coin:
            score.AddCoin();
            ++saveData.totalCoins;
            audio.Play(SoundEffect::Coin);
            particles.Burst(px, py, GOLD, 13, 150.0f);
            break;
        case PickupKind::Nitro:
            player.AddNitroCharge();
            audio.Play(SoundEffect::PowerUp);
            particles.Burst(px, py, SKYBLUE, 18, 180.0f);
            break;
        case PickupKind::Shield:
            player.ActivateShield();
            audio.Play(SoundEffect::PowerUp);
            particles.Burst(px, py, MAGENTA, 18, 180.0f);
            break;
        case PickupKind::Magnet:
            player.ActivateMagnet();
            audio.Play(SoundEffect::PowerUp);
            particles.Burst(px, py, LIME, 18, 180.0f);
            break;
        }
        collectibles.RemovePickup(i);
    }
}

void Game::DrawBackdrop(bool titleMode) const
{
    city.Draw(titleMode);
    road.Draw();
}

void Game::DrawGameplay(bool includeHud) const
{
    const float screenWidth = static_cast<float>(GetScreenWidth());
    const float screenHeight = static_cast<float>(GetScreenHeight());
    const float shake = shakeTimer > 0.0f ? shakeStrength * (shakeTimer / 0.28f) : 0.0f;
    Camera2D camera{};
    camera.offset = Vector2{ screenWidth * 0.5f + std::sin(static_cast<float>(GetTime() * 69.0)) * shake, screenHeight * 0.5f + std::cos(static_cast<float>(GetTime() * 58.0)) * shake };
    camera.target = Vector2{ screenWidth * 0.5f, screenHeight * 0.5f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    BeginMode2D(camera);
    DrawBackdrop(false);
    if (player.IsNitroActive())
    {
        const float roadLeft = road.GetLeft();
        const float roadWidth = road.GetWidth();
        const float streakPhase = static_cast<float>(GetTime() * 560.0);
        for (int i = 0; i < 18; ++i)
        {
            const float x = roadLeft + 16.0f + static_cast<float>((i * 83) % static_cast<int>(std::max(1.0f, roadWidth - 32.0f)));
            const float y = std::fmod(static_cast<float>(i * 97) + streakPhase, screenHeight);
            const float length = 20.0f + static_cast<float>((i * 11) % 28);
            DrawLineEx(Vector2{ x, y }, Vector2{ x, y + length }, i % 3 == 0 ? 2.0f : 1.0f, Fade(SKYBLUE, 0.32f));
        }
    }
    for (const auto& vehicle : traffic.GetVehicles()) vehicle->Draw();
    for (const auto& pickup : collectibles.GetPickups()) pickup->Draw();
    player.Draw();
    particles.Draw();
    EndMode2D();

    if (includeHud) DrawHud();
}

void Game::DrawHud() const
{
    const float width = static_cast<float>(GetScreenWidth());
    const float x = 22.0f;
    const float y = 18.0f;
    const float panelWidth = width - 44.0f;
    DrawPanel(Rectangle{ x, y, panelWidth, 105.0f }, Color{ 5, 10, 25, 220 });

    DrawText("SCORE", 42, 31, 14, Fade(SKYBLUE, 0.88f));
    DrawText(TextFormat("%06i", score.GetScore()), 42, 48, 27, RAYWHITE);
    DrawText(TextFormat("BEST %06i", saveData.highScore), 42, 81, 13, Fade(RAYWHITE, 0.67f));

    DrawText("DISTANCE", 222, 31, 14, Fade(SKYBLUE, 0.88f));
    DrawText(TextFormat("%.2f km", score.GetDistanceKm()), 222, 51, 23, RAYWHITE);
    DrawText(TextFormat("OVERTAKES %03i", score.GetOvertakes()), 222, 82, 13, Fade(RAYWHITE, 0.68f));

    DrawText("COINS", 420, 31, 14, Fade(GOLD, 0.92f));
    DrawText(TextFormat("%03i", score.GetCoins()), 420, 51, 23, RAYWHITE);
    DrawText(TextFormat("TOTAL %03i", saveData.totalCoins), 420, 82, 13, Fade(RAYWHITE, 0.68f));

    DrawText("COMBO", 565, 31, 14, Fade(MAGENTA, 0.92f));
    DrawText(TextFormat("x%i", score.GetCombo()), 565, 51, 23, RAYWHITE);
    DrawText("DODGE + COLLECT", 565, 82, 13, Fade(RAYWHITE, 0.68f));

    const float right = width - 50.0f;
    DrawText("HULL", static_cast<int>(right - 190.0f), 30, 14, Fade(SKYBLUE, 0.9f));
    Rectangle bar{ right - 190.0f, 52.0f, 180.0f, 15.0f };
    DrawRectangleRounded(bar, 0.45f, 6, Color{ 37, 42, 59, 255 });
    Rectangle fill = bar;
    fill.width *= std::clamp(static_cast<float>(player.GetHealth()) / 100.0f, 0.0f, 1.0f);
    DrawRectangleRounded(fill, 0.45f, 6, player.GetHealth() > 35 ? SKYBLUE : RED);
    DrawText(TextFormat("%i%%", player.GetHealth()), static_cast<int>(right - 190.0f), 75, 13, Fade(RAYWHITE, 0.72f));

    const char* nitroText = player.IsNitroActive()
        ? TextFormat("NITRO %.1fs", player.GetNitroTimeRemaining())
        : TextFormat("SPACE  NITRO x%i", player.GetNitroCharges());
    DrawText(nitroText, static_cast<int>(right - 95.0f), 75, 13, player.IsNitroActive() ? SKYBLUE : Fade(RAYWHITE, 0.78f));
    if (player.IsShieldActive()) DrawText(TextFormat("SHIELD %.1fs", player.GetShieldTimeRemaining()), static_cast<int>(right - 95.0f), 32, 14, MAGENTA);
    else if (player.IsMagnetActive()) DrawText(TextFormat("MAGNET %.1fs", player.GetMagnetTimeRemaining()), static_cast<int>(right - 95.0f), 32, 14, LIME);
}

void Game::DrawMainMenu() const
{
    const int centerX = GetScreenWidth() / 2;
    const float height = static_cast<float>(GetScreenHeight());
    DrawCentered("NEON RUSH", centerX, static_cast<int>(height * 0.16f), 68, Fade(MAGENTA, 0.48f));
    DrawCentered("NEON RUSH", centerX - 2, static_cast<int>(height * 0.16f) - 2, 64, SKYBLUE);
    DrawCentered("OUTRUN THE TRAFFIC", centerX, static_cast<int>(height * 0.16f + 61.0f), 20, RAYWHITE);
    DrawCentered("A SYNTHWAVE HIGHWAY SURVIVAL RUN", centerX, static_cast<int>(height * 0.16f + 91.0f), 13, Fade(RAYWHITE, 0.62f));

    const float startY = height * 0.55f;
    const char* labels[] = { "START RUN", "SETTINGS", "QUIT" };
    for (int i = 0; i < 3; ++i)
    {
        const Rectangle bounds = ButtonBounds(static_cast<float>(centerX), startY + i * 72.0f);
        DrawButton(bounds, labels[i], i == menuSelection || MouseOver(bounds));
    }

    DrawPanel(Rectangle{ 24.0f, height - 73.0f, 410.0f, 48.0f }, Color{ 5, 10, 25, 190 });
    DrawText("A / LEFT       D / RIGHT       SPACE  NITRO", 42, static_cast<int>(height - 58.0f), 14, Fade(RAYWHITE, 0.82f));
    DrawText(TextFormat("BEST RUN  %06i     TOTAL COINS  %03i", saveData.highScore, saveData.totalCoins), centerX - 180, static_cast<int>(height - 42.0f), 14, Fade(SKYBLUE, 0.88f));
}

void Game::DrawPauseMenu() const
{
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.56f));
    DrawPanel(Rectangle{ GetScreenWidth() * 0.5f - 230.0f, GetScreenHeight() * 0.5f - 206.0f, 460.0f, 412.0f });
    DrawCentered("PAUSED", GetScreenWidth() / 2, static_cast<int>(GetScreenHeight() * 0.5f - 164.0f), 38, SKYBLUE);
    const char* labels[] = { "RESUME", "RESTART RUN", "MAIN MENU" };
    const float startY = static_cast<float>(GetScreenHeight()) * 0.48f;
    for (int i = 0; i < 3; ++i)
    {
        const Rectangle bounds = ButtonBounds(GetScreenWidth() * 0.5f, startY + i * 72.0f);
        DrawButton(bounds, labels[i], i == pauseSelection || MouseOver(bounds));
    }
    DrawCentered("P / ESC TO RESUME", GetScreenWidth() / 2, static_cast<int>(GetScreenHeight() * 0.5f + 160.0f), 14, Fade(RAYWHITE, 0.62f));
}

void Game::DrawGameOver() const
{
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(Color{ 4, 5, 17, 255 }, 0.72f));
    const float centerX = static_cast<float>(GetScreenWidth()) * 0.5f;
    const float centerY = static_cast<float>(GetScreenHeight()) * 0.5f;
    DrawPanel(Rectangle{ centerX - 320.0f, centerY - 230.0f, 640.0f, 460.0f }, Color{ 7, 10, 28, 242 });
    DrawCentered("RUN OVER", static_cast<int>(centerX), static_cast<int>(centerY - 196.0f), 40, MAGENTA);
    DrawCentered(TextFormat("%06i", score.GetScore()), static_cast<int>(centerX), static_cast<int>(centerY - 135.0f), 52, RAYWHITE);
    DrawCentered("FINAL SCORE", static_cast<int>(centerX), static_cast<int>(centerY - 91.0f), 14, Fade(SKYBLUE, 0.9f));
    DrawText(TextFormat("DISTANCE     %.2f km", score.GetDistanceKm()), static_cast<int>(centerX - 210.0f), static_cast<int>(centerY - 42.0f), 18, RAYWHITE);
    DrawText(TextFormat("COINS        %i", score.GetCoins()), static_cast<int>(centerX - 210.0f), static_cast<int>(centerY - 9.0f), 18, GOLD);
    DrawText(TextFormat("OVERTAKES    %i", score.GetOvertakes()), static_cast<int>(centerX - 210.0f), static_cast<int>(centerY + 24.0f), 18, RAYWHITE);
    DrawText(TextFormat("BEST         %06i", saveData.highScore), static_cast<int>(centerX - 210.0f), static_cast<int>(centerY + 57.0f), 18, SKYBLUE);

    const Rectangle restart = ButtonBounds(centerX - 150.0f, centerY + 145.0f, 270.0f, 54.0f);
    const Rectangle mainMenu = ButtonBounds(centerX + 150.0f, centerY + 145.0f, 270.0f, 54.0f);
    DrawButton(restart, "RESTART", MouseOver(restart));
    DrawButton(mainMenu, "MAIN MENU", MouseOver(mainMenu));
    DrawCentered("ENTER / R  RESTART      M  MENU", static_cast<int>(centerX), static_cast<int>(centerY + 192.0f), 13, Fade(RAYWHITE, 0.68f));
}

void Game::DrawSettings() const
{
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(Color{ 4, 7, 22, 255 }, 0.76f));
    const float centerX = static_cast<float>(GetScreenWidth()) * 0.5f;
    const float startY = static_cast<float>(GetScreenHeight()) * 0.35f;
    DrawPanel(Rectangle{ centerX - 300.0f, startY - 92.0f, 600.0f, 430.0f });
    DrawCentered("SETTINGS", static_cast<int>(centerX), static_cast<int>(startY - 65.0f), 34, SKYBLUE);

    const char* labels[] = { "SOUND EFFECTS", "MUSIC", "DISPLAY MODE", "BACK" };
    const float rowGap = 88.0f;
    for (int i = 0; i < 4; ++i)
    {
        const float rowY = startY + i * rowGap;
        DrawText(labels[i], static_cast<int>(centerX - 242.0f), static_cast<int>(rowY - 12.0f), 16, i == settingsSelection ? SKYBLUE : RAYWHITE);
        if (i == 0 || i == 1)
        {
            const float value = i == 0 ? saveData.settings.soundVolume : saveData.settings.musicVolume;
            Rectangle track{ centerX - 15.0f, rowY - 5.0f, 230.0f, 16.0f };
            DrawRectangleRounded(track, 0.4f, 6, Color{ 36, 43, 62, 255 });
            Rectangle level = track;
            level.width *= value;
            DrawRectangleRounded(level, 0.4f, 6, i == 0 ? SKYBLUE : MAGENTA);
            DrawText(TextFormat("%i%%", static_cast<int>(value * 100.0f)), static_cast<int>(centerX + 229.0f), static_cast<int>(rowY - 10.0f), 16, RAYWHITE);
        }
        else if (i == 2)
        {
            const char* mode = IsWindowFullscreen() ? "FULLSCREEN" : "WINDOWED";
            DrawText(mode, static_cast<int>(centerX + 75.0f), static_cast<int>(rowY - 12.0f), 16, RAYWHITE);
        }
        else
        {
            DrawText("ENTER TO RETURN", static_cast<int>(centerX + 75.0f), static_cast<int>(rowY - 12.0f), 16, Fade(RAYWHITE, 0.76f));
        }
    }
    DrawCentered("UP / DOWN SELECT     LEFT / RIGHT ADJUST     F TOGGLE DISPLAY", GetScreenWidth() / 2, GetScreenHeight() - 66, 13, Fade(RAYWHITE, 0.62f));
}

void Game::Draw() const
{
    if (state == State::Playing || state == State::Paused || state == State::GameOver)
    {
        DrawGameplay(true);
        if (state == State::Paused) DrawPauseMenu();
        if (state == State::GameOver) DrawGameOver();
        return;
    }

    DrawBackdrop(true);
    if (state == State::Menu) DrawMainMenu();
    else if (state == State::Settings) DrawSettings();
}

void Game::Shutdown()
{
    if (shutdown)
    {
        return;
    }
    SaveProgress();
    audio.Shutdown();
    shutdown = true;
}
