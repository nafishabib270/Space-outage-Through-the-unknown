# Game Execution Flow Documentation

## Overview
This document traces the complete execution flow of the GeoWorld game, from startup to gameplay. It shows which functions call which other functions and where each function is located in the codebase.

---

## 1. STARTUP & INITIALIZATION

### 1.1 Entry Point: `main.cpp`

```
int main()                                  [main.cpp:1]
    ↓
    Creates GameController instance         [GameController.h]
    ↓
    Calls game.run()                        [GameController::run()]
```

**Location**: [main.cpp](main.cpp)

---

### 1.2 GameController Constructor

```
GameController::GameController()            [GameController.cpp:5]
    ↓
    1. ResourceManager()                    [ResourceManager.h]
    2. GameModel()                          [Models/Systems/GameModel.h]
       ├─ SessionStats()                    [Models/Systems/SessionStats.h]
       ├─ BulletPool()                      [Models/Systems/BulletPool.h]
       ├─ EnemySpawner()                    [Models/Systems/EnemySpawner.h]
       ├─ AlienSpawner()                    [Models/Systems/AlienSpawner.h]
       ├─ CollisionSystem()                 [Models/Systems/CollisionSystem.h]
       └─ PhysicsSystem()                   [Models/Systems/PhysicsSystem.h]
    ↓
    3. GameRenderer()                       [Views/Rendering/GameRenderer.h]
       ├─ HUD()                             [Views/UI/HUD.h]
       ├─ ThrusterEffect()                  [Views/Rendering/ThrusterEffect.h]
       └─ ThrusterEffect() (second)
    ↓
    4. MenuView()                           [Views/UI/MenuView.h]
    5. GameStateMachine()                   [Core/Interfaces/GameStateMachine.h]
    6. InputHandler()                       [Controllers/InputHandler.h]
    7. SpaceshipController()                [Controllers/SpaceshipController.h]
    8. AstronautController()                [Controllers/AstronautController.h]
    ↓
    9. InitWindow()                         [raylib]
    10. InitAudioDevice()                   [raylib]
    11. SetTargetFPS(120)                   [raylib]
    12. resources_.load()                   [Infrastructure/ResourceManager.cpp]
    13. PlayMusicStream()                   [raylib]
    ↓
    14. model_.addObserver(&renderer_)      [Models/Systems/GameModel.h]
        ├─ Registers renderer as observer
        └─ Observer pattern: Model notifies View after updates
```

**Locations**: 
- [GameController.cpp](Controllers/GameController.cpp)
- [GameModel.cpp](Models/Systems/GameModel.cpp)

---

## 2. MAIN GAME LOOP

### 2.1 GameController::run()

```
void GameController::run()                  [GameController.cpp:51]
    ↓
    while (!WindowShouldClose() && !isExit)
        ↓
        switch(stateMachine_.getState())
            ├─ GameState::Menu     → handleMenu()
            ├─ GameState::Playing  → handleGame()
            └─ GameState::Exit     → break
```

**Location**: [GameController.cpp](Controllers/GameController.cpp#L51)

---

## 3. MENU PHASE

### 3.1 GameController::handleMenu()

```
void GameController::handleMenu()           [GameController.cpp:35]
    ↓
    1. BeginDrawing()                       [raylib]
    ↓
    2. menuView_.render()                   [Views/UI/MenuView.cpp]
       └─ Renders menu screen
    ↓
    3. GameState next = 
       menuView_.handleInput()              [Views/UI/MenuView.cpp]
       ├─ Checks for key/mouse input
       └─ Returns next state (Playing or Exit)
    ↓
    4. EndDrawing()                         [raylib]
    ↓
    5. if (next == GameState::Playing)
       ├─ model_.reset()                    [Models/Systems/GameModel.cpp]
       │  └─ Resets all game state
       ├─ renderer_.resetThrusters()       [Views/Rendering/GameRenderer.cpp]
       ├─ hitSoundIdx_ = 0
       └─ stateMachine_.transitionTo(Playing)
    ↓
    6. if (next == GameState::Exit)
       └─ stateMachine_.transitionTo(Exit)
```

**Location**: [GameController.cpp](Controllers/GameController.cpp#L35)

---

## 4. GAME PHASE - MAIN LOOP

### 4.1 GameController::handleGame()

This is the core game loop that runs **120 times per second**.

```
void GameController::handleGame()           [GameController.cpp:45]
    ↓
    ┌─────────────────────────────────────────┐
    │ INPUT PHASE                             │
    └─────────────────────────────────────────┘
    ↓
    1. float dt = GetFrameTime()             [raylib - Delta Time]
    ↓
    2. UpdateMusicStream()                   [raylib]
    ↓
    3. InputSnapshot input = 
       inputHandler_.poll()                  [Controllers/InputHandler.cpp]
       ├─ IsKeyDown(KEY_W), IsKeyDown(KEY_A), etc.
       ├─ IsKeyDown(KEY_SPACE), IsKeyDown(KEY_ENTER)
       ├─ GetMousePosition()
       └─ Returns InputSnapshot struct with all current inputs
    ↓
    ┌─────────────────────────────────────────┐
    │ CONTROLLER DISPATCH PHASE               │
    └─────────────────────────────────────────┘
    ↓
    4. if (model_.isOnSpaceship() && 
          !model_.isCrashing())
       ├─ SpaceshipController hints =
       │  spaceshipCtrl_.handleInput()       [Controllers/SpaceshipController.cpp]
       │  ├─ Maps input to physics changes
       │  └─ Returns ThrusterHints (visual only)
       │
       └─ renderer_.updateThrusters()       [Views/Rendering/GameRenderer.cpp]
          └─ Updates visual thruster state
    ↓
    5. else if (!model_.isOnSpaceship())
       ├─ astronautCtrl_.handleInput()      [Controllers/AstronautController.cpp]
       │  └─ Maps input to astronaut movement
       │
       └─ renderer_.updateThrusters(false, false, false)
          └─ No thrusters for astronaut
    ↓
    ┌─────────────────────────────────────────┐
    │ MODEL UPDATE PHASE                      │
    └─────────────────────────────────────────┘
    ↓
    6. model_.update(dt)                     [Models/Systems/GameModel.cpp]
       (See section 5 below for detailed breakdown)
    ↓
    ┌─────────────────────────────────────────┐
    │ AUDIO PHASE                             │
    └─────────────────────────────────────────┘
    ↓
    7. playSoundForHits()                    [GameController.cpp]
       ├─ Gets last hit events from model
       ├─ Plays sound effects for collisions
       └─ Rotates through hit sound bank
    ↓
    ┌─────────────────────────────────────────┐
    │ RENDER PHASE                            │
    └─────────────────────────────────────────┘
    ↓
    8. BeginDrawing()                        [raylib]
    ↓
    9. renderer_.renderGame()                [Views/Rendering/GameRenderer.cpp]
       (See section 6 below for detailed breakdown)
    ↓
    10. EndDrawing()                         [raylib]
    ↓
    ┌─────────────────────────────────────────┐
    │ STATE TRANSITION PHASE                  │
    └─────────────────────────────────────────┘
    ↓
    11. if (model_.isGameOver())
        └─ stateMachine_.transitionTo(Menu)
```

**Location**: [GameController.cpp](Controllers/GameController.cpp#L45)

---

## 5. MODEL UPDATE PHASE - DETAILED

### 5.1 GameModel::update(float dt)

```
void GameModel::update(float dt)            [Models/Systems/GameModel.cpp:??]
    ↓
    if (isOnSpaceship())
        └─ updateSpaceshipPhase(dt)         [Models/Systems/GameModel.cpp]
           (See 5.2 below)
    ↓
    else
        └─ updateAstronautPhase(dt)         [Models/Systems/GameModel.cpp]
           (See 5.3 below)
    ↓
    notifyObservers()                       [Models/Systems/GameModel.cpp]
    └─ Calls renderer_.onModelChanged()     [Views/Rendering/GameRenderer.cpp]
```

**Location**: [Models/Systems/GameModel.cpp](Models/Systems/GameModel.cpp)

---

### 5.2 GameModel::updateSpaceshipPhase(float dt)

```
void GameModel::updateSpaceshipPhase(dt)    [Models/Systems/GameModel.cpp]
    ↓
    ┌─────────────────────────────────────────┐
    │ BULLET UPDATE                           │
    └─────────────────────────────────────────┘
    ↓
    1. bulletPool_.update(dt, SCREEN_W)      [Models/Systems/BulletPool.cpp]
       ├─ Updates bullet positions
       ├─ Removes out-of-bounds bullets
       └─ Manages cooldown timers
    ↓
    ┌─────────────────────────────────────────┐
    │ ENEMY SPAWNING & UPDATE                 │
    └─────────────────────────────────────────┘
    ↓
    2. enemySpawner_.update(dt, SCREEN_W, 
                            SCREEN_H, 
                            spaceshipPos_, 
                            score)             [Models/Systems/EnemySpawner.cpp]
       ├─ Spawns new HomingEnemy instances
       ├─ Updates all enemy positions
       ├─ Enemies home in on spaceship
       └─ Removes defeated enemies
    ↓
    3. auto enemies = 
       enemySpawner_.getCollidables()       [Models/Systems/EnemySpawner.cpp]
       └─ Returns vector of ICollidable*
    ↓
    ┌─────────────────────────────────────────┐
    │ COLLISION DETECTION - BULLETS vs ENEMIES│
    └─────────────────────────────────────────┘
    ↓
    4. lastHitEvents_ = 
       collisionSystem_.testBulletsVsEntities(
         bulletPool_.getBullets(),            [Models/Systems/CollisionSystem.cpp]
         enemies)
       ├─ Tests each bullet rect vs each enemy
       ├─ Applies damage to hit enemies
       ├─ Removes destroyed enemies
       ├─ Removes used bullets
       └─ Returns vector of HitEvent (for visual effects)
    ↓
    ┌─────────────────────────────────────────┐
    │ COLLISION DETECTION - ENEMIES vs PLAYER │
    └─────────────────────────────────────────┘
    ↓
    5. int dmg = 
       collisionSystem_.testEntitiesVsPlayer(
         enemies,                             [Models/Systems/CollisionSystem.cpp]
         spaceshipPos_,
         ENEMY_DAMAGE)
       ├─ Tests if enemy collides with spaceship
       ├─ Returns damage amount
       └─ Does NOT kill enemy (continuous damage)
    ↓
    6. if (dmg > 0)
       └─ stats_.damageSpaceship(dmg)       [Models/Systems/SessionStats.cpp]
          └─ Reduces spaceship health
    ↓
    ┌─────────────────────────────────────────┐
    │ CRASH PHYSICS (if destroyed)            │
    └─────────────────────────────────────────┘
    ↓
    7. if (stats_.spaceshipDestroyed())
       ├─ bool landed = 
       │  physics_.updateCrash(spaceshipPos_, [Models/Systems/PhysicsSystem.cpp]
       │                       dt, SCREEN_H)
       │  ├─ Applies gravity to falling spaceship
       │  ├─ Detects ground collision
       │  └─ Returns true when landed
       │
       └─ if (landed)
          └─ onSpaceship_ = false
             (Transitions to astronaut phase)
    ↓
    ┌─────────────────────────────────────────┐
    │ BACKGROUND SCROLLING                    │
    └─────────────────────────────────────────┘
    ↓
    8. scrollBackground(dt)                  [Models/Systems/GameModel.cpp]
       └─ Updates background position for parallax effect
```

**Location**: [Models/Systems/GameModel.cpp](Models/Systems/GameModel.cpp)

---

### 5.3 GameModel::updateAstronautPhase(float dt)

```
void GameModel::updateAstronautPhase(dt)    [Models/Systems/GameModel.cpp]
    ↓
    ┌─────────────────────────────────────────┐
    │ ALIEN SPAWNING & UPDATE                 │
    └─────────────────────────────────────────┘
    ↓
    1. alienSpawner_.update(dt, SCREEN_W,    [Models/Systems/AlienSpawner.cpp]
                           SCREEN_H)
       ├─ Spawns new GroundAlien instances
       ├─ Updates all alien positions
       ├─ Handles alien animation
       └─ Removes defeated aliens
    ↓
    2. auto aliens = 
       alienSpawner_.getCollidables()       [Models/Systems/AlienSpawner.cpp]
       └─ Returns vector of ICollidable*
    ↓
    ┌─────────────────────────────────────────┐
    │ COLLISION DETECTION - BULLETS vs ALIENS │
    └─────────────────────────────────────────┘
    ↓
    3. lastHitEvents_ = 
       collisionSystem_.testBulletsVsEntities(
         bulletPool_.getBullets(),            [Models/Systems/CollisionSystem.cpp]
         aliens)
       ├─ Tests each bullet rect vs each alien
       ├─ Applies damage to hit aliens
       ├─ Removes destroyed aliens
       ├─ Removes used bullets
       └─ Returns vector of HitEvent (for visual effects)
    ↓
    ┌─────────────────────────────────────────┐
    │ COLLISION DETECTION - ALIENS vs PLAYER  │
    └─────────────────────────────────────────┘
    ↓
    4. int dmg = 
       collisionSystem_.testEntitiesVsPlayer(
         aliens,                              [Models/Systems/CollisionSystem.cpp]
         astronautPos_,
         ALIEN_DAMAGE)
       ├─ Tests if alien collides with astronaut
       ├─ Returns damage amount
       └─ Does NOT kill alien
    ↓
    5. if (dmg > 0)
       └─ stats_.damageAstronaut(dmg)       [Models/Systems/SessionStats.cpp]
          └─ Reduces astronaut health
    ↓
    ┌─────────────────────────────────────────┐
    │ GAME OVER CHECK                         │
    └─────────────────────────────────────────┘
    ↓
    6. if (stats_.astronautDead())
       └─ Game transitions to Menu
          (Handled in GameController.handleGame())
    ↓
    ┌─────────────────────────────────────────┐
    │ BACKGROUND SCROLLING                    │
    └─────────────────────────────────────────┘
    ↓
    7. scrollBackground(dt)                  [Models/Systems/GameModel.cpp]
       └─ Updates background position for parallax effect
```

**Location**: [Models/Systems/GameModel.cpp](Models/Systems/GameModel.cpp)

---

## 6. VIEW RENDER PHASE - DETAILED

### 6.1 GameRenderer::renderGame()

```
void GameRenderer::renderGame() const       [Views/Rendering/GameRenderer.cpp]
    ↓
    1. ClearBackground(BLACK)               [raylib]
    ↓
    2. drawBackground()                     [Views/Rendering/GameRenderer.cpp]
       ├─ Draws background texture
       └─ Uses model_.getBgPos1() and getBgPos2()
    ↓
    3. if (model_.isOnSpaceship())
       ├─ drawSpaceship()                   [Views/Rendering/GameRenderer.cpp]
       │  ├─ Reads model_.getSpaceshipPos()
       │  ├─ Reads spaceship texture
       │  └─ Draws spaceship rectangle
       │
       └─ drawEnemies()                     [Views/Rendering/GameRenderer.cpp]
          ├─ Reads model_.getEnemySpawner()
          ├─ Iterates all enemies
          └─ Draws each enemy texture
    ↓
    4. else
       ├─ drawAstronaut()                   [Views/Rendering/GameRenderer.cpp]
       │  ├─ Reads model_.getAstronautPos()
       │  ├─ Reads astronaut texture
       │  └─ Draws astronaut rectangle
       │
       └─ drawAliens()                      [Views/Rendering/GameRenderer.cpp]
          ├─ Reads model_.getAlienSpawner()
          ├─ Iterates all aliens
          └─ Draws each alien texture
    ↓
    5. renderBullets()                      [Views/Rendering/GameRenderer.cpp]
       ├─ Reads model_.getBulletPool()
       ├─ Iterates all active bullets
       └─ Draws each bullet circle
    ↓
    6. renderHitEffects()                   [Views/Rendering/GameRenderer.cpp]
       ├─ Reads model_.getLastHitEvents()
       ├─ Draws visual feedback (sprites/particles)
       │  for collision damage
       └─ Clears effects after display
    ↓
    7. hud_.render()                        [Views/UI/HUD.cpp]
       ├─ Reads model_.getStats()
       ├─ Draws health bar
       ├─ Draws score text
       └─ Draws other UI elements
    ↓
    8. rearThrusters_.render()              [Views/Rendering/ThrusterEffect.cpp]
       └─ Renders rear thruster particle effect
    ↓
    9. sideThrusters_.render()              [Views/Rendering/ThrusterEffect.cpp]
       └─ Renders side thruster particle effect
```

**Location**: [Views/Rendering/GameRenderer.cpp](Views/Rendering/GameRenderer.cpp)

---

### 6.2 IObserver Pattern - Notification

```
After model_.update(dt) completes:
    ↓
    GameModel::notifyObservers()            [Models/Systems/GameModel.cpp]
    ↓
    for (IObserver* o : observers_)
        └─ o->onModelChanged()
           ↓
           GameRenderer::onModelChanged()   [Views/Rendering/GameRenderer.cpp]
           ├─ Updates animation frame indices
           ├─ Updates particle positions
           └─ Prepares visual effects for render phase
```

**Observer Pattern**: Model notifies View after updates, View reads updated state during render phase.

---

## 7. COLLISION & HIT EVENT FLOW

### 7.1 Collision Detection System

```
CollisionSystem::testBulletsVsEntities()    [Models/Systems/CollisionSystem.cpp]
    ↓
    for (auto& bullet : bullets)
        for (auto* entity : entities)
            ↓
            if (CheckCollisionRecs(bullet.rect, entity->getRect()))
                ├─ entity->takeDamage()     [Models/Entities/*.cpp]
                ├─ if (entity->isDead())
                │  └─ Remove from spawner
                ├─ hitEvent.pos = impact position
                └─ Add to lastHitEvents_
    ↓
    Return lastHitEvents_                   [std::vector<HitEvent>]
```

**Hit Events Flow**:
1. CollisionSystem detects collision → creates HitEvent
2. Model stores in `lastHitEvents_`
3. Controller retrieves and plays sound
4. Renderer reads and draws visual effect

---

## 8. PHYSICS SYSTEM

### 8.1 SpaceshipController Physics Application

```
SpaceshipController::handleInput()          [Controllers/SpaceshipController.cpp]
    ↓
    if (input.up)
        ├─ physics_.applyThrust()           [Models/Systems/PhysicsSystem.cpp]
        └─ hints.up = true
    ↓
    if (input.down)
        ├─ physics_.applyReverse()
        └─ hints.down = true
    ↓
    if (input.left)
        └─ physics_.applyLeftBoost()
    ↓
    if (input.right)
        └─ physics_.applyRightBoost()
    ↓
    if (input.fire)
        └─ bulletPool_.fire()               [Models/Systems/BulletPool.cpp]
           ├─ Checks cooldown
           ├─ Creates/reuses bullet
           └─ Sets initial velocity
    ↓
    physics_.update(spaceshipPos_, dt)      [Models/Systems/PhysicsSystem.cpp]
    └─ Updates position based on velocity/acceleration
```

**Location**: [Controllers/SpaceshipController.cpp](Controllers/SpaceshipController.cpp)

---

### 8.2 AstronautController Movement

```
AstronautController::handleInput()          [Controllers/AstronautController.cpp]
    ↓
    if (input.left)
        └─ Move astronautPos_.x left
    ↓
    if (input.right)
        └─ Move astronautPos_.x right
    ↓
    if (input.fire)
        └─ bulletPool_.fire()               [Models/Systems/BulletPool.cpp]
           ├─ Creates bullet
           └─ Sets upward trajectory
```

**Location**: [Controllers/AstronautController.cpp](Controllers/AstronautController.cpp)

---

## 9. COMPLETE FRAME SEQUENCE

```
FRAME N (1/120 of a second)
    ↓
    ┌─────────────────────────────────────────┐
    │ 1. INPUT (Controllers get snapshot)     │
    ├─────────────────────────────────────────┤
    │ InputHandler::poll()                    │
    │ → InputSnapshot with all current keys   │
    └─────────────────────────────────────────┘
    ↓
    ┌─────────────────────────────────────────┐
    │ 2. CONTROLLER DISPATCH (Mutate model)   │
    ├─────────────────────────────────────────┤
    │ SpaceshipController::handleInput()      │
    │   ├─ Updates physics_ state             │
    │   ├─ Calls bulletPool_.fire()           │
    │   └─ Returns thruster hints             │
    │                                         │
    │ AstronautController::handleInput()      │
    │   ├─ Updates astronautPos_              │
    │   └─ Calls bulletPool_.fire()           │
    └─────────────────────────────────────────┘
    ↓
    ┌─────────────────────────────────────────┐
    │ 3. MODEL UPDATE (Core game logic)       │
    ├─────────────────────────────────────────┤
    │ GameModel::update(dt)                   │
    │   ├─ updateSpaceshipPhase() OR          │
    │   │  updateAstronautPhase()             │
    │   │                                      │
    │   ├─ bulletPool_.update(dt)             │
    │   ├─ enemySpawner_.update()             │
    │   ├─ collisionSystem_.test*()           │
    │   ├─ physics_.update()                  │
    │   └─ scrollBackground(dt)               │
    │                                         │
    │   Then:                                 │
    │   └─ notifyObservers()                  │
    │      └─ renderer_.onModelChanged()      │
    └─────────────────────────────────────────┘
    ↓
    ┌─────────────────────────────────────────┐
    │ 4. AUDIO (Side effects from model)      │
    ├─────────────────────────────────────────┤
    │ playSoundForHits()                      │
    │ └─ PlaySound() for each collision       │
    └─────────────────────────────────────────┘
    ↓
    ┌─────────────────────────────────────────┐
    │ 5. RENDER (View reads model)            │
    ├─────────────────────────────────────────┤
    │ BeginDrawing()                          │
    │   renderer_.renderGame()                │
    │   ├─ drawBackground()                   │
    │   ├─ drawSpaceship() / drawAstronaut()  │
    │   ├─ drawEnemies() / drawAliens()       │
    │   ├─ renderBullets()                    │
    │   ├─ renderHitEffects()                 │
    │   ├─ hud_.render()                      │
    │   ├─ rearThrusters_.render()            │
    │   └─ sideThrusters_.render()            │
    │ EndDrawing()                            │
    └─────────────────────────────────────────┘
    ↓
    ┌─────────────────────────────────────────┐
    │ 6. STATE TRANSITION (Check game over)   │
    ├─────────────────────────────────────────┤
    │ if (model_.isGameOver())                │
    │   └─ stateMachine_.transitionTo(Menu)   │
    └─────────────────────────────────────────┘
    ↓
    REPEAT 120 times per second
```

---

## 10. KEY DESIGN PATTERNS

### 10.1 MVC Pattern

```
MODEL (GameModel)           CONTROLLER              VIEW (GameRenderer)
├─ Owns all state           ├─ Reads input          ├─ Reads state
├─ Runs physics             ├─ Mutates model        ├─ Never mutates
├─ Detects collisions       ├─ Calls update()       ├─ Calls render()
└─ Notifies observers       └─ Manages lifecycle    └─ Implements IObserver
```

### 10.2 Observer Pattern

```
GameModel (Subject)
├─ observers_ = [GameRenderer*]
└─ notifyObservers()
   └─ renderer_.onModelChanged()

GameRenderer (Observer)
└─ onModelChanged()
   └─ Updates visual effects
```

### 10.3 Object Pool Pattern

```
BulletPool
├─ bullets_[] (pre-allocated)
├─ update(dt) - moves active bullets
└─ fire() - reuses/activates bullets

EnemySpawner
├─ enemies_[] (std::vector of objects)
├─ update(dt) - spawns & updates
└─ Destruction is RAII (stack cleanup)
```

### 10.4 Strategy Pattern

```
spaceshipCtrl_.handleInput()
    ├─ Apply thrust physics
    └─ Update velocity

astronautCtrl_.handleInput()
    ├─ Direct position update
    └─ Different movement behavior
```

---

## 11. DATA FLOW SUMMARY

```
USER INPUT (Keyboard/Mouse)
    ↓
InputHandler::poll()        [raw raylib input]
    ↓
InputSnapshot              [clean booleans]
    ↓
SpaceshipController or     [maps to game logic]
AstronautController
    ↓
GameModel mutations        [state changes]
    ↓
GameModel::update(dt)      [physics, collisions]
    ↓
notifyObservers()          [model → view]
    ↓
GameRenderer              [reads state, renders]
    ↓
BeginDrawing/EndDrawing   [sends to GPU]
    ↓
DISPLAY ON SCREEN
```

---

## 12. FILE LOCATION QUICK REFERENCE

| Component | File Path |
|-----------|-----------|
| Entry Point | [main.cpp](main.cpp) |
| Root Controller | [Controllers/GameController.cpp](Controllers/GameController.cpp) |
| Root Model | [Models/Systems/GameModel.cpp](Models/Systems/GameModel.cpp) |
| Root View | [Views/Rendering/GameRenderer.cpp](Views/Rendering/GameRenderer.cpp) |
| Input Handler | [Controllers/InputHandler.cpp](Controllers/InputHandler.cpp) |
| Spaceship Controller | [Controllers/SpaceshipController.cpp](Controllers/SpaceshipController.cpp) |
| Astronaut Controller | [Controllers/AstronautController.cpp](Controllers/AstronautController.cpp) |
| Bullet System | [Models/Systems/BulletPool.cpp](Models/Systems/BulletPool.cpp) |
| Enemy Spawner | [Models/Systems/EnemySpawner.cpp](Models/Systems/EnemySpawner.cpp) |
| Alien Spawner | [Models/Systems/AlienSpawner.cpp](Models/Systems/AlienSpawner.cpp) |
| Physics System | [Models/Systems/PhysicsSystem.cpp](Models/Systems/PhysicsSystem.cpp) |
| Collision System | [Models/Systems/CollisionSystem.cpp](Models/Systems/CollisionSystem.cpp) |
| Session Stats | [Models/Systems/SessionStats.cpp](Models/Systems/SessionStats.cpp) |
| HUD/UI | [Views/UI/HUD.cpp](Views/UI/HUD.cpp) |
| Menu View | [Views/UI/MenuView.cpp](Views/UI/MenuView.cpp) |
| Thruster Effects | [Views/Rendering/ThrusterEffect.cpp](Views/Rendering/ThrusterEffect.cpp) |
| Resource Manager | [Infrastructure/ResourceManager.cpp](Infrastructure/ResourceManager.cpp) |
| Game State Machine | [Core/Interfaces/GameStateMachine.h](Core/Interfaces/GameStateMachine.h) |

---

## 13. STATE TRANSITIONS

```
STARTUP
    ↓
GameController created → Model/View initialized
    ↓
run() starts main loop
    ↓
┌─────────────────────────────────────────┐
│ MENU STATE                              │
├─────────────────────────────────────────┤
│ handleMenu()                            │
│ ├─ menuView_.render()                   │
│ ├─ menuView_.handleInput()              │
│ └─ Shows start menu                     │
│                                         │
│ User presses PLAY                       │
│ ├─ model_.reset()                       │
│ └─ transitionTo(Playing)                │
└─────────────────────────────────────────┘
    ↓
┌─────────────────────────────────────────┐
│ PLAYING STATE (Spaceship Phase)         │
├─────────────────────────────────────────┤
│ handleGame() loop                       │
│ ├─ updateSpaceshipPhase()               │
│ └─ Runs until spaceship destroyed       │
│                                         │
│ Spaceship crashes                       │
│ ├─ onSpaceship_ = false                 │
│ └─ Transitions to Astronaut Phase       │
└─────────────────────────────────────────┘
    ↓
┌─────────────────────────────────────────┐
│ PLAYING STATE (Astronaut Phase)         │
├─────────────────────────────────────────┤
│ handleGame() loop                       │
│ ├─ updateAstronautPhase()               │
│ └─ Runs until astronaut dies            │
│                                         │
│ Astronaut health → 0                    │
│ ├─ stats_.astronautDead() = true        │
│ └─ Triggers game over                   │
└─────────────────────────────────────────┘
    ↓
┌─────────────────────────────────────────┐
│ GAME OVER → MENU STATE                  │
├─────────────────────────────────────────┤
│ handleGame() detects isGameOver()       │
│ ├─ transitionTo(Menu)                   │
│ └─ Back to menu screen                  │
└─────────────────────────────────────────┘
    ↓
┌─────────────────────────────────────────┐
│ EXIT STATE                              │
├─────────────────────────────────────────┤
│ run() while condition fails             │
│ ├─ resources_.unload()                  │
│ ├─ CloseAudioDevice()                   │
│ ├─ CloseWindow()                        │
│ └─ return 0                             │
└─────────────────────────────────────────┘
```

---

## Summary

The game follows a classic **MVC + SOLID architecture**:

- **Model** (GameModel) owns all state and logic
- **Controllers** (GameController, InputHandler, SpaceshipController, AstronautController) mediate input and state changes
- **View** (GameRenderer) reads state and renders without mutation
- **120 FPS loop** processes: Input → Update → Render → State Check
- **Observer pattern** keeps View in sync with Model
- **Strict separation** ensures testability and extensibility
