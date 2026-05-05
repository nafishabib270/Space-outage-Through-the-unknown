# Modularization Guide - MVC + SOLID

## What is Modularization?

Modularization is the practice of organizing code into distinct, self-contained modules that each handle a specific concern. This project is organized into **7 main modules**, each with clear responsibilities and minimal dependencies on others.

---

## Module Overview

### 1. **Models/** - The Game's Data & Logic

**What it does**: Stores all game state and implements game rules.

**Contains**:
- `Models/Entities/` - Entity definitions (Bullet, Enemy types)
- `Models/Systems/` - Game systems (Physics, Collision, Spawning)

**Key Files**:
- `GameModel.h/cpp` - Coordinates all model systems
- `PhysicsSystem.h/cpp` - Physics calculations
- `CollisionSystem.h/cpp` - Collision detection

**When to modify**:
- Adding new game entities
- Changing physics/collision rules
- Modifying spawn behavior
- Adding new game statistics

**Example**: If you want enemies to move faster, edit `PhysicsSystem::update()` or adjust values in `EnemySpawner`.

---

### 2. **Controllers/** - Input & Game Flow

**What it does**: Handles all user input and orchestrates the application.

**Contains**:
- `GameController.h/cpp` - Main application loop
- `InputHandler.h/cpp` - Input polling
- `SpaceshipController.h/cpp` - Spaceship input mapping
- `AstronautController.h/cpp` - Astronaut input mapping

**Key Responsibility**: Convert raw input → Model mutations

**When to modify**:
- Changing input controls (keybindings)
- Adding new input types
- Changing game loop behavior
- Modifying frame rate or timing

**Example**: To make spaceship move faster, modify `SpaceshipController::handleMovement()` to increase acceleration values.

---

### 3. **Views/** - Rendering & Display

**What it does**: Renders all game state. Never modifies anything.

**Contains**:
- `Views/Rendering/` - Game world rendering
- `Views/UI/` - User interface elements

**Key Files**:
- `GameRenderer.h/cpp` - Main rendering system
- `HUD.h/cpp` - Health bars and score display
- `MenuView.h/cpp` - Menu system

**Important Rule**: These files can ONLY READ from Model, never write to it.

**When to modify**:
- Changing visual appearance
- Updating UI layout
- Adding visual effects
- Changing colors, animations

**Example**: To make the spaceship larger, modify the drawing code in `GameRenderer::renderSpaceship()`.

---

### 4. **Core/Interfaces/** - Contracts & Patterns

**What it does**: Defines interfaces and abstract types that other modules use.

**Contains**:
- `IObserver.h` - Notification pattern
- `ICollidable.h` - Collision interface
- `IUpdatable.h` - Update interface
- `IRenderable.h` - Rendering interface
- `GameStateMachine.h` - State machine
- `GameState.h` - State enumeration

**Key Principle**: These define contracts - interfaces that multiple modules implement.

**When to modify**:
- Adding new game states
- Changing observer pattern behavior
- Modifying collision interface

**Example**: To add a new game state (like "Paused"), add to `GameState` enum and add handler in `GameController`.

---

### 5. **Infrastructure/** - Utilities & Resources

**What it does**: Handles assets and cross-cutting concerns.

**Contains**:
- `ResourceManager.h/cpp` - Asset loading (textures, sounds)

**Key Principle**: This is the ONLY place assets are loaded/unloaded.

**When to modify**:
- Adding new asset types (music, fonts)
- Optimizing asset loading
- Implementing asset caching

**Example**: To add sound effects, add methods to `ResourceManager` and call them from `GameController`.

---

## Dependency Graph

```
┌─────────────────────────────────────────────┐
│        GameController (main loop)            │
│        Controllers/GameController            │
└──────────────────┬──────────────────────────┘
                   │
        ┌──────────┼──────────┐
        ▼          ▼          ▼
    InputHandler GameModel  GameRenderer
    (INPUT)      (LOGIC)    (OUTPUT)
        │          │          │
        └──────────┴──────────┘
             ▲
             │
    Core/Interfaces (contracts)
             ▲
             │
    Infrastructure/ResourceManager
```

**Flow**:
1. GameController polls InputHandler
2. GameController calls Controllers (SpaceshipController, etc.)
3. Controllers mutate GameModel
4. GameController calls GameModel.update()
5. GameRenderer reads GameModel (observes changes)
6. GameRenderer renders everything

---

## File Organization Strategy

### By Feature vs By Layer

This project uses **layered organization** (Models, Views, Controllers) rather than feature-based. This is ideal for games because:

- **Physics** changes are isolated to Models/Systems/
- **Rendering** changes are isolated to Views/
- **Input** changes are isolated to Controllers/

Alternative: Feature-based would be `Spaceship/`, `Enemies/`, `Bullets/` - mixing logic with rendering.

---

## Common Tasks

### Task 1: Add a New Enemy Type

**Files to modify**:
1. `Models/Entities/NewEnemy.cpp/h` - Create entity
2. `Models/Systems/GameModel.cpp/h` - Add collection
3. `Views/Rendering/GameRenderer.cpp` - Add rendering
4. `Models/Systems/NewEnemySpawner.cpp/h` - Add spawner (optional)

**Why separated**: Logic, spawning, rendering are independent concerns.

---

### Task 2: Change Input Controls

**Files to modify**:
1. `Controllers/InputHandler.h` - Add new input to InputSnapshot
2. `Controllers/InputHandler.cpp` - Poll new key
3. `Controllers/SpaceshipController.cpp` - Handle new input

**Why separated**: Input polling is separate from input handling.

---

### Task 3: Change Game State Logic

**Files to modify**:
1. `Core/Interfaces/GameState.h` - Add new state
2. `Controllers/GameController.cpp` - Handle state transitions
3. `Views/Rendering/GameRenderer.cpp` - Render based on state

**Why separated**: State definition, transition logic, and rendering are different concerns.

---

### Task 4: Optimize Physics

**Files to modify**:
1. `Models/Systems/PhysicsSystem.cpp` - Change calculations
2. Maybe adjust constants in entity classes

**Why isolated**: All physics is in one place - easy to find and optimize.

---

## Benefits of This Modularization

### 1. **Easy to Find Code**
- Looking for rendering? Check `Views/`
- Looking for physics? Check `Models/Systems/PhysicsSystem`
- Looking for input? Check `Controllers/`

### 2. **Easy to Change Things**
- Want different enemy colors? Modify `GameRenderer`, nothing else
- Want different movement speed? Modify `PhysicsSystem`, nothing else
- Want different controls? Modify `InputHandler` + controller

### 3. **No Circular Dependencies**
- Model doesn't know about View
- View doesn't mutate Model
- Easy to test each module independently

### 4. **Easy to Reuse**
- `PhysicsSystem` could be reused in another game
- `GameRenderer` architecture pattern could be used elsewhere
- Each module is self-contained

### 5. **Easy to Test**
- Mock GameModel and test GameRenderer independently
- Mock InputHandler and test Controllers independently
- Test each system in isolation

---

## Best Practices When Modifying

### ✅ DO

- Keep each module's responsibility focused
- Use const references to prevent accidental mutations
- Add new code in the appropriate module
- Use interfaces for dependencies
- Maintain the MVC separation

### ❌ DON'T

- Add rendering code to Model
- Add game logic to Views
- Call GameModel methods directly from Views
- Load assets outside ResourceManager
- Create tight coupling between modules

---

## Include Paths

The build system is configured so you can include headers simply:

```cpp
// Instead of:
#include "../../../Views/Rendering/GameRenderer.h"

// You can write:
#include "GameRenderer.h"  // Thanks to CMakeLists.txt configuration
```

**Include directories are configured for**:
- `.` (root)
- `Infrastructure`
- `Core/Interfaces`
- `Models/Systems`
- `Models/Entities`
- `Controllers`
- `Views/Rendering`
- `Views/UI`

---

## Adding a New Module

If you need to add a new module:

1. Create new folder (e.g., `Utilities/`)
2. Add `.cpp` and `.h` files
3. Update `CMakeLists.txt` to include new `.cpp` files
4. Update `CMakeLists.txt` `target_include_directories` if needed

Example:
```cmake
# Add to CMakeLists.txt
Utilities/MyUtility.cpp
# Add to include directories
Utilities
```

---

## Conclusion

This modular structure makes the codebase:
- **Predictable** - You know where to find things
- **Maintainable** - Changes are isolated
- **Scalable** - Easy to add new features without breaking existing code
- **Testable** - Each module can be tested independently

When making changes, always ask: "Which module should this code go in?" and "Will this create unwanted dependencies?"

