# Quick Reference - MVC + SOLID Structure

## 🎯 At a Glance

```
Input → Controller → Model → Renderer
 │        (Input)   (Logic)  (Output)
 └────────────────────────────────────┘
        Dependency Inversion
```

---

## 📂 Module Quick Map

| Module | Purpose | When to Edit |
|--------|---------|--------------|
| **Models/** | Game logic & data | Physics, entities, spawning |
| **Controllers/** | Input handling | Input controls, game loop |
| **Views/** | Rendering | Graphics, UI, animations |
| **Core/** | Interfaces | Game states, contracts |
| **Infrastructure/** | Utilities | Asset loading |

---

## 🗂️ File Location Quick Map

### I need to modify...

**Physics** → `Models/Systems/PhysicsSystem.cpp`  
**Collisions** → `Models/Systems/CollisionSystem.cpp`  
**Enemies** → `Models/Entities/` or `Models/Systems/EnemySpawner.cpp`  
**Bullets** → `Models/Entities/Bullet.cpp` or `Models/Systems/BulletPool.cpp`  
**Game State** → `Core/Interfaces/GameState.h`  

**Spaceship Input** → `Controllers/SpaceshipController.cpp`  
**Astronaut Input** → `Controllers/AstronautController.cpp`  
**All Input Polling** → `Controllers/InputHandler.cpp`  
**Game Loop** → `Controllers/GameController.cpp`  

**Game Rendering** → `Views/Rendering/GameRenderer.cpp`  
**HUD Display** → `Views/UI/HUD.cpp`  
**Menu** → `Views/UI/MenuView.cpp`  
**Visual Effects** → `Views/Rendering/ThrusterEffect.cpp`  

**Assets (Textures/Sounds)** → `Infrastructure/ResourceManager.cpp`  

---

## 🔄 Data Flow Cheat Sheet

```cpp
// 1. INPUT POLLING (InputHandler)
InputSnapshot input = inputHandler.getInput();

// 2. INPUT PROCESSING (SpaceshipController)
spaceshipController.handleInput(input, model);
// Calls: model.spaceship.setVelocity(), model.fireBullet(), etc.

// 3. MODEL UPDATE (GameModel)
model.update(deltaTime);
// Updates: physics, collision, spawning, stats

// 4. OBSERVE CHANGES (GameRenderer)
renderer.onModelChanged();
// Called automatically after model.update()

// 5. RENDER (GameRenderer)
renderer.render(screenW, screenH);
// Reads: const GameModel& (never writes!)
```

---

## 🧩 Core Interfaces

| Interface | Purpose | Used By |
|-----------|---------|---------|
| `IObserver` | Notify views of changes | GameRenderer, HUD |
| `ICollidable` | Collision handling | CollisionSystem |
| `IUpdatable` | Update contract | All entities |
| `IRenderable` | Rendering contract | All visible entities |

---

## ⚡ Common Operations

### Add a New Game State
```cpp
// 1. Core/Interfaces/GameState.h
enum GameState { Menu, Playing, Paused, GameOver, NewState };

// 2. Controllers/GameController.cpp
case GameState::NewState:
    handleNewState();
    break;

// 3. Views/Rendering/GameRenderer.cpp
if (state == GameState::NewState) {
    renderNewStateUI();
}
```

### Add a New Entity Type
```cpp
// 1. Models/Entities/NewEntity.h
class NewEntity : public ICollidable {
    // Implement collision methods
};

// 2. Models/Systems/GameModel.h
std::vector<NewEntity> newEntities;

// 3. Models/Systems/GameModel.cpp
void GameModel::update(float dt) {
    for (auto& entity : newEntities) {
        entity.update(dt);
    }
}

// 4. Views/Rendering/GameRenderer.cpp
void GameRenderer::render() {
    for (auto& entity : model.getNewEntities()) {
        drawEntity(entity);
    }
}
```

### Change Input Behavior
```cpp
// 1. Controllers/InputHandler.h
struct InputSnapshot {
    // Add new input field
    bool newAction;
};

// 2. Controllers/InputHandler.cpp
newAction = IsKeyDown(KEY_X);  // Poll raw input

// 3. Controllers/SpaceshipController.cpp
if (input.newAction) {
    // Handle new action
}
```

---

## 🚫 Rules to Follow

1. **Model** = Pure logic, no rendering, no input
2. **View** = Pure rendering, read-only access to Model
3. **Controller** = Input handling & orchestration only
4. **Only ResourceManager** loads assets
5. **Use const references** to Model in View

---

## 📊 Dependency Rules

```
✅ ALLOWED:
- View depends on Model (read-only)
- Controller depends on Model & View
- Model depends on Core/Interfaces

❌ NOT ALLOWED:
- Model depends on View
- View depends on Controller
- View writes to Model
- Circular dependencies
```

---

## 🧪 Testing Strategy

```cpp
// Test Model independently
GameModel model;
model.update(1.0f);
assert(model.getScore() == expected);

// Test View with mocked Model
MockGameModel mock;
GameRenderer renderer(mock);
renderer.onModelChanged();  // Should not crash

// Test Controller with mocked Model
MockGameModel mock;
SpaceshipController controller;
controller.handleInput(input, mock);
```

---

## 🔍 Code Organization Principles

1. **SRP** - One reason to change per class
2. **DIP** - Depend on abstractions, not concrete classes
3. **OCP** - Open for extension, closed for modification
4. **ISP** - Segregated interfaces (no fat interfaces)
5. **LSP** - Substitutable implementations

---

## 📝 File Naming Convention

- **Headers**: `ClassName.h`
- **Implementation**: `ClassName.cpp`
- **Interfaces**: `IInterfaceName.h`
- **Systems**: `SystemName.h` (e.g., `PhysicsSystem.h`)
- **Utilities**: `UtilityName.h` (e.g., `ResourceManager.h`)

---

## 🏗️ Build & Run

```bash
cd "MVC + SOLID"
mkdir build && cd build
cmake ..
cmake --build . --config Release
./GeoWorld  # or GeoWorld.exe on Windows
```

---

## 📚 Documentation Files

- **ARCHITECTURE.md** - Deep dive into design decisions
- **MODULARIZATION_GUIDE.md** - How to understand the structure
- **README.md** - Project overview

---

## ❓ Quick Troubleshooting

| Issue | Check |
|-------|-------|
| Compilation error on include | Check CMakeLists.txt include paths |
| Model appears unchanged | Did you call `model.update()`? |
| View shows old data | Is GameRenderer implementing IObserver? |
| Input not responding | Check InputHandler polls the right key |
| Circular dependency error | Model shouldn't include View headers |

---

## 💡 Pro Tips

1. **Always use const references** when reading from Model in View
2. **Never load assets** outside ResourceManager
3. **Keep Model pure** - no raylib calls, no rendering
4. **Test with mocks** - Create MockGameModel for unit tests
5. **Document state changes** in GameController

---

## 🎮 Entities in This Game

| Entity | Location | Parent |
|--------|----------|--------|
| Spaceship | Model (implied) | - |
| Astronaut | Model (implied) | - |
| Bullet | `Models/Entities/Bullet` | ICollidable |
| HomingEnemy | `Models/Entities/HomingEnemy` | ICollidable |
| GroundAlien | `Models/Entities/GroundAlien` | ICollidable |

---

## 🔗 Dependencies Summary

```
GameModel
├── PhysicsSystem
├── CollisionSystem
├── EnemySpawner
├── AlienSpawner
├── BulletPool
├── SessionStats
├── Bullet (collection)
├── HomingEnemy (collection)
└── GroundAlien (collection)

GameController
├── InputHandler
├── SpaceshipController
├── AstronautController
├── GameModel (mutable)
├── GameRenderer
├── ResourceManager
└── GameStateMachine

GameRenderer
├── GameModel (const)
├── ResourceManager (const)
├── HUD
├── ThrusterEffect
└── IObserver (implements)
```

---

