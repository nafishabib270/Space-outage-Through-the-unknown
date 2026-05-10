# GeoWorld — Complete Code Documentation

## Table of Contents
1. [Architecture Overview](#architecture-overview)
2. [Execution Flow](#execution-flow)
3. [Component Breakdown](#component-breakdown)
4. [Data Flow & Synchronization](#data-flow--synchronization)
5. [SOLID Principles Implementation](#solid-principles-implementation)
6. [Key Systems](#key-systems)
7. [Two-Phase Gameplay](#two-phase-gameplay)
8. [Design Patterns](#design-patterns)

---

## Architecture Overview

This project uses **MVC (Model-View-Controller)** architecture combined with **SOLID principles**.

### The Three-Layer Structure

```
┌─────────────────────────────────────────────────────────────┐
│                        INPUT LAYER                          │
│                                                             │
│  Raylib Input Events → InputHandler.poll() → InputSnapshot │
└─────────────────────────────────────────────────────────────┘
                              ↓
┌─────────────────────────────────────────────────────────────┐
│                     CONTROLLER LAYER                        │
│                                                             │
│  GameController (orchestrator)                             │
│    ├── SpaceshipController → handles spaceship input       │
│    ├── AstronautController → handles astronaut input       │
│    └── Manages GameStateMachine (Menu/Playing/Exit)        │
│                                                             │
│  Controllers call Model mutation methods in response to     │
│  input. They NEVER render or do physics math.              │
└─────────────────────────────────────────────────────────────┘
                              ↓
┌─────────────────────────────────────────────────────────────┐
│                      MODEL LAYER                            │
│                                                             │
│  GameModel — Single Source of Truth                        │
│    ├── SessionStats (score, health, phase)                 │
│    ├── BulletPool (bullet management)                      │
│    ├── PhysicsSystem (movement math)                       │
│    ├── CollisionSystem (hit detection)                     │
│    ├── EnemySpawner (homing drone spawning)                │
│    ├── AlienSpawner (ground alien spawning)                │
│    └── Observers (registered View components)              │
│                                                             │
│  After update():                                            │
│    → notifyObservers() → IObserver::onModelChanged()       │
└─────────────────────────────────────────────────────────────┘
                              ↓
┌─────────────────────────────────────────────────────────────┐
│                       VIEW LAYER                            │
│                                                             │
│  GameRenderer (implements IObserver)                        │
│    ├── Reads GameModel (const references only)             │
│    ├── renderGame() — draws everything                     │
│    ├── onModelChanged() — called by Model after update     │
│    ├── HUD (health, score display)                         │
│    └── ThrusterEffect (visual feedback)                    │
│                                                             │
│  The View NEVER mutates the Model.                         │
│  The Model NEVER imports View code.                        │
└─────────────────────────────────────────────────────────────┘
```

### Key Principle: MVC Isolation

```
Model ← knows nothing about View
  ↓
  └─→ notifyObservers() → View (via IObserver interface)
  
Controller → reads InputSnapshot → calls Model methods
           → never touches View or rendering
```

---

## Execution Flow

### Main Loop (GameController::run)

```
1. INITIALIZATION
   ├── Create GameModel (initializes all systems)
   ├── Create GameRenderer, register as observer: model_.addObserver(&renderer_)
   ├── Load resources: resources_.load()
   └── SetTargetFPS(120)

2. MAIN LOOP (while !WindowShouldClose())
   │
   ├─→ DISPATCH (State Machine)
   │   ├── if (stateMachine_.is(Menu)) → handleMenu()
   │   └── if (stateMachine_.is(Playing)) → handleGame()
   │
   ├─→ handleGame() ─────────────────────────────────────────
   │   │
   │   ├─ INPUT POLLING
   │   │  └─ InputSnapshot input = inputHandler_.poll()
   │   │      └─ Reads: IsKeyDown(W/A/S/D), IsMouseButtonDown(MOUSE_LEFT_BUTTON), etc.
   │   │
   │   ├─ PHASE 1: SPACESHIP
   │   │  └─ SpaceshipController::handleInput(input, dt)
   │   │     ├─ input.up → calls model_.getPhysics().moveUp(spaceship, dt, height)
   │   │     ├─ input.down → calls model_.getPhysics().moveDown(spaceship, dt, height)
   │   │     ├─ input.left → calls model_.getPhysics().moveLeft(spaceship, dt)
   │   │     ├─ input.right → calls model_.getPhysics().moveRight(spaceship, dt, width)
   │   │     └─ input.fire → adds bullet to BulletPool
   │   │
   │   ├─ PHASE 2: ASTRONAUT
   │   │  └─ AstronautController::handleInput(input, dt)
   │   │     ├─ input.left → calls physics_.moveAstroLeft(astronaut, dt)
   │   │     ├─ input.right → calls physics_.moveAstroRight(astronaut, dt, width)
   │   │     ├─ input.jump → calls physics_.startJump()
   │   │     └─ input.fire → calculates bullet direction from mouse position
   │   │
   │   ├─ MODEL UPDATE
   │   │  └─ model_.update(dt)
   │   │     ├─ SpaceshipPhase Update:
   │   │     │  ├─ physics_.applyFriction(dt)
   │   │     │  ├─ enemySpawner_.update(dt)
   │   │     │  ├─ alienSpawner_.update(dt)
   │   │     │  ├─ Move all enemies
   │   │     │  ├─ Update bullets
   │   │     │  ├─ TEST COLLISIONS
   │   │     │  │  └─ collisionSystem_.testBulletsVsEntities(bullets, enemies)
   │   │     │  │     ├─ For each bullet: check if overlaps any enemy
   │   │     │  │     ├─ If hit: enemy.takeDamage(), add to HitEvent list
   │   │     │  │     └─ Return {damages, killed}
   │   │     │  └─ testEntitiesVsPlayer(enemies, playerRect)
   │   │     │     ├─ For each enemy: check if overlaps spaceship
   │   │     │     └─ If hit: ship health -= damage, add to HitEvent list
   │   │     │
   │   │     ├─ or AstronautPhase Update:
   │   │     │  ├─ physics_.updateAstronaut(astronaut, dt)
   │   │     │  ├─ [Same collision tests as spaceship phase]
   │   │     │  └─ physics_.updateCrash() checks for crash completion
   │   │     │
   │   │     └─ scrollBackground(dt)
   │   │
   │   ├─ NOTIFY OBSERVERS
   │   │  └─ model_.notifyObservers()
   │   │     └─ For each observer in observers_ vector:
   │   │        └─ observer→onModelChanged()
   │   │           └─ GameRenderer::onModelChanged()
   │   │              └─ Caches frame data (animations, effects)
   │   │
   │   └─ RENDER
   │      ├─ BeginDrawing()
   │      ├─ ClearBackground(LIGHTGRAY)
   │      ├─ renderer_.renderGame()
   │      │  ├─ drawBackground()
   │      │  ├─ drawSpaceship() or drawAstronaut()
   │      │  ├─ drawEnemies()
   │      │  ├─ drawAliens()
   │      │  ├─ drawBullets()
   │      │  ├─ drawHitEffects() ← HitEvent data from Model
   │      │  └─ drawHUD() ← Stats from Model
   │      └─ EndDrawing()
   │
   └─→ 60 FPS sync via SetTargetFPS(120)

3. CLEANUP
   └── Resources freed automatically (destructors)
```

---

## Component Breakdown

### 1. GameController (Controllers/GameController.h)

**Responsibility:** Orchestrate the entire application loop.

**Key Members:**
```cpp
ResourceManager      resources_;      // Asset manager
GameModel            model_;          // The game state
GameRenderer         renderer_;       // View component
GameStateMachine     stateMachine_;   // Menu/Playing/Exit state
InputHandler         inputHandler_;   // Raw input polling
SpaceshipController  spaceshipCtrl_;  // Spaceship input handler
AstronautController  astronautCtrl_;  // Astronaut input handler
```

**SOLID Compliance:**
- **SRP:** Only orchestrates; doesn't do physics, rendering, or input reading directly
- **DIP:** Depends on GameModel reference (abstract interface)
- **OCP:** Adding new states is just a new handler method

**Key Methods:**
- `run()` — Main loop
- `handleMenu()` — Menu input and rendering
- `handleGame()` — Game input and update
- `playSoundForHits()` — Audio feedback

---

### 2. GameModel (Models/Systems/GameModel.h)

**Responsibility:** Own all game state. Never touch rendering.

**Key Members:**
```cpp
SessionStats    stats_;         // Score, health, phase
BulletPool      bulletPool_;    // All bullets
EnemySpawner    enemySpawner_;  // Homing drone spawner
AlienSpawner    alienSpawner_;  // Ground alien spawner
CollisionSystem collisionSystem_; // Hit detection
PhysicsSystem   physics_;        // Movement math
Rectangle spaceshipPos_;         // Spaceship hitbox
Rectangle astronautPos_;         // Astronaut hitbox
bool onSpaceship_;               // Current phase flag
```

**SOLID Compliance:**
- **SRP:** Coordinates systems; no rendering code anywhere
- **DIP:** CollisionSystem depends on ICollidable* abstractions
- **OCP:** New entity types extend ICollidable; Model unchanged

**Key Methods:**
```cpp
void update(float dt);              // Core game logic
void addObserver(IObserver* o);     // Register View
void notifyObservers();             // Tell View to update
const SessionStats& getStats() const; // View reads stats
```

**Critical Contract with View:**
```cpp
// Const accessors ONLY for the View
const SessionStats&    getStats()         const;
const BulletPool&      getBulletPool()    const;
const EnemySpawner&    getEnemySpawner()  const;
// ... all return const references or bool values
```

**Critical Contract with Controller:**
```cpp
// Mutable accessors ONLY for Controllers
BulletPool&    getBulletPool();   // Add bullets
PhysicsSystem& getPhysics();      // Call movement methods
Rectangle&     getSpaceshipPos(); // Controllers set position
```

---

### 3. InputHandler (Controllers/InputHandler.h)

**Responsibility:** Poll raw Raylib input into clean snapshots.

**Key Type:**
```cpp
struct InputSnapshot {
    bool up, down, left, right;  // WASD movement
    bool jump;                    // Space
    bool fire;                    // Mouse click
    bool anyMovement;             // Any key pressed
    Vector2 mousePos;             // Cursor position
    bool mouseDown;               // LMB pressed
};
```

**SOLID Compliance:**
- **SRP:** Only input polling; no game logic
- **DIP:** Called by Controllers; doesn't know about Model

**Key Method:**
```cpp
InputSnapshot poll() const;  // Called every frame by Controller
```

---

### 4. SpaceshipController & AstronautController

**Responsibility:** Translate input into Model mutation calls.

**SpaceshipController::handleInput()**
```cpp
if (input.up)    physics.moveUp(spaceship, dt, height);
if (input.down)  physics.moveDown(spaceship, dt, height);
if (input.left)  physics.moveLeft(spaceship, dt);
if (input.right) physics.moveRight(spaceship, dt, width);
if (input.fire)  bulletPool.add(bullet);
```

**AstronautController::handleInput()**
```cpp
if (input.left)  physics.moveAstroLeft(astronaut, dt);
if (input.right) physics.moveAstroRight(astronaut, dt, width);
if (input.jump)  physics.startJump();
if (input.fire)  bulletPool.add(bullet_toward_mouse);
```

**SOLID Compliance:**
- **SRP:** Only input → Model mapping
- **DIP:** Doesn't know about specific entity types

---

### 5. GameRenderer (Views/Rendering/GameRenderer.h)

**Responsibility:** Render everything. Never mutate Model.

**Key Members:**
```cpp
const GameModel&       model_;      // Read-only reference
const ResourceManager& res_;        // Textures & sounds
HUD                    hud_;        // Score/health display
ThrusterEffect         rearThrusters_;   // Spaceship thruster effect
ThrusterEffect         sideThrusters_;   // Side thruster effect
```

**MVC Contract (implements IObserver):**
```cpp
void onModelChanged() override {
    // Called by Model after update()
    // Cache animation frames, effect positions, etc.
}

void renderGame() const {
    // Executed inside BeginDrawing/EndDrawing
    drawBackground();
    drawSpaceship();    // or drawAstronaut()
    drawEnemies();
    drawAliens();
    drawBullets();
    drawHitEffects();   // Uses HitEvent data from Model
    drawHUD();
}
```

**CRITICAL:** All render methods are `const`. No Model mutation.

**SOLID Compliance:**
- **SRP:** Only rendering; no logic
- **DIP:** Reads abstract Model interface (const accessors)
- **OCP:** New entity types → new draw method (no Model changes)

---

## Data Flow & Synchronization

### One-Way Data Flow (MVC)

```
INPUT
  ↓
Controller → calls Model method (e.g., physics.moveUp())
  ↓
Model.update() → advances all systems
  ↓
Model.notifyObservers() → renderer.onModelChanged()
  ↓
Renderer caches data (animation frames, effect positions)
  ↓
Renderer.renderGame() → draws to screen
```

**Key Insight:** The View never triggers Model updates. Only the Controller does.

### Observer Pattern (Model ↔ View)

**Registration:**
```cpp
// In GameController::GameController()
model_.addObserver(&renderer_);  // Register the View
```

**Notification:**
```cpp
// In GameModel::update()
notifyObservers();  // After all systems have updated

// Implementation
void GameModel::notifyObservers() {
    for (auto* observer : observers_) {
        observer->onModelChanged();  // Virtual call
    }
}
```

**View Response:**
```cpp
// In GameRenderer (overrides IObserver)
void GameRenderer::onModelChanged() override {
    // Cache data for this frame (optional; could skip for performance)
    // Example: astroFrameTimer_ += dt
}

// Then BeginDrawing/EndDrawing calls:
renderGame();  // Uses cached Model data
```

---

## SOLID Principles Implementation

### 1. Single Responsibility Principle (SRP)

| Component | Single Responsibility |
|-----------|----------------------|
| `GameModel` | Own and update all game state |
| `PhysicsSystem` | Calculate movement math |
| `CollisionSystem` | Detect overlaps and apply immediate consequences |
| `GameRenderer` | Render to screen |
| `InputHandler` | Poll raw input into clean snapshots |
| `ResourceManager` | Load and own assets |
| `EnemySpawner` | Manage homing enemy pool and timing |

**Example: PhysicsSystem**
```cpp
// PhysicsSystem does ONLY physics math
void moveUp(Rectangle& pos, float dt, int screenH) {
    speed_ = std::min(speed_ + ACCELERATION * dt, MAX_SPEED);
    pos.y -= speed_ * dt;
    pos.y = std::max(pos.y, 0.0f);  // Clamp to screen
}

// It does NOT:
// - Check if the entity is alive (that's SessionStats)
// - Render anything (that's GameRenderer)
// - Play sounds (that's GameController)
```

### 2. Open/Closed Principle (OCP)

**Closed for modification, open for extension.**

**Example: Adding a new enemy type**

Current system:
```cpp
// CollisionSystem depends on ICollidable*, not concrete types
std::vector<HitEvent> testBulletsVsEntities(
    std::vector<Bullet>&      bullets,
    std::vector<ICollidable*> entities);  // ← Abstract pointers
```

Adding `GroundAlien` required:
1. Implement `ICollidable` in `GroundAlien`
2. Return `ICollidable*` from `alienSpawner_.getAll()`
3. No changes to `CollisionSystem` code

**CollisionSystem stays closed for modification.**

---

### 3. Liskov Substitution Principle (LSP)

**Subtypes must be substitutable for base types.**

```cpp
// These are all interchangeable in CollisionSystem
class Bullet : public ICollidable { ... };
class HomingEnemy : public ICollidable { ... };
class GroundAlien : public ICollidable { ... };

// CollisionSystem works with pointers, not concrete types
testBulletsVsEntities(bullets, {
    (ICollidable*)&homingEnemy,
    (ICollidable*)&groundAlien,
    // ... and bullets are tested against both identically
});
```

---

### 4. Interface Segregation Principle (ISP)

**Clients should not depend on interfaces they don't use.**

**Bad (Fat Interface):**
```cpp
class IEntity {
    virtual void update(float dt) = 0;
    virtual void render(Renderer&) = 0;
    virtual Rectangle getBounds() = 0;
    virtual void takeDamage(float) = 0;
};
```

**Good (Segregated Interfaces):**
```cpp
class IUpdatable {
    virtual void update(float dt) = 0;
};

class IRenderable {
    virtual void render(Renderer&) = 0;
};

class ICollidable {
    virtual Rectangle getBounds() = 0;
    virtual void takeDamage(float) = 0;
};

// CollisionSystem only depends on what it needs
class CollisionSystem {
    testBulletsVsEntities(
        std::vector<Bullet>&,
        std::vector<ICollidable*>  // ← Only collision contract
    );
};

// GameModel only depends on what it needs
class GameModel {
    void update() {
        // Only calls ICollidable methods, not render
    }
};
```

---

### 5. Dependency Inversion Principle (DIP)

**Depend on abstractions, not concrete implementations.**

**Example: CollisionSystem**

```cpp
// CollisionSystem depends on abstraction (ICollidable*)
class CollisionSystem {
public:
    std::vector<HitEvent> testBulletsVsEntities(
        std::vector<Bullet>&,
        std::vector<ICollidable*> entities  // ← Abstraction, not concrete
    );
};

// GameModel passes concrete objects as abstractions
std::vector<ICollidable*> allCollidables = getAllCollidables();
// Returns HomingEnemy*, GroundAlien*, etc., as ICollidable*
collisionSystem_.testBulletsVsEntities(bullets, allCollidables);
```

**Benefits:**
- CollisionSystem never imports `HomingEnemy.h` or `GroundAlien.h`
- Adding new enemy types requires zero changes to CollisionSystem
- Testing CollisionSystem in isolation is trivial (mock ICollidable)

---

## Key Systems

### PhysicsSystem

**Handles all numeric motion calculations.**

```cpp
// Spaceship flight
void moveUp   (Rectangle& pos, float dt, int screenH);
void moveDown (Rectangle& pos, float dt, int screenH);
void moveLeft (Rectangle& pos, float dt);
void moveRight(Rectangle& pos, float dt, int screenW);
void applyFriction(float dt);      // Deceleration
bool updateCrash(Rectangle& pos, float dt, int screenH);  // Falling animation

// Astronaut
void moveAstroLeft (Rectangle& pos, float dt);
void moveAstroRight(Rectangle& pos, float dt, int screenW);
void startJump();
void updateAstronaut(Rectangle& pos, float dt);  // Gravity + jump
bool isOnAir() const;
```

**Constants:**
```cpp
static constexpr float INIT_SPEED    = 200.0f;
static constexpr float ACCELERATION  = 5.0f;
static constexpr float JUMP_SPEED    = 700.0f;
static constexpr float ASTRO_GRAVITY = 1500.0f;
```

---

### CollisionSystem

**Detects overlaps and applies immediate damage.**

```cpp
// Returns list of hit events for View to render effects
std::vector<HitEvent> testBulletsVsEntities(
    std::vector<Bullet>&,
    std::vector<ICollidable*> entities);

// Returns total damage dealt to player
int testEntitiesVsPlayer(
    std::vector<ICollidable*> entities,
    Rectangle playerRect,
    int damagePerHit);
```

**Key Structure:**
```cpp
struct HitEvent {
    Rectangle entityBounds;  // Where to draw the hit effect
    bool killed;             // Did it die?
};
```

**Execution:**
1. For each bullet: loop through entities checking `CheckCollisionRecs()`
2. If overlap found:
   - Call `entity→takeDamage(bulletDamage)`
   - Add HitEvent to return list
   - Remove bullet from pool
3. View uses HitEvent data to render hit flash sprites

---

### EnemySpawner & AlienSpawner

**Manage pools of enemies with spawn timing.**

```cpp
class EnemySpawner {
    void update(float dt);         // Spawn timer, remove dead
    const std::vector<HomingEnemy>& getAll() const;
    std::vector<ICollidable*> getCollidables();
};

class AlienSpawner {
    void update(float dt);
    const std::vector<GroundAlien>& getAll() const;
    std::vector<ICollidable*> getCollidables();
};
```

---

### BulletPool

**Memory-efficient bullet management with pooling.**

```cpp
class BulletPool {
public:
    void add(const Bullet& b);           // Fire a bullet
    std::vector<Bullet>& getAll();       // For rendering
    void update(float dt);               // Move and remove off-screen
    void clear();                        // Reset for new phase
};
```

**Benefits:**
- No dynamic allocation during gameplay
- Pool-size pre-allocated in constructor
- Bullets automatically removed when off-screen

---

### SessionStats

**Tracks game progress and state.**

```cpp
class SessionStats {
    int getHealth() const;
    int getScore() const;
    bool spaceshipDestroyed() const;   // Phase transition
    bool astronautDead() const;         // Game over
    
    void takeDamage(int amount);
    void awardScore(int points);
    void destroySpaceship();
};
```

---

## Two-Phase Gameplay

### Phase 1: Spaceship (Flying)

**State:** `onSpaceship_ = true`

**Player Control:**
- W/A/S/D to move
- Mouse left-click to fire right
- Bullets fire horizontally to the right

**Physics:**
- Acceleration/friction flight model
- Speed capped at 400 px/sec
- Clamped to screen bounds

**Enemies:**
- **HomingEnemies:** Intercept the spaceship
- **GroundAliens:** Walking on the ground
- Only homing enemies target the ship

**Transition:**
When `spaceship.health <= 0`:
1. `stats_.destroySpaceship()` sets `spaceshipDestroyed_ = true`
2. `GameModel::updateSpaceshipPhase()` detects this
3. Sets `onSpaceship_ = false` → Phase 2
4. Transitions `astronautPos_` to screen
5. Clears `bulletPool_` (spaceship bullets disappear)
6. `physics_.reset()` — astronaut physics initialized

---

### Phase 2: Astronaut (Ground Combat)

**State:** `onSpaceship_ = false`

**Player Control:**
- W/A/S/D to move left/right and jump
- Mouse left-click fires toward cursor
- Bullets fire at mouse angle

**Physics:**
- WALK_SPEED = 200 px/sec horizontal
- JUMP_SPEED = 700 px/sec upward
- GRAVITY = 1500 px/sec² downward
- Landing on ground (Y >= 535) ends jump

**Enemies:**
- Same enemies from Phase 1 remain
- New spawns continue until game over

**Game Over:**
When `astronaut.health <= 0`:
1. `stats_.astronautDead() = true`
2. `GameModel::isGameOver()` returns true
3. GameController detects this
4. Transitions to Menu state

---

## Design Patterns

### 1. Model-View-Controller (MVC)

**Separation of concerns:**

```
Model (data + logic)
  ↓ (notifies)
  → Observer (View)
  
Controller (input → Model calls)
  ↑ (reads)
  ← InputHandler
```

**In Code:**
- Model owns all state (GameModel)
- View reads Model (GameRenderer)
- Controller translates input (InputHandler → SpaceshipController)

---

### 2. Observer Pattern

**Decouples Model from View.**

```cpp
// Model declares observers
std::vector<IObserver*> observers_;

// View registers itself
model_.addObserver(&renderer_);

// Model notifies after update
void GameModel::notifyObservers() {
    for (auto* obs : observers_) {
        obs->onModelChanged();
    }
}

// View responds
void GameRenderer::onModelChanged() override {
    // Cache animation frames, etc.
}
```

---

### 3. Strategy Pattern (Implicit)

**PhysicsSystem changes behavior based on game phase.**

```cpp
if (onSpaceship_) {
    updateSpaceshipPhase(dt);  // Uses spaceship physics
} else {
    updateAstronautPhase(dt);  // Uses astronaut/gravity physics
}
```

---

### 4. Object Pool Pattern

**BulletPool reuses bullet objects instead of allocating/deallocating.**

```cpp
class BulletPool {
    std::vector<Bullet> pool_;  // Fixed-size pool
    void add(const Bullet& b);  // Reuses existing slots
};
```

---

### 5. Factory Pattern

**Spawners create entities on demand.**

```cpp
// EnemySpawner creates HomingEnemies
class EnemySpawner {
    void update(float dt) {
        // Creates new enemies at spawn timer intervals
    }
};

// AlienSpawner creates GroundAliens
class AlienSpawner {
    void update(float dt) {
        // Creates new aliens at random intervals
    }
};
```

---

## Linking It All Together

### From Raw Input to On-Screen Rendering

**Step-by-step trace of a spaceship jump:**

```
1. PlayerPresses(W)
   ↓
2. InputHandler::poll() reads IsKeyDown('W')
   ↓
3. InputSnapshot.up = true
   ↓
4. SpaceshipController::handleInput(input, dt)
   ├─→ if (input.up)
   │   └─→ model_.getPhysics().moveUp(spaceship, dt, screenH)
   │
5. PhysicsSystem::moveUp()
   ├─→ speed_ += ACCELERATION * dt
   ├─→ spaceship.y -= speed_ * dt
   ├─→ Clamp spaceship.y to [0, screenH]
   │
6. GameModel::update() (called by GameController)
   ├─→ (spaceship position already updated by controller)
   ├─→ Check physics_.applyFriction() (if no longer pressing)
   ├─→ Call collisionSystem_.testEntitiesVsPlayer()
   ├─→ notifyObservers()
   │
7. GameRenderer::onModelChanged()
   ├─→ Cache spaceship position (optional)
   │
8. GameRenderer::renderGame()
   ├─→ DrawTexturePro(spaceship texture, srcRect, spaceship.position, ...)
   │
9. Screen updated → player sees spaceship moved up
```

---

### Data Ownership Flow

```
Input Data         → InputSnapshot (temporary, stack-based)
                       ↓
Physical State     → PhysicsSystem members (private)
                       ↓
Entity Positions   → GameModel members (spaceship/astronautPos)
                       ↓
Enemies/Bullets    → GameModel systems (BulletPool, Spawners)
                       ↓
Game Stats         → SessionStats (health, score)
                       ↓
Hit Effects        → CollisionSystem → HitEvent list
                       ↓
Rendering Data     → GameRenderer (const references only)
```

---

## Initialization Sequence

```
main.cpp
  ↓
GameController::GameController()
  ├─ ResourceManager::load() {textures, sounds}
  ├─ GameModel::GameModel() {initializes systems}
  ├─ GameRenderer::GameRenderer() {caches assets}
  └─ model_.addObserver(&renderer_) {register MVC link}
  
GameController::run()
  ├─ InitWindow(1800, 900, "GeoWorld")
  ├─ InitAudioDevice()
  ├─ SetTargetFPS(120)
  └─ Main loop (see Execution Flow above)
```

---

## Key Files Reference

| File | Purpose | Key Classes |
|------|---------|------------|
| `main.cpp` | Entry point | — |
| `GameController.h/cpp` | Application loop | `GameController` |
| `GameModel.h/cpp` | Game state | `GameModel` |
| `PhysicsSystem.h/cpp` | Movement math | `PhysicsSystem` |
| `CollisionSystem.h/cpp` | Hit detection | `CollisionSystem`, `HitEvent` |
| `GameRenderer.h/cpp` | Rendering | `GameRenderer` |
| `InputHandler.h/cpp` | Input polling | `InputSnapshot` |
| `SpaceshipController.h/cpp` | Spaceship input | `SpaceshipController` |
| `AstronautController.h/cpp` | Astronaut input | `AstronautController` |
| `IObserver.h` | Observer interface | `IObserver` |
| `ICollidable.h` | Collision contract | `ICollidable` |
| `BulletPool.h/cpp` | Bullet management | `BulletPool` |
| `EnemySpawner.h/cpp` | Enemy spawning | `EnemySpawner` |
| `AlienSpawner.h/cpp` | Alien spawning | `AlienSpawner` |
| `SessionStats.h/cpp` | Game progress | `SessionStats` |

---

## Summary

**GeoWorld is a masterclass in MVC + SOLID:**

1. **Model** owns all state and never touches rendering
2. **View** reads state and never mutates it
3. **Controller** translates input and never does physics/rendering
4. **Observer** pattern connects Model and View without coupling
5. **Dependency Inversion** makes systems independent and testable
6. **Single Responsibility** means each class has one reason to change

The two-phase gameplay (Spaceship → Astronaut) is managed cleanly through `GameModel::onSpaceship_` and phase-specific update methods. Physics, collisions, and spawning are all isolated systems that GameModel coordinates.

**Result:** Easy to extend (add new entity types, new weapons, new UI), easy to test (mock ICollidable, stub Model), and easy to understand (clear separation of concerns).
