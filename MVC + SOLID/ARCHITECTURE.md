# MVC + SOLID Modular Architecture

## Overview

This project implements a **strict MVC (Model-View-Controller) pattern** combined with **SOLID principles** to ensure clean, maintainable, and extensible code. The folder structure is organized into logical modules representing different concerns.

## Folder Structure

```
MVC + SOLID/
├── Models/                          # M - Game Logic & Data
│   ├── Entities/                   # Entity definitions
│   │   ├── Bullet.cpp/h           # Projectile entity
│   │   ├── HomingEnemy.cpp/h      # Smart enemy entity
│   │   └── GroundAlien.cpp/h      # Ground alien entity
│   └── Systems/                    # Game systems
│       ├── GameModel.cpp/h         # Model root - coordinates all systems
│       ├── PhysicsSystem.cpp/h     # Physics calculations
│       ├── CollisionSystem.cpp/h   # Collision detection
│       ├── EnemySpawner.cpp/h      # Enemy spawning
│       ├── AlienSpawner.cpp/h      # Alien spawning
│       ├── BulletPool.cpp/h        # Bullet pooling
│       └── SessionStats.cpp/h      # Game statistics
│
├── Controllers/                     # C - Input & Orchestration
│   ├── GameController.cpp/h        # Main application controller
│   ├── InputHandler.cpp/h          # Input polling
│   ├── SpaceshipController.cpp/h   # Spaceship input mapper
│   └── AstronautController.cpp     # Astronaut input mapper
│
├── Views/                           # V - Rendering & Display
│   ├── Rendering/                  # Rendering system
│   │   ├── GameRenderer.cpp/h      # Main game renderer
│   │   └── ThrusterEffect.cpp/h    # Visual effects
│   └── UI/                         # User interface
│       ├── HUD.cpp/h               # Heads-up display
│       └── MenuView.cpp/h          # Menu system
│
├── Core/                            # Interfaces & State
│   └── Interfaces/                 # Design patterns
│       ├── IObserver.h             # Observer pattern
│       ├── ICollidable.h           # Collision interface
│       ├── IUpdatable.h            # Update interface
│       ├── IRenderable.h           # Rendering interface
│       ├── GameState.h             # Game states enum
│       └── GameStateMachine.h      # State machine
│
├── Infrastructure/                  # Cross-cutting Concerns
│   ├── ResourceManager.cpp/h       # Asset loading/unloading
│
├── CMakeLists.txt                  # Build configuration
├── main.cpp                        # Entry point
└── build.yml                       # CI/CD configuration
```

## Module Responsibilities

### Models/ - Game Logic & State

**Purpose**: Contains all game data and logic. The single source of truth.

#### Models/Systems/GameModel (Root Model)
- Coordinates all model systems
- Owns all entity collections
- Maintains game state
- Provides const-ref accessors for View, mutable accessors for Controller
- Notifies observers after updates
- **SRP**: Coordinates systems only; no rendering or input

#### Models/Systems/ Subsystems
- **PhysicsSystem**: Numeric calculations (acceleration, friction, gravity)
- **CollisionSystem**: Overlap detection, damage application
- **EnemySpawner**: Timed spawning of homing enemies
- **AlienSpawner**: Randomized ground alien spawning
- **BulletPool**: Object pooling with cooldown management
- **SessionStats**: Score, health, level tracking

#### Models/Entities/
- **Bullet**: Projectile with position, velocity
- **HomingEnemy**: Intelligent enemy with targeting
- **GroundAlien**: Ground-based enemy with animation

**Dependencies**: Minimal internal dependencies; no external dependencies except raylib

---

### Controllers/ - Input & Orchestration

**Purpose**: Handles user input and coordinates between Model and View.

#### GameController (Root Controller)
- Main application loop (120 FPS)
- Polls input via InputHandler
- Dispatches to domain controllers
- Calls model_.update(dt)
- Calls renderer_.render()
- Manages state transitions
- Plays sound effects

#### Domain Controllers
- **InputHandler**: Polls raylib input, provides clean InputSnapshot
- **SpaceshipController**: Maps spaceship input to physics mutations
- **AstronautController**: Maps astronaut input to physics mutations

**Design**: Dependency inversion - depends on abstract GameModel and GameRenderer, not implementations

**Key Principle**: Controllers are the ONLY path to mutate the Model

---

### Views/ - Rendering & Display

**Purpose**: Displays game state without logic or mutations.

#### Views/Rendering/GameRenderer (Root View)
- Implements IObserver pattern
- Reads GameModel via const reference
- Renders all game entities
- Manages view-only state (animation frames, particles)
- Owns HUD and visual effects

#### Views/UI/
- **HUD**: Renders health bars and score (reads SessionStats)
- **MenuView**: Menu rendering and interaction
- **ThrusterEffect**: Particle visual effects (view-only state)

**Contract**: Reads Model, never writes to it

---

### Core/Interfaces/ - Design Patterns

**Purpose**: Define contracts and abstract types for decoupling.

#### Interfaces
- **IObserver**: Notification pattern (Model → View)
- **ICollidable**: Collision contract
- **IUpdatable**: Update contract
- **IRenderable**: Rendering contract
- **GameStateMachine**: State transitions
- **GameState**: Game state enumeration

**Principle**: Interface Segregation - each interface has ONE responsibility

---

### Infrastructure/

**Purpose**: Cross-cutting concerns and utilities.

#### ResourceManager
- ONLY class that loads/unloads textures and sounds
- Receives resources via dependency injection
- RAII pattern for resource cleanup
- Never accesses global state

**Principle**: SRP - resource management only

---

## Data Flow

```
1. INPUT PHASE
   InputHandler.poll() → InputSnapshot

2. UPDATE PHASE
   SpaceshipController/AstronautController
   - Read InputSnapshot
   - Call GameModel mutation methods

3. MODEL UPDATE PHASE
   GameModel.update(dt)
   - PhysicsSystem.update(dt)
   - CollisionSystem.update()
   - EnemySpawner.update(dt)
   - AlienSpawner.update(dt)
   - BulletPool.update(dt)

4. NOTIFICATION PHASE
   GameModel.notifyObservers()
   - GameRenderer.onModelChanged()

5. RENDER PHASE
   GameRenderer.render()
   - Reads GameModel (const ref)
   - Renders everything
   - Never writes to Model
```

---

## SOLID Principles Applied

### Single Responsibility Principle (SRP)
- Each class has ONE reason to change
- PhysicsSystem changes only for physics math
- GameRenderer changes only for rendering logic
- InputHandler changes only for input handling

### Open/Closed Principle (OCP)
- New entity types extend ICollidable without changing CollisionSystem
- New states extend GameState without changing state machine core
- New views implement IObserver without changing GameModel

### Liskov Substitution Principle (LSP)
- HomingEnemy and GroundAlien substitute via ICollidable interface
- Contracts never violated

### Interface Segregation Principle (ISP)
- ICollidable: collision methods ONLY
- IUpdatable: update methods ONLY
- IRenderable: rendering methods ONLY
- No bloated interfaces

### Dependency Inversion Principle (DIP)
- CollisionSystem depends on `ICollidable*` (abstraction), not concrete types
- GameRenderer depends on const GameModel& (abstraction)
- ResourceManager passed via dependency injection

---

## Adding New Features

### Adding a New Enemy Type

1. **Create entity** in `Models/Entities/NewEnemy.cpp/h`
   - Inherit from ICollidable
   - Implement collision methods only

2. **Add to GameModel** in `Models/Systems/GameModel.cpp/h`
   - Add collection for new enemy type
   - Update methods to iterate and update

3. **Add spawner** (optional) in `Models/Systems/`
   - Create NewEnemySpawner if needed
   - Add to GameModel

4. **Add rendering** in `Views/Rendering/GameRenderer.cpp`
   - Read from GameModel const ref
   - Draw the new entity type

5. **Add input mapping** (if player-controlled) in `Controllers/`
   - Create NewEntityController
   - Wire into GameController

### Adding New Game State

1. Extend `GameState` enum in `Core/Interfaces/GameState.h`
2. Add state handler in `GameController`
3. Add state check in `GameRenderer` for conditional rendering

### Adding New Input

1. Add new key/button to `InputHandler`
2. Add new controller mapping

---

## Build & Compile

```bash
cd "MVC + SOLID"
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

The executable will be in `build/GeoWorld` (or `GeoWorld.exe` on Windows).

---

## Key Insights

- **Model owns all state**, Controllers mutate it, View observes it
- **No raylib calls in Model or Controllers** (except Input polling)
- **ResourceManager is the only asset loader** - never load directly in classes
- **const references prevent accidental mutations** in the View
- **IObserver avoids circular dependency** - Model doesn't know about View
- **Tests can mock GameModel** - MVC decoupling enables unit testing

---

## Conclusion

This architecture prioritizes:
1. **Separation of Concerns** - Clear MVC boundaries
2. **Testability** - Minimal dependencies, interfaces for mocking
3. **Extensibility** - New features don't require modifying existing code
4. **Maintainability** - Each class has one clear purpose
5. **Reusability** - Systems can be reused in other projects

