# GeoWorld — Space Outage

A 2D side-scrolling action game built with **raylib** and **C++17**.  
Refactored using **MVC architecture** and all five **SOLID principles**.

---

## ▶ Play Immediately (No Build Required)

1. Download `dist/GeoWorld-Windows.zip` from this repository
2. Extract the zip anywhere on your Windows PC
3. Double-click `GeoWorld.exe` — done

No installers. No DLLs. No Visual C++ redistributable. Just run.

---

## Gameplay

| Phase | Description |
|-------|-------------|
| **Phase 1 — Spaceship** | Fly with WASD. Left-click shoots right. Survive homing drones. When health hits zero the ship crashes. |
| **Phase 2 — Astronaut** | Move and jump with WASD. Left-click shoots toward the mouse. Fight remaining drones and ground aliens. |

### Controls

| Input | Action |
|-------|--------|
| `W S A D` | Move spaceship / walk and jump (astronaut) |
| `W` on ground | Jump |
| Left Mouse | Fire (spaceship: right / astronaut: toward cursor) |
| `L` | God mode (no damage — debug key) |
| `ESC` | Quit |

---

## Build From Source

**Requirements:** CMake ≥ 3.16, C++17 compiler, internet (CMake fetches raylib).

```bash
git clone https://github.com/YOUR_USERNAME/geoworld.git
cd geoworld
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

- **Windows:** `build/Release/GeoWorld.exe`
- **Linux:** install `libgl1-mesa-dev libx11-dev libxrandr-dev libxi-dev` first, then `build/GeoWorld`
- **macOS:** `build/GeoWorld`

---

## Assets

Place these files in the `assets/` folder before building:

```
spaceship1.png            Spaceship sprite
enemy.png                 Homing enemy sprite
Alien1.png                Ground alien sprite sheet (8 frames)
Char_Robot.png            Astronaut sprite sheet (8 frames)
bullet_1.png              Bullet sprite
hiteffect.png             Hit flash sprite
Map1.png                  Scrolling background
background_music_1.mp3    Background music
laser-gun-81720.mp3       Laser fire sound
hit_sound.wav             Hit impact sound
```

---

## Project Structure

```
geoworld/
├── assets/                         ← All game assets
├── include/
│   ├── core/                       ← Interfaces & infrastructure
│   │   ├── ICollidable.h           ← ISP: collision contract only
│   │   ├── IRenderable.h           ← ISP: render contract only
│   │   ├── IUpdatable.h            ← ISP: update contract only
│   │   ├── IObserver.h             ← MVC Observer pattern
│   │   ├── GameState.h             ← enum class (scoped, no globals)
│   │   ├── GameStateMachine.h      ← State transition logic
│   │   └── ResourceManager.h       ← RAII asset ownership
│   ├── model/                      ← All game state (zero rendering)
│   │   ├── GameModel.h             ← Model root, notifies View
│   │   ├── SessionStats.h          ← Score, health, hit power
│   │   ├── entities/
│   │   │   ├── Bullet.h / BulletPool.h
│   │   │   ├── HomingEnemy.h       ← Intercept AI
│   │   │   └── GroundAlien.h       ← Animated ground enemy
│   │   └── systems/
│   │       ├── CollisionSystem.h   ← ICollidable* only (DIP)
│   │       ├── EnemySpawner.h      ← Timed slot spawning
│   │       ├── AlienSpawner.h      ← Randomised spawning
│   │       └── PhysicsSystem.h     ← Movement & jump maths
│   ├── view/                       ← Rendering (reads Model, never writes)
│   │   ├── GameRenderer.h          ← Main renderer (IObserver)
│   │   ├── HUD.h                   ← Health bar + score
│   │   ├── MenuView.h              ← Menu screen
│   │   └── ThrusterEffect.h        ← Particle exhaust
│   └── controller/                 ← Input → Model mutations
│       ├── GameController.h        ← App loop, composition root
│       ├── InputHandler.h          ← Raw input polling only
│       ├── SpaceshipController.h
│       └── AstronautController.h
├── src/                            ← Implementations (mirrors include/)
├── .github/workflows/build.yml     ← Auto-builds Windows + Linux
└── CMakeLists.txt
```

---

## MVC Architecture

```
INPUT
  │
  ▼
CONTROLLER ──mutates──▶ MODEL ──notifies──▶ VIEW
(GameController)        (GameModel)         (GameRenderer)
(SpaceshipCtrl)         (SessionStats)      (HUD)
(AstronautCtrl)         (HomingEnemy)       (MenuView)
(InputHandler)          (BulletPool)        (ThrusterEffect)
                        (CollisionSystem)
                        (PhysicsSystem)
```

**Model** owns all game state. Zero rendering calls.  
**View** reads Model via `const&`. Never writes to Model.  
**Controller** reads input, calls Model mutators, triggers View.  
**HitEvents** produced by CollisionSystem let GameRenderer draw hit
flashes without any `DrawTexturePro` call inside the Model.

---

## SOLID Principles

| Principle | Applied Where |
|-----------|--------------|
| **S — Single Responsibility** | `SessionStats` tracks numbers only. `HUD` draws UI only. `PhysicsSystem` computes movement only. `CollisionSystem` tests overlaps only. `ResourceManager` loads/unloads assets only. |
| **O — Open/Closed** | Add `ShieldedEnemy : ICollidable` in a new file — `CollisionSystem`, `GameModel`, and all spawners are untouched. |
| **L — Liskov Substitution** | `HomingEnemy*` and `GroundAlien*` both substitute as `ICollidable*` in `CollisionSystem::testBulletsVsEntities()` with identical correct behaviour. |
| **I — Interface Segregation** | Four minimal interfaces: `ICollidable`, `IRenderable`, `IUpdatable`, `IObserver`. `ThrusterEffect` is `IRenderable` + `IUpdatable` only — never forced to implement collision or spawn methods. |
| **D — Dependency Inversion** | `CollisionSystem` accepts `ICollidable*`. `GameController` receives `GameModel&` and `GameRenderer&`. `ResourceManager` injected through constructor — zero globals. |

---

## Bugs Fixed From Original C Code

| Original Bug | Fix |
|---|---|
| `alien_timer += dt` called **twice per frame** | `AlienSpawner::update()` increments once |
| `srand(time(NULL))` at 120 fps inside movement | Called once in `GameController` constructor |
| `UnloadTexture(spaceship)` called twice | RAII destructor in `ResourceManager` |
| `DrawHealthBar()` defined inside `main()` | `HUD::drawHealthBar()` — View layer |
| ~30 global variables | Zero globals — all state private to owning class |
| 6 copy-pasted collision loops | 1 `CollisionSystem::testBulletsVsEntities()` |

---

## GitHub Actions — Automatic Builds

Every push to `main` builds both platforms automatically.  
Every GitHub Release attaches the zips automatically.

```bash
# Create and push a release tag
git tag v1.0.0
git push origin v1.0.0
# Then publish the Release on GitHub → zips are attached automatically
```
