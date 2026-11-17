# NEON RUSH

**Outrun the traffic. Survive the highway.**

NEON RUSH is a C++17 arcade highway survival game built with Raylib. Drive through a procedurally drawn synthwave city, change lanes to avoid traffic, collect coins and power-ups, and build a score through overtakes and near misses.

## Features

- Four lane highway with animated markings, neon boundaries, roadside lights, and a procedural night skyline.
- Smooth lane changes and four traffic types: sedan, sports car, truck, and police car.
- Traffic spawning with lane spacing, speed scaling, cleanup, and polymorphic vehicle updates and drawing.
- Health, temporary collision invulnerability, shield protection, and a game-over/restart flow.
- Nitro charges, shield, coin magnet, coin trails, particles, and collision screen shake.
- Survival score, distance, coins, overtakes, near-miss rewards, and combo multiplier.
- Main menu, pause menu, settings, and game-over screen with keyboard and mouse interaction.
- Synthesized original sound effects and ambient music; the game does not require external audio or image assets.
- Persistent high score, coin total, volume settings, and fullscreen preference under `saves/` in the current working directory.
- Resizable window with a 960 × 540 minimum size.

## Controls

| Input | Action |
| --- | --- |
| A / Left Arrow | Move one lane left |
| D / Right Arrow | Move one lane right |
| Space | Use a nitro charge |
| P / Escape | Pause or resume |
| Up / Down | Select menu items |
| Enter | Confirm selection |
| Mouse | Select menu buttons and adjust settings sliders |
| F | Toggle fullscreen in Settings |

The first run starts with one nitro charge. Collect a blue N pickup for another charge; shield and magnet pickups activate when collected.

## Build

The project uses the existing Visual Studio 2026, MSVC, vcpkg, x64-windows, and Raylib installation. From a Visual Studio Developer PowerShell with CMake on `PATH`, run:

```powershell
cmake --build build --config Release
```

The executable is `build\Release\NEON_RUSH.exe`. CMake is configured to use the existing vcpkg toolchain in `build/CMakeCache.txt`; configure from scratch with:

```powershell
cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-windows
cmake --build build --config Release
```

Run the logic checks with:

```powershell
ctest --test-dir build -C Release --output-on-failure
```

## Screenshots

- Main menu: `[Screenshot placeholder — capture from the running game]`
- Gameplay: `[Screenshot placeholder — capture from the running game]`
- Game over: `[Screenshot placeholder — capture from the running game]`

## Technology

- C++17
- Raylib 6.2
- CMake and CTest
- MSVC / Visual Studio 2026
- vcpkg (`x64-windows`)

## Project layout

```text
include/
  core/                 Game object interface
  entities/             Vehicles and collectibles
  game/                 Game-state orchestration
  systems/              Traffic, scoring, audio, saves, particles, factory
  world/                Procedural road and city background
src/                    Implementations and application entry point
tests/                  Logic checks
docs/                   Architecture, UML, and test notes
```

See [docs/architecture.md](docs/architecture.md), [docs/UML.md](docs/UML.md), [docs/testing.md](docs/testing.md), and [docs/viva-guide.md](docs/viva-guide.md) for implementation and viva preparation.
