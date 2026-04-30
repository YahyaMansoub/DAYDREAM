# Projectile Simulation

Simple SFML 3 projectile simulator with a built-in UI (no ImGui dependency).

## Build

Requirements:
- CMake 3.20+
- SFML 3.x (graphics, window, system)
- A C++17 compiler

Optional: If you want text labels in the UI, place a TTF font at:
- Projectile_Simulation/assets/DejaVuSans.ttf

Or rely on system fonts (Linux or Windows) which the app will try to load.

From this folder:

```
cmake -S . -B build
cmake --build build
```

Run:

```
./build/projectile_app
```
