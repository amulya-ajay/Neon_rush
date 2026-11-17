# Testing

## Automated checks

Build the Release configuration and run the CTest suite:

```powershell
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
```

`NEON_RUSH_LOGIC_TESTS` checks lane limits, collision health clamping, vehicle-factory type stats, automatic traffic spawning and lane placement, score/combo behavior, and save/load of high score, coin total, and settings. The save test uses a uniquely named temporary file and removes it afterward.

## Results for this implementation

- Existing Release configuration before implementation: build succeeded.
- Updated Release application and logic-test target: build succeeded.
- CTest: `1/1` test passed.
- Runtime smoke check: launched `build/Release/NEON_RUSH.exe`; the window displayed the highway, vehicle traffic, pickups, and live HUD.

The runtime check was limited to visual startup and gameplay rendering. UI automation detected user input in the game window while I was checking controls, so I stopped interacting with it. Keyboard/mouse navigation across the menu, pause, settings, and game-over screens has not been fully verified manually.

The separate ignored `out/build/x64-Debug` cache still has `raylib_DIR` set to `NOTFOUND`; the verified build uses the repository's existing `build` Release tree and vcpkg configuration.
