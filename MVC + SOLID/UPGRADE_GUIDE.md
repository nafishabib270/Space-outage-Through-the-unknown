# Game Upgrade Guide: Adding Enemies & AI Improvements

## Table of Contents
1. [Upgrade Scope](#upgrade-scope)
2. [How MVC + SOLID Enables Easy Upgrades](#how-mvc--solid-enables-easy-upgrades)
3. [Adding New Enemy Types](#adding-new-enemy-types)
4. [Upgrading Enemy AI](#upgrading-enemy-ai)
5. [Practical Examples](#practical-examples)
6. [Impact Analysis](#impact-analysis)

---

## Upgrade Scope

### What Can Be Upgraded Without Changing Core Architecture?

#### ✅ Easy Upgrades (Isolated Changes)
1. **New Enemy Types**
   - Add `class SniperEnemy`, `class BomberEnemy`, `class ShieldedEnemy`, etc.
   - Each inherits from `ICollidable` independently
   - Zero changes to existing enemy classes

2. **Enemy AI Improvements**
   - Smarter targeting (predict player position)
   - Evasion patterns (dodge incoming bullets)
   - Group behavior (enemies working together)
   - Difficulty scaling (AI improves with level)

3. **Behavior Variations**
   - Different movement patterns
   - Custom attack strategies
   - Spawn behavior modifications
   - Detection range tuning

4. **Difficulty Progression**
   - Spawn rate increases
   - Enemy speed scaling
   - AI aggressiveness levels
   - New enemy introduction timing

5. **Visual Upgrades**
   - New sprites and animations
   - Visual effects (trails, glows)
   - Damage feedback (health bars, cracks)
   - No code changes required

#### ⚠️ Medium Complexity (Affecting Multiple Modules)
1. **New Game Mechanics**
   - Enemy spawning variations (waves, bosses)
   - New collision types (shield, reflect)
   - Complex animation systems

2. **Physics Modifications**
   - Gravity changes
   - New movement types
   - Environmental interactions

#### ❌ Major Changes (Require Architecture Review)
1. **Core Loop Changes** (frame rate, threading)
2. **Game Mode Overhauls** (entirely new phases)
3. **Multiplayer Implementation**

---

## How MVC + SOLID Enables Easy Upgrades

### The Problem Without MVC + SOLID

```cpp
// ❌ WITHOUT MVC + SOLID: All mixed together
class Game {
    void update() {
        // Input processing
        if (IsKeyPressed(KEY_W)) spaceship.y -= 5;
        
        // Entity logic
        enemy.x += enemy.velocityX;
        
        // Collision detection
        if (CheckCollision(spaceship.rect, enemy.rect)) {
            // Rendering state changes
            hitEffect.visible = true;
            hitEffect.x = enemy.x;
        }
        
        // Rendering
        DrawSprite(spaceship);
        DrawSprite(enemy);
        DrawSprite(hitEffect);
    }
};

// Want to add a new enemy?
// Problem: You must understand:
// - How input affects enemies (none, but hard to verify)
// - How collision detection works (it's scattered)
// - How rendering connects to logic (tightly coupled)
// - How to avoid breaking existing code (guess and test)
```

### The Solution: MVC + SOLID

```cpp
// ✅ WITH MVC + SOLID: Clear separation of concerns

// MODEL: Pure logic, no rendering or input
class BomberEnemy : public ICollidable {
    void update(float dt, Rectangle target) {
        // Only game logic here
        if (isAimedAtTarget(target)) {
            launchBomb();
        }
    }
};

// CONTROLLER: Input and orchestration only
class GameController {
    void update(float dt) {
        gameModel_.update(dt);           // Update logic
        renderer_.render(gameModel_);    // Render logic
    }
};

// VIEW: Rendering only
class GameRenderer {
    void render(const GameModel& model) {
        // Only read from model, never change it
        for (auto& enemy : model.getEnemies()) {
            drawEnemySprite(enemy);
        }
    }
};
```

### Why This Matters for Upgrades

| Aspect | Without MVC + SOLID | With MVC + SOLID |
|--------|-------------------|------------------|
| **Adding New Enemy** | Touch 5+ files, understand whole codebase | Touch 1 file (Models/Entities) |
| **Changing AI** | Risk breaking rendering, input, collision | Change only the AI logic |
| **Testing Logic** | Requires rendering engine, input handlers | Logic runs in isolation |
| **Bug Risk** | Changing one thing breaks 3 others | Change is isolated, predictable |
| **Code Reuse** | Difficult (all concerns mixed) | Natural (inherit from ICollidable) |
| **New Programmer** | Steep learning curve | Understands by folder names |

---

## Adding New Enemy Types

### Step-by-Step: Create a BomberEnemy

#### Step 1: Create the Header File
**File**: `Models/Entities/BomberEnemy.h`

```cpp
#pragma once
#include "../Core/Interfaces/ICollidable.h"
#include "raylib.h"

class BomberEnemy final : public ICollidable {
public:
    BomberEnemy();

    // Spawning
    void spawn(float x, float y, float speed);
    void update(float dt, Rectangle target);
    void despawn() { alive_ = false; }

    // ICollidable interface
    Rectangle getBounds()        const override { return rec_; }
    bool      isAlive()          const override { return alive_; }
    float     getHealth()        const override { return health_; }
    void      takeDamage(float a)      override;
    void      kill()                   override;

    // Bomber-specific: check if ready to drop bomb
    bool isReadyToDropBomb() const { return bombTimer_ <= 0; }
    Rectangle getBombBounds() const { return bombBounds_; }

private:
    Rectangle rec_{ 0, 0, 0, 0 };
    float health_{ 100.0f };
    float maxHealth_{ 100.0f };
    bool alive_{ false };
    float velocityX_{ 0 };
    float velocityY_{ 0 };

    // Bomber-specific AI
    float bombTimer_{ 3.0f };        // Drop bomb every 3 seconds
    float bombCooldown_{ 3.0f };
    float detectionRange_{ 800.0f };
    Rectangle bombBounds_{ 0, 0, 0, 0 };

    void updateBombingPattern(Rectangle target);
    void clampPosition();
};
```

#### Step 2: Implement the CPP File
**File**: `Models/Entities/BomberEnemy.cpp`

```cpp
#include "BomberEnemy.h"
#include <cmath>

BomberEnemy::BomberEnemy() {}

void BomberEnemy::spawn(float x, float y, float speed) {
    rec_.x = x;
    rec_.y = y;
    rec_.width = 32.0f;
    rec_.height = 32.0f;
    
    health_ = maxHealth_;
    alive_ = true;
    velocityX_ = speed;
    velocityY_ = 0;
    bombTimer_ = bombCooldown_;
}

void BomberEnemy::update(float dt, Rectangle target) {
    if (!alive_) return;

    // Simple horizontal movement
    rec_.x += velocityX_ * dt;

    // Update bombing pattern
    updateBombingPattern(target);

    // Decrement bomb timer
    if (bombTimer_ > 0) {
        bombTimer_ -= dt;
    }

    clampPosition();
}

void BomberEnemy::updateBombingPattern(Rectangle target) {
    // Drop bomb when directly above player and in detection range
    float distToPlayer = std::abs(rec_.x - (target.x + target.width / 2));
    
    if (distToPlayer < detectionRange_ && rec_.y < 300) {
        if (isReadyToDropBomb()) {
            // Set bomb position and ready flag
            bombBounds_ = {rec_.x + rec_.width / 2, rec_.y + rec_.height, 10.0f, 10.0f};
            bombTimer_ = bombCooldown_;
        }
    }
}

void BomberEnemy::takeDamage(float amount) {
    health_ -= amount;
    if (health_ <= 0) {
        kill();
    }
}

void BomberEnemy::kill() {
    alive_ = false;
}

void BomberEnemy::clampPosition() {
    // Keep within screen bounds (adjust as needed)
}
```

#### Step 3: Register with GameModel
**File**: `Models/Systems/GameModel.h`

Add to class:
```cpp
// Existing code...
std::vector<HomingEnemy>& getEnemies() { return enemySpawner_.getEnemies(); }

// NEW: Add bomber enemies
std::vector<BomberEnemy>& getBomberEnemies() { return bomberEnemies_; }
```

#### Step 4: Create a BomberSpawner (or Extend EnemySpawner)
**Option A: Extend existing spawner**

```cpp
// In EnemySpawner.h - add new spawning pool
std::vector<BomberEnemy> bomberPool_;
```

**Option B: Create separate BomberSpawner**
```cpp
// Models/Systems/BomberSpawner.h
class BomberSpawner {
public:
    void update(float dt, int screenW, int screenH, Rectangle target);
    std::vector<ICollidable*> getCollidables();
    const std::vector<BomberEnemy>& getBombers() const { return pool_; }

private:
    std::vector<BomberEnemy> pool_;
    float spawnTimer_{ 0 };
    float spawnInterval_{ 5.0f };  // Spawn every 5 seconds
};
```

#### Step 5: Update Collision System
**No Changes Needed!** ✅

The `CollisionSystem` already works with `ICollidable*` pointers:

```cpp
// In GameModel::update() - existing code already handles this
std::vector<ICollidable*> allEnemies;

// Add homing enemies
for (auto& enemy : enemySpawner_.getEnemies()) {
    if (enemy.isAlive()) {
        allEnemies.push_back(&enemy);
    }
}

// Add bomber enemies - SAME CODE PATTERN
for (auto& bomber : bomberSpawner_.getBombers()) {
    if (bomber.isAlive()) {
        allEnemies.push_back(&bomber);  // ICollidable*
    }
}

// Collision system doesn't care about type!
collisionSystem_.testBulletsVsEntities(bullets_, allEnemies);
```

#### Step 6: Update Renderer
**Minimal Changes**

```cpp
// In GameRenderer::render()

// Existing homing enemies rendering
for (auto& enemy : model_.getEnemies()) {
    if (enemy.isAlive()) {
        DrawRectangleRec(enemy.getBounds(), RED);
    }
}

// NEW: Add bomber enemies rendering
for (auto& bomber : model_.getBomberEnemies()) {
    if (bomber.isAlive()) {
        DrawRectangleRec(bomber.getBounds(), BLUE);  // Different color
    }
}
```

#### Result: Only 4 Files Changed
1. ✅ Created `BomberEnemy.h` 
2. ✅ Created `BomberEnemy.cpp`
3. ✅ Modified `GameModel` (minimal: add vector, getters)
4. ✅ Modified `GameRenderer` (minimal: add draw loop)

**Existing code touched: 0 lines broken** ✅

---

## Upgrading Enemy AI

### Current AI: HomingEnemy

```cpp
// Current: Turns and accelerates toward target
void HomingEnemy::updateTracking(Rectangle target) {
    float targetX = target.x + target.width / 2;
    
    // In detection range?
    if (std::abs(rec_.x - targetX) < detectionRange_) {
        tracking_ = true;
        
        // Simple: accelerate toward target
        if (rec_.x < targetX) {
            velocityX_ = speed_;
        } else {
            velocityX_ = -speed_;
        }
    }
}
```

### Upgrade 1: Predict Player Position

```cpp
// IMPROVED: Predict where player will be
void HomingEnemy::updateTracking(Rectangle target) {
    float targetX = target.x + target.width / 2;
    float targetVelocityX = target.x - lastTargetX_;  // Velocity estimation
    lastTargetX_ = target.x;
    
    // Predict position 0.5 seconds ahead
    float predictedX = targetX + targetVelocityX * 0.5f;
    
    if (std::abs(rec_.x - predictedX) < detectionRange_) {
        tracking_ = true;
        
        // Aim for predicted position, not current position
        if (rec_.x < predictedX) {
            velocityX_ = speed_ * 1.2f;  // Slightly faster to intercept
        } else {
            velocityX_ = -speed_ * 1.2f;
        }
    }
}
```

**Where to make this change:**
- File: `Models/Entities/HomingEnemy.cpp`
- Method: `updateTracking()`
- Affected files: 1
- Risk: Zero (fully contained in entity class)

### Upgrade 2: Evasion Behavior

```cpp
// NEW: Dodge incoming bullets
void HomingEnemy::updateEvasion(const std::vector<Bullet>& bullets) {
    // Check for nearby bullets
    for (const auto& bullet : bullets) {
        float distToBullet = std::abs(rec_.x - bullet.getBounds().x);
        
        if (distToBullet < 100.0f && bullet.isAlive()) {
            // Dodge upward or downward
            if (bullet.getVelocityX() > 0) {  // Coming from left
                velocityY_ = -speed_ * 0.5f;   // Move up
            } else {
                velocityY_ = speed_ * 0.5f;    // Move down
            }
        }
    }
}
```

**Call from GameModel:**
```cpp
void GameModel::update(float dt) {
    // Existing code...
    
    // NEW: Update evasion for all enemies
    for (auto& enemy : enemySpawner_.getEnemies()) {
        if (enemy.isAlive()) {
            enemy.updateEvasion(bulletPool_.getBullets());
        }
    }
}
```

### Upgrade 3: Group Behavior (Flocking)

```cpp
// NEW: Enemies coordinate attacks
class HomingEnemy {
    void updateFlocking(const std::vector<HomingEnemy>& allEnemies) {
        Vector2 cohesion{ 0, 0 };   // Tend toward group center
        Vector2 separation{ 0, 0 }; // Avoid crowding
        
        int nearby = 0;
        float groupX = rec_.x;
        float groupY = rec_.y;
        
        for (const auto& other : allEnemies) {
            if (&other == this || !other.isAlive()) continue;
            
            float dist = std::hypot(rec_.x - other.rec_.x, rec_.y - other.rec_.y);
            
            if (dist < 150.0f) {  // "Nearby" threshold
                nearby++;
                groupX += other.rec_.x;
                groupY += other.rec_.y;
                
                // Separation: push away if too close
                if (dist < 50.0f) {
                    separation.x += (rec_.x - other.rec_.x);
                    separation.y += (rec_.y - other.rec_.y);
                }
            }
        }
        
        // Blend group tendency with individual tracking
        if (nearby > 0) {
            float avgGroupX = groupX / nearby;
            float weight = 0.3f;  // 30% group tendency, 70% personal
            
            velocityX_ = velocityX_ * (1 - weight) + 
                        (avgGroupX - rec_.x) * weight;
        }
    }
};
```

### Upgrade 4: Difficulty Scaling

```cpp
// In EnemySpawner - scale AI difficulty with score
void EnemySpawner::update(float dt, int screenW, int screenH, 
                          Rectangle target, int playerScore) {
    // Existing spawn logic...
    
    // NEW: Scale difficulty based on score
    float difficultyMultiplier = 1.0f + (playerScore / 10000.0f);  // +10% per 10k points
    
    for (auto& enemy : pool_) {
        if (!enemy.isAlive()) continue;
        
        // Speed increases with difficulty
        float scaledSpeed = speed_ * difficultyMultiplier;
        
        // Detection range expands
        enemy.detectionRange_ = 1000.0f * difficultyMultiplier;
        
        // AI gets smarter (use more advanced tactics)
        if (difficultyMultiplier > 1.5f) {
            enemy.useAdvancedAI = true;  // Enable flocking, evasion
        }
    }
}
```

---

## Practical Examples

### Example 1: Add a "SniperEnemy" (Advanced Shot)

**File**: `Models/Entities/SniperEnemy.h`

```cpp
#pragma once
#include "../Core/Interfaces/ICollidable.h"

class SniperEnemy final : public ICollidable {
public:
    void spawn(float x, float y);
    void update(float dt, Rectangle target);
    
    bool isReadyToFire() const { return fireTimer_ <= 0; }
    Rectangle getProjectileBounds() const { return projectileBounds_; }

    // ICollidable interface
    Rectangle getBounds() const override { return rec_; }
    bool isAlive() const override { return alive_; }
    float getHealth() const override { return health_; }
    void takeDamage(float a) override;
    void kill() override;

private:
    Rectangle rec_{ 0, 0, 0, 0 };
    float health_{ 80.0f };
    bool alive_{ false };
    
    float fireTimer_{ 2.0f };      // Fire every 2 seconds
    float fireCooldown_{ 2.0f };
    Rectangle projectileBounds_{ 0, 0, 0, 0 };
    
    void aimAndFire(Rectangle target);
};
```

**Key Differences from HomingEnemy:**
- Stationary or slower movement
- Long-range fire attack (not chasing)
- Longer cooldown between shots
- Higher skill level (harder to defeat)

**Integration:**
```cpp
// GameModel.h
std::vector<SniperEnemy>& getSnipers() { return sniperSpawner_.getSniper(); }

// GameRenderer
for (auto& sniper : model_.getSnipers()) {
    DrawRectangleRec(sniper.getBounds(), YELLOW);
}
```

### Example 2: Add Wave-Based Spawning

**File**: `Models/Systems/WaveManager.h`

```cpp
#pragma once
#include "EnemySpawner.h"
#include "BomberSpawner.h"

class WaveManager {
public:
    void update(float dt);
    bool isWaveComplete() const;
    
    int getCurrentWave() const { return currentWave_; }
    float getWaveProgress() const;

private:
    int currentWave_{ 1 };
    float waveTimer_{ 0 };
    float waveDuration_{ 30.0f };  // 30 seconds per wave
    
    void spawnWavePattern();
    void nextWave();
};
```

**Update GameModel to use WaveManager:**
```cpp
// Models/Systems/GameModel.h
class GameModel {
    WaveManager waveManager_;
    
    void update(float dt) {
        waveManager_.update(dt);
        
        // Rest of update logic
    }
};
```

### Example 3: Upgrade AI to Track Multiple Targets

```cpp
// Models/Entities/HomingEnemy.h - add field
class HomingEnemy {
private:
    bool trackPlayer_{ true };
    Rectangle primaryTarget_{ 0, 0, 0, 0 };
    
    void selectTarget(const GameModel& model);
};

// Models/Entities/HomingEnemy.cpp
void HomingEnemy::selectTarget(const GameModel& model) {
    // Track astronaut instead of spaceship if available
    if (model.getGameState() == GameState::ASTRONAUT_PHASE) {
        primaryTarget_ = model.getAstronautBounds();
    } else {
        primaryTarget_ = model.getSpaceshipBounds();
    }
}
```

---

## Impact Analysis

### Why MVC + SOLID Makes Upgrades Safe

#### 1. **Single Responsibility Principle (SRP)**

Each class has ONE reason to change:

```
HomingEnemy.cpp
├── Changes only if: HomingEnemy behavior needs update
├── Never changes if: Rendering updates, input changes, collision formula changes
└── Result: Safe to modify without understanding rest of codebase

PhysicsSystem.cpp
├── Changes only if: Physics calculations need adjustment
├── Never changes if: New enemies added, collision rules change
└── Result: Can improve physics without testing entire game
```

#### 2. **Open/Closed Principle (OCP)**

Open for extension, closed for modification:

```
✅ GOOD: Add BomberEnemy (extends system)
   - No changes to existing classes
   - No risk of breaking HomingEnemy
   
❌ BAD: Modify HomingEnemy to also bomb
   - Must edit HomingEnemy.h/cpp
   - Existing code depends on HomingEnemy behavior
   - Risk of breaking existing gameplay
```

#### 3. **Liskov Substitution Principle (LSP)**

All enemies are interchangeable via ICollidable:

```cpp
// CollisionSystem never knows concrete enemy type
void CollisionSystem::testBulletsVsEntities(
    std::vector<Bullet>& bullets,
    std::vector<ICollidable*> entities) {  // Any ICollidable!
    
    for (auto* entity : entities) {
        entity->takeDamage(10);  // Works for any enemy type
    }
}

// Result: Add SniperEnemy, BomberEnemy, etc. → Zero changes to CollisionSystem
```

#### 4. **Interface Segregation Principle (ISP)**

Entities only implement interfaces they need:

```cpp
// Current
class HomingEnemy : public ICollidable { /* ... */ };

// Future - if we add destructible obstacles
class Obstacle : public ICollidable { /* ... */ };

// Future - if we add moving platforms
class Platform : public ICollidable, public IRenderable { /* ... */ };

// Each uses only what it needs
```

#### 5. **Dependency Inversion Principle (DIP)**

High-level systems depend on abstractions, not concrete types:

```cpp
// ✅ CollisionSystem depends on ICollidable (abstraction)
class CollisionSystem {
    std::vector<HitEvent> testBulletsVsEntities(
        std::vector<ICollidable*> entities);  // Abstraction
};

// Result: CollisionSystem unchanged when adding enemies
```

### Change Impact Matrix

| Change Type | Scope | Risk | Why MVC Helps |
|------------|-------|------|--------------|
| Add new enemy type | 2-4 files | ✅ Low | New classes isolated, ICollidable pattern handles polymorphism |
| Improve enemy AI | 1 file | ✅ Low | AI logic self-contained in entity class |
| Change difficulty curve | 1 file | ✅ Low | EnemySpawner/WaveManager isolated |
| Add visual effect | 2-3 files | ✅ Low | View changes don't affect Model |
| Modify collision rules | 1 file | ✅ Low | CollisionSystem is pure logic, no dependencies |
| Add new game phase | 3-5 files | ⚠️ Medium | Clear Controller/Model separation |
| Rewrite spawning system | 2-3 files | ⚠️ Medium | Systems are modular, can replace |

### Code Reusability

**Because of MVC + SOLID, you get instant reusability:**

```cpp
// HomingEnemy AI can be reused for:
class BomberEnemy : public ICollidable {
    void update(float dt, Rectangle target) {
        updateTracking(target);      // Reuse from HomingEnemy
        updateBombingPattern(target); // New behavior
    }
};

// CollisionSystem works for:
- HomingEnemy bullets
- GroundAlien bites
- BomberEnemy bombs
- SniperEnemy shots
- Future ShieldEnemy reflections
- All without modification
```

### Testing Benefits

**Isolate and test individual systems:**

```cpp
// Can test BomberEnemy AI without:
// - Rendering engine
// - Input system
// - Sound system
// - Full GameModel

TEST(BomberEnemyAI, DropsBomsWhenAbovePlayer) {
    BomberEnemy enemy;
    enemy.spawn(100, 100, 100);  // Isolated test
    
    Rectangle playerAt = {95, 500, 32, 32};  // Far below
    enemy.update(0.1f, playerAt);
    
    ASSERT_TRUE(enemy.isReadyToDropBomb());
}
```

---

## Quick Reference: Upgrade Checklist

### Adding a New Enemy Type

- [ ] Create `Models/Entities/NewEnemy.h` (inherit from ICollidable)
- [ ] Create `Models/Entities/NewEnemy.cpp` (implement interface)
- [ ] Create `Models/Systems/NewEnemySpawner.h` (or extend EnemySpawner)
- [ ] Add to `GameModel.h` (add vector, getter)
- [ ] Update `GameModel.cpp` (integrate into main update loop)
- [ ] Add to `GameRenderer.cpp` (add rendering loop)
- [ ] **No changes needed to**: CollisionSystem, PhysicsSystem, Controllers, InputHandler

### Improving Enemy AI

- [ ] Edit `Models/Entities/EnemyName.cpp` (existing AI method)
- [ ] Update behavior logic only
- [ ] Test by running game
- [ ] **No other files affected**

### Scaling Difficulty

- [ ] Edit `Models/Systems/EnemySpawner.cpp` or `WaveManager.cpp`
- [ ] Add score-based multipliers
- [ ] Adjust spawn timing and enemy stats
- [ ] **Single file modification**

---

## Summary

| Aspect | Traditional Monolithic | With MVC + SOLID |
|--------|----------------------|------------------|
| Add new enemy | Edit 8+ files, understand whole game | Create 2 files, follow pattern |
| Improve AI | Risk breaking rendering, collision | Change 1 isolated method |
| Find bugs | Could be in any of 30 files | Know exactly which module affected |
| Test new feature | Need full rendering, input, assets | Can test logic in isolation |
| Add new person to team | Takes weeks to understand code | Can follow folder structure in days |
| Reuse code | Difficult, everything intertwined | Natural through interfaces |

**The MVC + SOLID architecture transforms the game from a monolithic blob into building blocks. Each block is simple, tested independently, and combines with others predictably.**
