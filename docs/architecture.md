# Architecture

NEON RUSH uses a small object-oriented structure with `Game` coordinating state and delegating vehicle spawning, collectible spawning, score calculation, drawing, audio, and persistence to focused classes. `main.cpp` owns the Raylib window and calls `Game::Update` and `Game::Draw` once per frame.

## Runtime flow

1. `main` creates a resizable Raylib window and the `Game` object.
2. `Game` loads the save file and settings, initializes audio, and starts in the menu state.
3. While playing, `Game::Update` clamps frame time, updates the player and world, advances traffic and collectibles, updates effects and score, then resolves collisions and pickups.
4. `Game::Draw` renders the city, road, polymorphic vehicles and pickups, particles, and the current screen UI.
5. A collision can reduce player health and start a short invulnerability window. A shield absorbs the collision. At zero health, the run is saved and the game-over state is shown.
6. `Game::Shutdown` saves settings and progress and releases audio resources before the Raylib window closes.

## Core and entities

- `GameObject` defines the abstract `Update(float)` and `Draw() const` interface and a virtual destructor.
- `Vehicle` adds position, speed, size, health, bounds, and collision damage handling.
- `PlayerCar` implements lane switching, nitro charges, shield and magnet timers, invulnerability, and its own visual treatment.
- `TrafficVehicle` adds lane identity, difficulty speed scaling, damage and overtake values, and near-miss bookkeeping.
- `Sedan`, `SportsCar`, `Truck`, and `PoliceCar` override drawing and provide distinct dimensions, speeds, collision damage, and score values.
- `Collectible` is the common interface for `Coin` and `PowerUp`; `PowerUp` supports nitro, shield, and magnet types.

Traffic and pickups are owned by `std::vector<std::unique_ptr<...>>`. `TrafficManager` updates and removes vehicles; `CollectibleManager` updates, spawns, and removes pickups. The game iterates the collections through their base types, so derived `Update` and `Draw` behavior is dispatched at runtime.

## Systems

- `VehicleFactory` constructs each traffic class from a `TrafficType`, keeping type selection out of the game loop.
- `DifficultyManager` gradually increases traffic speed and reduces spawn intervals, with caps to keep the run bounded.
- `ScoreManager` tracks score, distance, coins, overtakes, and combo. Its `operator+=` centralizes score-point addition; overloaded `AddOvertake` calls support ordinary passes and near-miss passes.
- `ParticleSystem` uses a capped particle vector for sparks, pickup bursts, and nitro trails.
- `Road` calculates shared lane centers so player and traffic stay aligned when the window is resized.
- `CityBackground` draws a deterministic skyline, windows, and stars using Raylib primitives.
- `AudioManager` synthesizes short effects and an original ambient synth bed in memory. Audio-device failure leaves the rest of the game running silently.
- `SaveManager` reads and writes a small key/value file at `saves/neon_rush_save.txt`, relative to the current working directory. Missing files use defaults; malformed values fall back safely.

## State flow

`Game` owns the state enum: `Menu`, `Playing`, `Paused`, `GameOver`, and `Settings`. Gameplay simulation updates only in `Playing`; pause freezes player, road, traffic, pickups, score, and particles. Menu and settings input supports keyboard navigation and mouse selection. Fullscreen and audio settings persist.

## Collision and scoring decisions

Collision checks use Raylib axis-aligned rectangles derived from vehicle bounds. A traffic vehicle is removed on contact so it cannot apply damage on successive frames. An unshielded hit reduces health and gives temporary invulnerability. A near miss is recorded when expanded player bounds pass close to a vehicle without overlapping. Passed vehicles can then award overtake and near-miss points.

## Resource ownership

Raylib owns the window and audio device. `Game` owns its systems by value; managers own runtime entities with `unique_ptr`. Generated `Wave` buffers are released immediately after loading sounds. `AudioManager::Shutdown` releases sounds and the streaming music buffer before closing the audio device. No external image, font, or audio asset is required.
