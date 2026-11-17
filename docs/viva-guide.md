# NEON RUSH — Viva Study Guide

This guide follows the code in this repository. Learn the flow and explain it in your own words; do not memorize answers that you cannot connect to the implementation.

## 1. A one-minute project explanation

> NEON RUSH is a C++17 and Raylib top-down arcade highway survival game. The player switches between four lanes, avoids traffic, collects coins and power-ups, and tries to survive for a higher score. The project uses an abstract game-object interface, a vehicle inheritance hierarchy, runtime polymorphism for traffic, and managers for spawning, scoring, audio, saves, and particles. The world art and sound are generated at runtime, so the game does not depend on external assets.

## 2. Start at `main.cpp`

`main` configures a resizable Raylib window, creates a `Game`, then repeats this frame loop until the user closes the window or chooses Quit:

1. `game.Update(GetFrameTime())` advances input and game logic.
2. `BeginDrawing()` starts a frame and `ClearBackground()` clears the previous image.
3. `game.Draw()` draws the current screen.
4. `EndDrawing()` presents the frame.

When the loop ends, `Game::Shutdown()` saves progress and releases audio resources, then `CloseWindow()` releases the Raylib window. `GetFrameTime()` makes movement frame-rate independent: movement uses speed in pixels per second multiplied by elapsed seconds. `Game::Update` caps a long frame at 0.05 seconds to avoid a large jump after a pause or stall.

## 3. Class structure and OOP concepts

### Abstraction and inheritance

- `GameObject` is an abstract interface. Its pure virtual `Update(float)` and `Draw() const` methods describe what updateable, drawable objects do.
- `Vehicle` extends `GameObject` with shared position, speed, dimensions, health, collision bounds, and collision damage handling.
- `PlayerCar` and `TrafficVehicle` extend `Vehicle` with different behavior.
- `Sedan`, `SportsCar`, `Truck`, and `PoliceCar` extend `TrafficVehicle`. They have different sizes, speeds, damage values, score values, and drawing.
- `Collectible` is an abstract base for `Coin` and `PowerUp`.

### Runtime polymorphism

`TrafficManager` owns traffic as `std::vector<std::unique_ptr<TrafficVehicle>>`. The vector can contain any derived traffic type. The game calls `Update()` and `Draw()` through the base class; C++ dispatches to the matching derived implementation at runtime.

`VehicleFactory::Create` chooses the concrete type and returns a `std::unique_ptr<TrafficVehicle>`. This keeps object creation out of the main frame loop.

### Encapsulation, composition, and resource ownership

- Entity fields are private or protected. The rest of the game uses methods such as `ChangeLane`, `ActivateShield`, and `GetBounds` instead of editing every field directly.
- `Game` is the composition root: it owns the player, world, managers, score, effects, and audio system, and coordinates them by game state.
- `std::unique_ptr` makes ownership explicit and destroys traffic and pickups automatically when they are removed.
- `GameObject` has a virtual destructor, so deleting a derived object through a base pointer is safe.
- `AudioManager` releases the generated sounds, music stream, and audio device in `Shutdown()`.

### Compile-time polymorphism

`ScoreManager::operator+=(int)` centralizes the addition of score points. `AddOvertake(int)` and `AddOvertake(int, bool)` are overloaded: one records an ordinary pass, while the other can also award a near-miss bonus.

## 4. What happens during a run

`Game` has five states: `Menu`, `Playing`, `Paused`, `GameOver`, and `Settings`.

While Playing, each update roughly does this:

1. Read player input and smoothly move toward the selected lane center.
2. Update the difficulty, road scroll, traffic spawns, and pickups.
3. Update particles, nitro effects, distance, survival score, and combo timers.
4. Check vehicle collisions and pickup collection.
5. If health reaches zero, save the run and move to Game Over.

Paused state does not update the player, road, traffic, pickups, score, or particles. The skyline can still animate behind the pause panel.

## 5. Main gameplay systems

- **Road and city:** `Road` calculates four shared lane centers and draws animated markings and neon boundaries. `CityBackground` draws a deterministic skyline, windows, and stars with Raylib primitives.
- **Traffic:** `TrafficManager` picks available lanes, chooses a vehicle type, spawns through `VehicleFactory`, updates speed, and removes vehicles that pass the play area. Spawn checks keep vehicles from appearing too close together in the same lane.
- **Collision:** `Game::HandleCollisions` compares Raylib axis-aligned rectangles (`CheckCollisionRecs`). Each colliding vehicle is removed, so it cannot damage the player every frame. A non-shielded hit lowers health and starts temporary invulnerability. A shield absorbs the hit.
- **Near misses and score:** close passes can be marked as near misses. `ScoreManager` tracks score, distance, coins, overtakes, and combo. Survival adds points over time; coins, overtakes, and near misses add skill bonuses.
- **Pickups:** `CollectibleManager` creates coin trails and timed power-ups. A coin adds score and currency; nitro adds a charge, while shield and magnet activate immediately.
- **Difficulty:** `DifficultyManager` gradually raises traffic speed and shortens the spawn interval, with caps.
- **Effects:** `ParticleSystem` has a fixed capacity for bursts and trails. Collision effects add sparks and screen shake; nitro adds exhaust and speed streaks.
- **Audio:** `AudioManager` synthesizes short sound effects and a simple ambient synth stream in memory. If the audio device is unavailable, audio calls safely do nothing and gameplay continues.
- **Save data:** `SaveManager` stores high score, total coins, sound/music volume, and fullscreen preference in `saves/neon_rush_save.txt`, relative to the current working directory. Missing or malformed saves fall back to defaults.

## 6. Controls and where to find them

- **A / Left Arrow:** move left one lane.
- **D / Right Arrow:** move right one lane.
- **Space:** use one stored nitro charge.
- **P / Escape:** pause or resume.
- **Up / Down and Enter:** navigate and confirm menus.
- **F:** toggle fullscreen in Settings.
- **Mouse:** select menu items and adjust volume sliders.

For implementation details, trace `PlayerCar::Update`, `Game::UpdateSettings`, and `Game::UpdatePause`.

## 7. Likely viva questions

### Why is `GameObject` abstract?

It defines the common update/draw contract without choosing a specific visual or behavior. Concrete objects provide their own implementations.

### Why use `unique_ptr` for traffic?

The traffic manager owns each vehicle exclusively. `unique_ptr` destroys it automatically when it leaves the vector, and still supports virtual calls through the base type.

### How does runtime polymorphism happen?

Different derived vehicles are stored as `unique_ptr<TrafficVehicle>`. Calls to virtual `Update()` and `Draw()` use the derived class implementation.

### Why use a factory?

The factory centralizes the mapping from `TrafficType` to a concrete class. The traffic system can request a type without containing each vehicle constructor branch.

### How do you avoid frame-rate-dependent movement?

Use `GetFrameTime()` and multiply speeds by `deltaTime`. The same speed then covers the same distance per second at different frame rates.

### How do you prevent repeated collision damage?

On contact, the vehicle is removed. If the player was not shielded, a short invulnerability timer protects against another hit immediately afterward.

### What data is saved?

High score, total coins, sound volume, music volume, and fullscreen preference. Current-run score and distance reset for a new run.

### What tests exist?

`tests/logic_tests.cpp` checks lane limits, health clamping, vehicle factory stats, traffic spawning and lane placement, score/combo behavior, and save/load behavior. CTest runs this as `neon_rush_logic`. It does not automate the visual menu or every gameplay interaction.

### What would you improve next?

Add focused UI/integration tests for menu transitions and fullscreen changes, then capture real menu/game-over screenshots for the README. Keep the existing Release build and CTest results separate from UI checks when reporting what was tested.

## 8. Build and test commands

Run these from a Visual Studio Developer PowerShell with CMake available:

```powershell
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
```

The last verified local result was a successful Release build and `1/1` passing CTest check.

## 9. A good way to study

1. Read `src/main.cpp` and explain the frame loop aloud.
2. Draw the inheritance tree from `GameObject` to each vehicle class without looking.
3. Trace one traffic car from `TrafficManager` → `VehicleFactory` → derived `Draw()` call.
4. Trace one collision from `CheckCollisionRecs` through health, invulnerability, particles, and Game Over.
5. Explain the save file and one test from `logic_tests.cpp`.

If you can explain those five traces in your own words, you can defend the main architecture and gameplay loop.
