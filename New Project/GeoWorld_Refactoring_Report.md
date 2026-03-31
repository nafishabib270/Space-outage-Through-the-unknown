# GeoWorld: Refactoring Report (C → C++ OOP)

**Course:** Advanced Programming Lab  
**Project:** GeoWorld — Space Outage (raylib)

---

## Table of Contents

### Part A — AI Prompts Used
1. [Overview](#overview)
2. [Stage 1: Analysis & Planning Prompts](#stage-1-analysis--planning-prompts)
3. [Stage 2: OOP & Interface Design Prompts](#stage-2-oop--interface-design-prompts)
4. [Stage 3: Class Implementation Prompts](#stage-3-class-implementation-prompts)
5. [Stage 4: System Integration Prompts](#stage-4-system-integration-prompts)
6. [Stage 5: Documentation Prompts](#stage-5-documentation-prompts)
7. [Prompt Design Philosophy](#prompt-design-philosophy)
8. [Summary of Prompt Categories](#summary-of-prompt-categories)

### Part B — Detailed Explanation of Changes
9. [Executive Summary](#9-executive-summary)
10. [Why C++ Over C](#10-why-c-over-c)
11. [Design Model](#11-design-model)
12. [SOLID Principles Applied — C vs C++ Contrasted](#12-solid-principles-applied--c-vs-c-contrasted)
13. [Detailed Change Log (C → C++)](#13-detailed-change-log-c--c)
14. [What Was Improved & What Remains](#14-what-was-improved--what-remains)
15. [Lessons Learned](#15-lessons-learned)
16. [Appendix A: File Metrics Comparison](#appendix-a-file-metrics-comparison)
17. [Appendix B: Module Dependency Map](#appendix-b-module-dependency-map)

---

# PART A — AI PROMPTS USED

---

## Overview

GeoWorld — Space Outage is a 2D side-scrolling action game built with the raylib graphics library. The game has two distinct phases: a spaceship piloting phase where the player fights homing enemy drones in space, and a ground astronaut phase where the player — having crash-landed — fights on foot against both ground-level aliens and remaining enemy drones. The game features sprite sheet animation, particle thruster effects, directional mouse-aimed shooting, a live health bar, a score counter, a main menu, and looping background music with a round-robin hit sound pool.

The project went through two full transformations. The original C code (ver1) was a single `main.c` of approximately 550 lines, with six loosely designed supporting files (`enemy.c`, `alien1.c`, `bullet.c`, `booster.c`, `ui.c`, `game_state.c`) totalling around 700 lines of combined code. The C refactor (ver2 — C with SOLID) reorganised those seven files into thirteen focused modules, eliminated all ~30 global variables into `GameWorld` and `Resources` structs, unified six copy-pasted collision loops into one `collision_bullets_vs_entities()` function, fixed two confirmed bugs, and introduced an `EntityBase` common struct for LSP-compliant collision. The C++ OOP version (ver3 — current) redesigned the entire architecture using classes, abstract interfaces, inheritance, constructors, destructors, `std::vector`, and `std::unique_ptr`, enforcing every SOLID principle through the type system rather than through programmer discipline alone.

AI was used throughout the full transformation from ver1 to ver3. Analysis prompts inventoried every function, global, and struct. OOP design prompts designed the interface hierarchy. Class implementation prompts produced each `.h`/`.cpp` pair. Integration prompts wired the composition root. Documentation prompts generated this report. Each stage built directly on the previous one's output.

---

## Stage 1: Analysis & Planning Prompts

### Prompt 1.1 — Initial Codebase Audit

> "I have a C game project built with raylib called GeoWorld / Space Outage. It consists of 7 files: `main.c` (~550 lines), `enemy.c/.h`, `alien1.c/.h`, `bullet.c/.h`, `booster.c/.h`, `ui.c/.h`, and `game_state.c/.h`. For every function across all files, list its name, which file it lives in, its parameter types, return type, and estimated line count. For every global variable in `main.c`, list its name, type, and what it represents. For every struct across all header files, list all fields and describe what the type models. Then identify: (1) functions doing more than one job, (2) unused struct fields or parameters, (3) global state that could be encapsulated, (4) duplicated code blocks, (5) naming inconsistencies between files. Summarize the overall architecture in 3–5 sentences."

**Purpose:** Before designing a C++ architecture we needed a complete inventory of everything the C code was doing, hidden or explicit. Manually tracing ~25 functions, ~30 globals, and 6 struct types across 700 lines while identifying cross-file inconsistencies is error-prone and incomplete. AI produced the full inventory in one pass, giving a baseline that every subsequent design decision referenced.

**What we learned:**
- `main.c` had ~30 module-level global variables (`enemy_timer`, `alien_timer`, `bullet_now`, `score`, `spaceship_health`, `gravity`, `on_spaceship`, `hit_power`, `downfall_velocity`, etc.) with no encapsulating struct — every function in every file could read or overwrite any of them silently
- Collision detection was duplicated six times in `main.c`: bullets-vs-enemies (spaceship phase), bullets-vs-aliens (spaceship phase), bullets-vs-enemies (astronaut phase), bullets-vs-aliens (astronaut phase), enemies-vs-spaceship contact, enemies-vs-astronaut contact — six ~20-line loops with identical structure and only the array name changed
- `Alien1` and `Enemy` were completely separate structs with no shared base type, making any generic entity operation impossible without code duplication
- `DrawHealthBar()` was defined directly inside `main.c` instead of in `ui.c/.h`
- `srand(time(NULL))` was called inside `update_enemy()` on every frame — a side-effect buried inside a movement function that also produced wrong random values at >1Hz
- `alien_timer += dt` appeared twice on consecutive lines in the astronaut phase block — a silent double-speed spawn bug
- The `int once` flag, `float downfall_velocity`, and `int on_spaceship` transition variables were bare globals with no explanation of their lifecycle

### Prompt 1.2 — C++ OOP Opportunity Mapping

> "Given the Space Outage C codebase analysis, identify every concept that maps naturally to a C++ class. For each: (1) name the class; (2) list the C globals or struct fields it would own as private members; (3) list the C functions it would absorb as methods; (4) identify which other classes it depends on and how that dependency should be injected. Then identify which concepts should become pure abstract interfaces (zero data, all pure virtual) and which should be concrete classes. Explain why Bullet should NOT extend GameEntity even though it participates in collision."

**Purpose:** Moving from C to C++ is not a syntax translation — it is a design decision about ownership, lifetime, and contract. This prompt forced an explicit mapping between C artifacts (globals, loose functions, struct fields) and C++ concepts (private members, methods, interfaces, constructors) before writing any code.

**Key mappings identified:**
- ~30 globals in `main.c` → private members of `GameWorld`, `Spaceship`, `Astronaut`, `SessionStats`, `EnemySpawner`, `AlienSpawner`, and `BulletPool`
- Six copy-pasted collision loops → `CollisionSystem::testBulletsVsEntities()` accepting `std::vector<ICollidable*>` — one function, zero type knowledge
- `DrawHealthBar()` in `main.c` + `button()` in `ui.c` → methods of a `HUD` class receiving `const SessionStats&`
- All `LoadTexture` / `UnloadTexture` / `LoadSound` / `UnloadSound` calls → `ResourceManager` with RAII destructor
- `Enemy` and `Alien1` structs → `HomingEnemy` and `GroundAlien` classes extending abstract `GameEntity`
- `bullet` struct + pool globals → `Bullet` class (not extending `GameEntity` — it has no health, no spawn timer, and no AI; forcing it to inherit those would mean implementing methods it has no use for, violating ISP)

### Prompt 1.3 — Interface Hierarchy Design

> "Design the complete abstract interface hierarchy for the C++ version of Space Outage. The design must satisfy ISP — no class should be forced to implement methods it does not need. Show the four minimal interfaces (IDrawable, IUpdatable, ICollidable, ISpawnable), why they are separate instead of one combined IGameObject, which concrete classes implement which combination, and why Bullet implements ICollidable + IDrawable but not IUpdatable or ISpawnable. Then show the ISpawner interface and explain why GameWorld should hold std::unique_ptr<ISpawner> instead of a concrete EnemySpawner directly."

**Purpose:** Getting the interface hierarchy right before writing a single class is the most important decision in an OOP design. A mega-interface (`IGameObject` with `draw()`, `update()`, `spawn()`, `takeDamage()`, `getScore()`) forces every class to implement methods it does not use — the exact ISP violation. This prompt produced the four-interface design that the entire class hierarchy is built on.

---

## Stage 2: OOP & Interface Design Prompts

### Prompt 2.1 — Abstract Base Class Design

> "Design the `GameEntity` abstract base class for the C++ Space Outage project. It must: (1) inherit from ICollidable, IDrawable, IUpdatable, and ISpawnable; (2) hold only the fields that every entity shares — `rec_` (Rectangle), `health_`, `maxHealth_`, `alive_`, `velocityX_`, `velocityY_` — as protected members; (3) implement ICollidable and ISpawnable concretely (getBounds, isAlive, getHealth, takeDamage, kill, despawn, isActive) so subclasses don't repeat them; (4) declare draw(), update(), and spawn() as pure virtual so each subclass provides its own implementation. Explain why `Bullet` deliberately does NOT extend `GameEntity`, referencing ISP and the fields Bullet would inherit but never use."

**Purpose:** `GameEntity` is the LSP foundation — every concrete entity (`HomingEnemy`, `GroundAlien`) IS-A `GameEntity`, which means `CollisionSystem` can hold `ICollidable*` and call `takeDamage()` on either without knowing which type it has. The base must own only what is truly universal, or it becomes a bloated base that violates SRP.

**Design decisions made:**
- `health_`, `maxHealth_`, `alive_`, `rec_`, `velocityX_`, `velocityY_` are `protected` — subclasses read them directly but external code goes through the public interface
- `takeDamage()` is implemented in the base: subtracts from `health_`, calls `kill()` if it reaches zero, resets `health_` to `maxHealth_` for the next spawn cycle — this behavior is identical for every entity type, so it belongs in the base
- `draw()`, `update()`, and `spawn()` are pure virtual — `HomingEnemy::draw()` and `GroundAlien::draw()` render completely differently (one draws a static sprite, one animates a sprite sheet), so the base cannot provide a meaningful default
- `Bullet` implements only `ICollidable` and `IDrawable` — it would inherit `health_`, `maxHealth_`, `velocityY_`, `spawn()` semantics, and `despawn()` from `GameEntity`, none of which apply to a projectile

### Prompt 2.2 — CollisionSystem and Dependency Inversion

> "Design the `CollisionSystem` class for the C++ Space Outage project. It must: (1) depend only on `ICollidable` and `Bullet` abstractions — never on `HomingEnemy` or `GroundAlien` directly; (2) have `testBulletsVsEntities()` accept `std::vector<Bullet>&` and `std::vector<ICollidable*>` and handle both enemy and alien types through the interface; (3) receive `SessionStats&` through its constructor (not as a global); (4) draw the hit effect using a `Texture2D` passed through the constructor. Show how `HomingEnemy` and `GroundAlien` can both be passed as `ICollidable*` to the same function call, and explain why this is Liskov Substitution. Compare with the six copy-pasted loops in the original C code and explain what bug that duplication caused."

**Purpose:** `CollisionSystem` is the most important DIP demonstration in the project. In the original C code, `hit_power` and `score` were read directly as globals inside the collision loops — hidden dependencies with no indication in the function signature. `CollisionSystem` receives `SessionStats&` through its constructor, making the dependency explicit, compiler-enforced, and testable.

### Prompt 2.3 — ISpawner and the Composition Root Pattern

> "Design the `ISpawner` abstract interface and explain why `GameWorld` should hold spawners through this interface rather than through concrete `EnemySpawner*` pointers. Show: (1) the complete `ISpawner` interface with `update()`, `getCollidables()`, `reset()`, and `drawAll()`; (2) how `EnemySpawner` and `AlienSpawner` implement it; (3) how `GameWorld` stores them as `std::unique_ptr<EnemySpawner>` and accesses them as `ISpawner*`; (4) how adding a `BossSpawner` in the future requires zero changes to `GameWorld` or `CollisionSystem`; (5) the role of `GameLoop` as the composition root — the only file where concrete types are constructed and wired together."

**Purpose:** The composition root pattern is the practical implementation of DIP. Without a designated composition root, concrete type dependencies leak everywhere. `GameLoop` is the only file that imports `EnemySpawner.h`, `AlienSpawner.h`, and `ResourceManager.h` together — every other file sees only interfaces.

---

## Stage 3: Class Implementation Prompts

### Prompt 3.1 — HomingEnemy and GroundAlien Implementation

> "Implement `HomingEnemy` and `GroundAlien` as concrete classes extending `GameEntity` for the C++ Space Outage project. For `HomingEnemy`: the intercept trajectory calculation from `update_enemy()` in the original C code (computing `velocity_y` based on time-to-intercept and distance to target) must move into a private `computeIntercept()` method called from `update()`. The `srand(time(NULL))` call inside movement must be removed — seed only once at program start in `main()`. For `GroundAlien`: the sprite sheet animation (8 frames, 0.1s per frame) must move into `update()` with `frameTimer_` and `currentFrame_` as private members. Both classes are `final` — they are not designed to be subclassed further. Explain why marking them `final` is a LSP safeguard."

**Purpose:** The most dangerous behavior from the original C code — `srand(time(NULL))` being called 120 times per second inside movement logic — required explicit identification and removal. This prompt forced that fix as part of the class design rather than leaving it as a known bug.

**Key implementations produced:**
- `HomingEnemy::computeIntercept()` — private method, called once per entity when `!targetLocked_`, computes `velocityY_` from `dx / velocityX_` time estimate
- `GroundAlien::update()` — advances `frameTimer_`, increments `currentFrame_` modulo `FRAME_COUNT`, calls `despawn()` when off-screen or dead
- Both classes marked `final` — prevents accidental subclassing that could break LSP guarantees (a `ShieldedGroundAlien` that overrides `takeDamage()` without calling the base implementation would silently skip the health reset)

### Prompt 3.2 — ResourceManager with RAII

> "Implement `ResourceManager` for the C++ Space Outage project using RAII. It must: (1) be the ONLY class in the entire project that calls `LoadTexture`, `UnloadTexture`, `LoadMusicStream`, `UnloadMusicStream`, `LoadSound`, `UnloadSound`; (2) call `unload()` in its destructor so resources are freed automatically when `GameLoop` goes out of scope; (3) delete its copy constructor and copy assignment operator — resources are owned uniquely; (4) expose assets through const accessor methods that return by value (Texture2D, Sound are small raylib structs); (5) track a `loaded_` flag so the destructor is safe to call even if `load()` was never reached. Show the original C code where `UnloadTexture(spaceship)` was called twice by accident, and explain how RAII prevents this class of bug."

**Purpose:** In the original C code, `UnloadTexture(spaceship)` appeared twice at cleanup — a double-free that raylib handled gracefully but represented a real resource management bug. RAII means the destructor runs exactly once when `GameLoop` is destroyed, making the double-free structurally impossible.

### Prompt 3.3 — BulletPool Separation from Bullet

> "Implement `Bullet` and `BulletPool` as two separate classes. `Bullet` must implement `ICollidable` and `IDrawable` only — no pool mechanics, no cooldown, no slot tracking. `BulletPool` owns the `std::vector<Bullet>`, the round-robin slot counter, and the cooldown timer. Explain why separating these satisfies SRP: if the pool size changes, only `BulletPool` changes; if bullet rendering changes, only `Bullet` changes. Show how `Spaceship` and `Astronaut` both receive `BulletPool&` through their constructors (DIP) and call `fire()` without knowing how the pool is implemented. Compare with the original C code where `bullet_now` and `bullet_timer` were bare globals in `main.c` and bullet firing was inlined directly in the game loop."

**Purpose:** In the original C, firing a bullet required knowing `bullet_now`, `bullet_timer`, `bullets[]`, and `shoot_cooldown_timer` — four globals accessed directly in the game loop. `BulletPool` encapsulates all four. `Spaceship` and `Astronaut` call `bulletPool_.fire()` and `bulletPool_.canFire()` with no knowledge of how recycling works.

---

## Stage 4: System Integration Prompts

### Prompt 4.1 — GameWorld as Coordinator

> "Implement `GameWorld` for the C++ Space Outage project. It must: (1) own all gameplay systems as members — `SessionStats`, `BulletPool`, `Spaceship`, `Astronaut`, `std::unique_ptr<EnemySpawner>`, `std::unique_ptr<AlienSpawner>`, `CollisionSystem`, `HUD`; (2) wire them together in its constructor via member-initializer list, using ResourceManager& received as a constructor parameter (DIP); (3) split the game into two private phase handlers — `updateSpaceshipPhase()` and `updateAstronautPhase()` — so neither phase's update logic is visible in the public interface; (4) expose only `reset()`, `update(float dt)`, `draw()`, and `isGameOver()` publicly; (5) build the collidable lists for `CollisionSystem` by calling `getCollidables()` on each spawner through the `ISpawner` interface. Explain why `GameWorld` is an orchestrator, not a god class — it contains no physics math, no collision math, and no rendering code."

**Purpose:** `GameWorld` is the most complex class in the project because it holds the most members. The key architectural constraint is that it contains no logic — it delegates entirely. `updateSpaceshipPhase()` calls `spaceship_.update()`, `enemySpawner_->updateAll()`, `bulletPool_.update()`, `collisionSystem_.testBulletsVsEntities()`. It reads results and passes them on. It does not compute intercept trajectories, does not check individual bullet positions, and does not draw anything directly.

### Prompt 4.2 — GameLoop as State Machine and Composition Root

> "Implement `GameLoop` as the state machine and composition root for the C++ Space Outage project. It must: (1) own `ResourceManager`, `GameWorld`, and `MenuScreen` as members (not pointers — their lifetimes match `GameLoop`'s lifetime exactly); (2) call `InitWindow` and `InitAudioDevice` in its constructor and `CloseWindow` and `CloseAudioDevice` in its destructor; (3) implement `run()` as a `while (!WindowShouldClose())` loop with a `switch` on `GameState`; (4) be the ONLY class that imports both `ResourceManager.h` and `GameWorld.h` and `MenuScreen.h` — it is the only file where concrete types meet; (5) delete its copy constructor. Show how `main.cpp` becomes 5 lines: construct `GameLoop`, call `run()`, return 0. Compare with the original `main()` that was 550 lines."

**Purpose:** The original `main()` in C was 550 lines. The C++ `main()` is 5 lines. The reduction is not achieved by hiding code — it is achieved by assigning each piece of logic to the class that owns it. `GameLoop::run()` is ~30 lines. `GameWorld::update()` is ~20 lines. Each class's method is short because each class has one job.

---

## Stage 5: Documentation Prompts

### Prompt 5.1 — SOLID Contrast Documentation

> "Generate a documentation section contrasting the original C code and the final C++ OOP code for every SOLID principle. For each principle: (1) state the rule in one sentence; (2) show the concrete problem in the original C code with an actual code snippet from `main.c`; (3) show the C++ solution with actual class code; (4) explain what the C++ language feature (class, virtual, constructor injection, std::vector, etc.) does that C structs and function pointers cannot enforce. Use exact variable names, function names, and line references from the actual codebase."

**Purpose:** SOLID principles are sometimes taught as abstract rules. This prompt forced concrete, code-level evidence for every claim — the exact globals that violated DIP, the exact loops that violated OCP, the exact missing base type that forced the LSP violation in C. The result is verifiable: anyone can open the original C file and find the exact problems described.

### Prompt 5.2 — Language Benefit Analysis

> "Explain concretely how switching from C to C++ benefited the Space Outage project. Do not list generic C++ features. For each benefit, name the specific C problem it solves, the C++ mechanism that solves it, and show before/after code. Cover: (1) RAII and the double UnloadTexture bug; (2) constructors and the uninitialised entity problem (init_enemy had to be called separately from struct declaration); (3) private members and the global hit_power mutation; (4) virtual dispatch and the six copy-pasted collision loops; (5) std::vector and the magic array size numbers; (6) std::unique_ptr and the polymorphic ISpawner storage problem; (7) enum class and the MENU/GAME/EXIT name collision problem."

**Purpose:** A language migration is only justified if the new language solves concrete problems the old one could not. This prompt required evidence for every claim — no vague statements about C++ being "more modern" or "safer." Each benefit maps to a specific bug or structural problem in the original C code.

---

## Prompt Design Philosophy

The most important decision in every prompt was naming specific artifacts from the actual code. Generic prompts like "refactor this game to use OOP" produce generic results like "consider using classes" or "try encapsulation." Prompts that named `srand(time(NULL))` being called inside `update_enemy()`, `alien_timer += dt` appearing twice consecutively, `x_co` and `y_co` never being read, `DrawHealthBar()` being defined in `main.c` — those produced specific, actionable, verifiable answers.

The second key decision was separating interface design from implementation. We never asked "design and implement the CollisionSystem in one step." Interface design prompts produced pure header designs with rationale. Implementation prompts referenced those headers and asked for the `.cpp` bodies. This allowed rejection or modification of the design (for example, deciding `Bullet` would not extend `GameEntity`) before any implementation was written.

The third decision was to use the composition root pattern as a lens for every class design question. Any time a class seemed to need a dependency, the question was: "who constructs this class, and can the dependency be passed through the constructor?" If the answer was yes, the dependency went into the constructor. If the answer was no, the design was reconsidered. This discipline eliminated all global state from the project except for `GameState`, which was then also eliminated by making it a private member of `GameLoop`.

The fourth decision was to treat `final` as a design statement, not an optimization. Marking `HomingEnemy` and `GroundAlien` as `final` is a declaration that no further subclassing was intended — it prevents accidental LSP violations where a subclass overrides `takeDamage()` without calling the base implementation. AI initially omitted `final`; a correction prompt added it after reviewing the class hierarchy for subclassing risks.

---

## Summary of Prompt Categories

| Category | # Prompts | Primary Output Produced |
|----------|-----------|-------------------------|
| Analysis & Planning | 3 | Full C inventory, OOP mapping (globals → members, functions → methods), interface hierarchy design |
| OOP & Interface Design | 3 | Four interface definitions (IDrawable, IUpdatable, ICollidable, ISpawnable), ISpawner, GameEntity base class design, CollisionSystem DIP design |
| Class Implementation | 3 | HomingEnemy, GroundAlien, Bullet, BulletPool, ResourceManager, SessionStats, HUD, ThrusterEffect implementations |
| System Integration | 2 | GameWorld coordinator, GameLoop composition root, main.cpp reduction to 5 lines |
| Documentation | 2 | SOLID contrast with code snippets, language benefit analysis with before/after |
| **Total** | **13** | **Complete C → C++ transformation** |

---

# PART B — DETAILED EXPLANATION OF CHANGES

---

## 9. Executive Summary

GeoWorld — Space Outage is a two-phase 2D side-scrolling action game built with raylib. In Phase 1 the player pilots a spaceship shooting homing enemy drones while navigating vertically. When the spaceship health reaches zero it falls and crashes, transitioning to Phase 2 where the player controls an astronaut on foot — jumping, moving horizontally, and firing toward the mouse cursor — against both surviving drones and new ground-level aliens that walk in from the right.

The original C code (ver1) consisted of 7 files totalling approximately 700 lines. All meaningful game logic lived in a single `main()` function of approximately 550 lines: spaceship movement, astronaut physics, spawn timing, all six collision loops, bullet management, background scrolling, phase transition, and health bar drawing. ~30 bare global variables were the shared state that every function read and wrote without any indication in their signatures. Two confirmed bugs existed: `alien_timer += dt` incremented twice per frame (double alien spawn rate), and `srand(time(NULL))` was called 120 times per second inside enemy movement logic (producing wrong random values). Four unused struct fields (`x_co`, `y_co` in both `Enemy` and `Alien1`) and one unused function parameter (`alien_tex` in `init_alien()`) existed throughout.

The C++ OOP version (ver3 — current) is a 41-file project totalling approximately 1,200 lines. `main.cpp` is 5 lines. No collision logic is duplicated — `CollisionSystem::testBulletsVsEntities()` handles every entity type through `ICollidable*`. All state is private to the class that owns it — no globals exist. Resource loading and unloading is handled automatically by `ResourceManager`'s RAII destructor. Both confirmed bugs are fixed. Four abstract interfaces enforce contracts at compile time. Every public function and class carries a documentation comment.

The three most impactful improvements: (1) the four-interface hierarchy (`IDrawable`, `IUpdatable`, `ICollidable`, `ISpawnable`) that makes every class's contracts compiler-enforced rather than convention-enforced; (2) the `GameLoop` composition root that is the only file where concrete types are wired together, making every other class independently testable; (3) `ResourceManager` with RAII that makes the double-free bug structurally impossible.

---

## 10. Why C++ Over C

Switching from C to C++ for this project was justified by seven concrete problems in the C code that C++ solves structurally — not by language preference.

### 10.1 RAII Eliminates Resource Leaks

**C problem:** Every `LoadTexture()` needed a manually paired `UnloadTexture()`. In ver1, `UnloadTexture(spaceship)` was called twice at cleanup — a double-free that happened to be harmless in this raylib version but is a real bug class. If a function returned early (error handling added later, for example), assets leaked silently.

```c
/* ver1 main.c cleanup — UnloadTexture(spaceship) appears twice */
UnloadTexture(spaceship);
UnloadTexture(background);
UnloadTexture(spaceship);   /* BUG: duplicate unload */
UnloadTexture(enemy);
UnloadMusicStream(background_music);
```

**C++ solution:** `ResourceManager`'s destructor calls `unload()` exactly once when `GameLoop` goes out of scope. The double-free is structurally impossible — there is only one destructor call per object lifetime.

```cpp
// ResourceManager.cpp — destructor runs automatically, exactly once
ResourceManager::~ResourceManager() {
    unload();  // called by the compiler, not by the programmer
}

void ResourceManager::unload() {
    if (!loaded_) return;
    UnloadTexture(spaceship_);
    UnloadTexture(background_);
    // ... one call per asset, no duplicates possible
    loaded_ = false;
}
```

### 10.2 Constructors Guarantee Valid State

**C problem:** Initialisation was a separate `init_enemy()` call that the programmer had to remember to invoke after declaring the array. Forgetting it left structs in undefined state — `alive` could be 1, `health` could be garbage. The function signature gave no indication it was required before any other function.

```c
/* ver1 — declaration and initialisation are separate, forgettable steps */
Enemy enemies[10];
/* ... lines of other setup code ... */
init_enemy(enemies, 10, enemy_tex);  /* must remember this */
/* if forgotten: enemies[0].alive is undefined */
```

**C++ solution:** `HomingEnemy`'s constructor runs before any code can access the object. An uninitialised `HomingEnemy` cannot exist.

```cpp
// HomingEnemy.cpp — valid state is guaranteed at construction
HomingEnemy::HomingEnemy(Texture2D texture)
    : GameEntity(150.0f,
                 static_cast<float>(texture.width)  / 5.0f,
                 static_cast<float>(texture.height) / 5.0f),
      texture_(texture)
{}
// health_, alive_, rec_ are set by GameEntity's constructor.
// No separate init call. No undefined state possible.
```

### 10.3 Private Members Prevent Accidental Mutation

**C problem:** `hit_power` was a global `int` that any function in any file could write to. The collision loops in ver1 incremented it (`if (score % 10 == 0) hit_power += 10`) inside the inner loop, meaning it could increment multiple times in a single frame if multiple collisions happened simultaneously.

```c
/* ver1 main.c — hit_power is a global, anyone can write it */
int hit_power = 10;

/* Inside collision loop — can increment multiple times per frame */
if (enemies[j].health <= 0) {
    enemies[j].alive = 0, score++;
    if (score % 10 == 0) hit_power += 10;  /* called per-collision */
}
```

**C++ solution:** `SessionStats` keeps `hitPower_` private. The only mutation path is `registerKill()`, which increments `score_` first and then checks the modulo — one call per kill, correct behavior guaranteed.

```cpp
// SessionStats.cpp — one mutation path, correct logic enforced
void SessionStats::registerKill() {
    ++score_;
    if (score_ % 10 == 0) hitPower_ += 10;  // runs once per kill
}
// No external code can write hitPower_ directly.
```

### 10.4 Virtual Dispatch Replaces Copy-Pasted Loops

**C problem:** `Enemy` and `Alien1` had no shared base type. Six collision loops were copy-pasted because there was no way to write one function that accepted either type.

```c
/* ver1 — two of six nearly-identical loops */
/* Loop 1: bullets vs enemies */
for (int i = 0; i < 50; i++) {
    if (bullets[i].active == 1) {
        for (int j = 0; j < 10; j++) {
            if (enemies[j].alive == 1) {
                if (CheckCollisionRecs(enemies[j].rec, bullets[i].pos_rec)) {
                    enemies[j].health -= hit_power;
                    bullets[i].active = 0;
                    if (enemies[j].health <= 0) enemies[j].alive = 0, score++;
                }
            }
        }
    }
}
/* Loop 2: bullets vs aliens — identical structure, different array name */
for (int i = 0; i < 50; i++) {
    if (bullets[i].active == 1) {
        for (int j = 0; j < 10; j++) {
            if (aliens1[j].alive == 1) {
                if (CheckCollisionRecs(aliens1[j].rec, bullets[i].pos_rec)) {
                    aliens1[j].health -= hit_power;
                    bullets[i].active = 0;
                    if (aliens1[j].health <= 0) aliens1[j].alive = 0, score++;
                }
            }
        }
    }
}
/* ... four more identical loops ... */
```

**C++ solution:** `ICollidable` is the shared contract. `CollisionSystem::testBulletsVsEntities()` accepts `std::vector<ICollidable*>` and calls `getBounds()`, `takeDamage()`, and `isAlive()` through the interface — one function, zero type knowledge.

```cpp
// CollisionSystem.cpp — one function, handles every entity type
void CollisionSystem::testBulletsVsEntities(
    std::vector<Bullet>&      bullets,
    std::vector<ICollidable*> entities,
    std::vector<Sound>&       hitSounds,
    int&                      soundIndex)
{
    for (auto& bullet : bullets) {
        if (!bullet.isActive()) continue;
        for (auto* entity : entities) {
            if (!entity->isAlive()) continue;
            if (!CheckCollisionRecs(entity->getBounds(), bullet.getBounds())) continue;
            entity->takeDamage(static_cast<float>(stats_.getHitPower()));
            bullet.deactivate();
            if (!entity->isAlive()) stats_.registerKill();
            break;
        }
    }
}
```

### 10.5 `std::vector` Eliminates Magic Array Sizes

**C problem:** Pool sizes were magic numbers (`10`, `20`, `50`, `100`) scattered through every loop. The number `20` meant "alien pool size" in one loop and "collision damage" in another. Changing `MAX_ENEMIES` from 10 to 15 required finding every `for (int i = 0; i < 10; i++)` in every file and judging whether that `10` referred to enemies.

```c
/* ver1 — magic numbers with dual meanings */
Enemy enemies[10];         /* 10 = MAX_ENEMIES */
Alien1 aliens1[20];        /* 20 = MAX_ALIENS */
bullet bullets[50];        /* 50 = MAX_BULLETS */

for (int i = 0; i < 10; i++) { /* is this MAX_ENEMIES or damage? */
    enemies[i].alive = 0;
}
```

**C++ solution:** `std::vector` sized at construction. Pool sizes are constants passed once to the constructor; no loop in any other class hardcodes them.

```cpp
// BulletPool.cpp — size is a constructor parameter, not a magic number
BulletPool::BulletPool(int poolSize, Texture2D bulletTexture)
    : pool_(poolSize)          // std::vector<Bullet> sized here
{
    for (auto& b : pool_) b.setTexture(bulletTexture);
}

// GameWorld.cpp — sizes live in named constants, one place
BulletPool bulletPool_{ 50, res.getBullet() };  // MAX_BULLETS = 50
```

### 10.6 `std::unique_ptr` Enables Polymorphic Ownership

**C problem:** C cannot store a collection of polymorphic objects without storing void pointers or function-pointer tables. Holding an array of `EnemySpawner` and `AlienSpawner` as a common type required manual vtable emulation — error-prone and verbose.

**C++ solution:** `std::unique_ptr<EnemySpawner>` and `std::unique_ptr<AlienSpawner>` are stored and accessed through `ISpawner&`. Adding a `BossSpawner` is adding one `make_unique<BossSpawner>(...)` call in `GameLoop` — no existing class changes.

```cpp
// GameWorld.h — polymorphic ownership with automatic lifetime management
std::unique_ptr<EnemySpawner> enemySpawner_;
std::unique_ptr<AlienSpawner> alienSpawner_;

// GameWorld.cpp — accessed through ISpawner interface
enemySpawner_->update(dt, SCREEN_W, SCREEN_H);  // ISpawner::update()
auto enemies = enemySpawner_->getCollidables();  // ISpawner::getCollidables()
```

### 10.7 `enum class` Prevents Name Collisions

**C problem:** `enum { MENU, GAME, EXIT }` leaked `MENU`, `GAME`, and `EXIT` into the global namespace. A variable named `game` elsewhere would conflict. Implicit conversion to `int` allowed nonsensical operations like `current_state + 1`.

```c
/* ver1 — unscoped enum, values leak into global namespace */
typedef enum Game_state { MENU, GAME, EXIT } GameState;
GameState current_state = MENU;
/* MENU, GAME, EXIT are now global identifiers */
```

**C++ solution:** `enum class GameState` scopes the values. `GameState::Menu` is unambiguous and cannot be added to an integer by accident.

```cpp
// GameState.h — scoped enum, no global namespace pollution
enum class GameState {
    Menu,
    Playing,
    Exit
};
// Usage: GameState::Menu — cannot conflict with any other name
// Cannot do: GameState::Menu + 1 — compiler error
```

---

## 11. Design Model

The C++ project is organized into four layers. Each layer may only depend on layers below it — no upward dependencies exist.

```
┌───────────────────────────────────────────────────────────────┐
│  ENTRY POINT                                                   │
│  main.cpp — 5 lines: construct GameLoop, call run()           │
└─────────────────────────┬─────────────────────────────────────┘
                          │ owns
┌─────────────────────────▼─────────────────────────────────────┐
│  ORCHESTRATION                                                  │
│  GameLoop — state machine (Menu/Playing/Exit), composition root│
│  GameWorld — coordinates all systems, owns all game state      │
│  MenuScreen — menu rendering and button input                  │
└──────┬──────────────────┬──────────────────────┬──────────────┘
       │                  │                       │
┌──────▼──────┐  ┌────────▼────────┐  ┌──────────▼────────────┐
│  CONTROLLERS │  │  ENTITY SYSTEMS  │  │  SUPPORT SYSTEMS       │
│  Spaceship   │  │  HomingEnemy     │  │  CollisionSystem        │
│  Astronaut   │  │  GroundAlien     │  │  SessionStats           │
│  BulletPool  │  │  EnemySpawner    │  │  HUD                    │
│  Bullet      │  │  AlienSpawner    │  │  ResourceManager        │
│              │  │  ThrusterEffect  │  │                         │
└──────┬───────┘  └────────┬─────────┘  └──────────┬────────────┘
       │                   │                        │
       └───────────────────┴────────────────────────┘
                           │ all depend on
          ┌────────────────▼────────────────────────┐
          │  SHARED CONTRACTS (interfaces)            │
          │  IDrawable / IUpdatable                   │
          │  ICollidable / ISpawnable / ISpawner      │
          │  GameEntity (abstract base)               │
          │  GameState (enum class)                   │
          └──────────────────────────────────────────┘
```

**Layer access rules:**
- `CollisionSystem` must not import `HomingEnemy.h` or `GroundAlien.h` — it works through `ICollidable*` only
- `EnemySpawner` and `AlienSpawner` must not call collision functions — they manage timing and call `spawn()` only
- `Spaceship` and `Astronaut` must not reference enemy or alien state — they know only their own `Rectangle` and `BulletPool&`
- `ResourceManager` is the only class that calls any raylib Load/Unload function
- `GameLoop` is the only file that imports both `ResourceManager.h` and `GameWorld.h` together

---

## 12. SOLID Principles Applied — C vs C++ Contrasted

---

### 12.1 Single Responsibility Principle

**Rule:** Every class or module should have exactly one reason to change.

**C problem — `main.c` did everything simultaneously:**

```c
/* ver1 main.c — health bar rendering defined inside the entry point */
void DrawHealthBar(int x, int y, int width, int height,
                   int health, int maxHealth) {
    float ratio = (float)health / maxHealth;
    DrawRectangle(x, y, width, height, GRAY);
    Color healthColor = GREEN;
    if (health < 60 && health >= 30) healthColor = YELLOW;
    else if (health < 30)            healthColor = RED;
    DrawRectangle(x, y, (int)(width * ratio), height, healthColor);
    DrawRectangleLines(x, y, width, height, BLACK);
}

/* In the same file, in the same loop — spawn timing: */
enemy_timer += dt;
if (enemy_timer >= enemy_time_limit) {
    SpawnEnemy(&enemies[enemy_now], screenWidth, screenHeight * .8, enemy_speed);
    enemy_now++;
    if (enemy_now >= 10) {
        enemy_now = 0;
        enemy_speed += 2;
        if (enemy_time_limit > 0.5) enemy_time_limit -= 0.1f;
    }
    enemy_timer = 0;
}
/* Then spaceship input. Then bullet firing. Then collision x6. Then audio. */
/* One function. Every concern. */
```

**C++ solution — one class, one job:**

```cpp
// HUD.cpp — rendering is its ONLY job. Changes when UI design changes.
void HUD::drawHealthBar(int health, int maxHealth) const {
    float ratio  = static_cast<float>(health) / maxHealth;
    Color bar    = (health < 30) ? RED : (health < 60) ? YELLOW : GREEN;
    DrawRectangle(25, 25, 300, 50, GRAY);
    DrawRectangle(25, 25, static_cast<int>(300 * ratio), 50, bar);
    DrawRectangleLines(25, 25, 300, 50, BLACK);
}

// EnemySpawner.cpp — spawn timing is its ONLY job. Changes when difficulty changes.
void EnemySpawner::update(float dt, int screenW, int screenH) {
    timer_ += dt;
    if (timer_ < timeLimit_) return;
    if (!pool_[slot_].isAlive())
        pool_[slot_].spawn(screenW + 10.0f,
            static_cast<float>(rand() % static_cast<int>(screenH * 0.8f)), speed_);
    timer_ = 0;
    slot_  = (slot_ + 1) % static_cast<int>(pool_.size());
    if (slot_ == 0) { speed_ += 2.0f; if (timeLimit_ > minTimeLimit_) timeLimit_ -= 0.1f; }
}

// SessionStats.cpp — score and health tracking is its ONLY job.
void SessionStats::registerKill() {
    ++score_;
    if (score_ % 10 == 0) hitPower_ += 10;
}
```

If the health bar colour scheme changes: open `HUD.cpp` only. If spawn difficulty changes: open `EnemySpawner.cpp` only. If scoring rules change: open `SessionStats.cpp` only. In the original C, all three changes required editing `main.c` and risking every other concern in that 550-line file.

---

### 12.2 Open/Closed Principle

**Rule:** Open for extension, closed for modification.

**C problem — adding Alien required modifying `main.c` and copy-pasting collision:**

```c
/* ver1 — adding alien collision meant copy-pasting this 20-line block */
/* and substituting aliens1[] for enemies[] — main.c was modified */
for (int i = 0; i < 50; i++) {
    if (bullets[i].active == 1) {
        for (int j = 0; j < 10; j++) {
            if (enemies[j].alive == 1) {
                if (CheckCollisionRecs(enemies[j].rec, bullets[i].pos_rec)) {
                    enemies[j].health -= hit_power;
                    enemies[j].velocity_x -= 1;
                    bullets[i].active = 0;
                    if (enemies[j].health <= 0) enemies[j].alive = 0, score++;
                    if (score % 10 == 0) hit_power += 10;
                }
            }
        }
    }
}
/* Alien version: change enemies[] → aliens1[], .pos_rec → .pos_rec — identical */
/* This is modification of working code every time a new entity is added */
```

**C++ solution — adding `ShieldedEnemy` requires zero changes to existing files:**

```cpp
// ShieldedEnemy.h — new file only, nothing existing is touched
class ShieldedEnemy final : public GameEntity {
public:
    explicit ShieldedEnemy(Texture2D texture);
    void spawn(float x, float y, float speed) override;
    void update(float dt)                      override;
    void draw()                          const override;
    // Override takeDamage to add shield logic:
    void takeDamage(float amount)              override;
private:
    float shieldHealth_{ 50.0f };
    Texture2D texture_;
};

// GameLoop.cpp — one new line, no existing line changed:
// shieldSpawner_ = std::make_unique<ShieldedEnemySpawner>(res.getShieldEnemy(), 5, 15.0f, 2.0f);

// CollisionSystem.cpp — NOT TOUCHED.
// EnemySpawner.cpp   — NOT TOUCHED.
// HomingEnemy.cpp    — NOT TOUCHED.
```

---

### 12.3 Liskov Substitution Principle

**Rule:** A derived type must be substitutable for its base type without the calling code knowing which concrete type it has.

**C problem — no shared type, collision had to branch per type:**

```c
/* ver1 — Enemy and Alien1 are completely separate. No shared base. */
typedef struct {
    int x_co, y_co;      /* unused */
    float health;
    float velocity_x;
    float velocity_y;
    Rectangle rec;
    int alive;
    int tar_locked;
} Enemy;

typedef struct {
    int x_co, y_co;      /* unused */
    float health;
    float velocity_x;
    Rectangle rec;
    int alive;
    float frame_timer;
    int cur_frame;
} Alien1;
/* Same fields. Different types. No shared base. Six loops required. */
```

**C++ solution — `ICollidable` makes both types substitutable:**

```cpp
// ICollidable.h — shared contract enforced by the compiler
class ICollidable {
public:
    virtual Rectangle getBounds()        const = 0;
    virtual bool      isAlive()          const = 0;
    virtual float     getHealth()        const = 0;
    virtual void      takeDamage(float amount) = 0;
    virtual void      kill()                   = 0;
    virtual ~ICollidable() = default;
};

// GameEntity.h — implements ICollidable concretely for all subclasses
class GameEntity : public ICollidable, public IDrawable,
                   public IUpdatable,  public ISpawnable {
protected:
    Rectangle rec_;
    float     health_;
    bool      alive_;
    /* ... */
};

// HomingEnemy IS-A GameEntity IS-A ICollidable
// GroundAlien IS-A GameEntity IS-A ICollidable
// Both substitute for ICollidable* in CollisionSystem with correct behavior.

// GameWorld.cpp — mixed vector, one function, zero type checks
std::vector<ICollidable*> GameWorld::getAllCollidables() {
    auto v = enemySpawner_->getCollidables();  // HomingEnemy* cast to ICollidable*
    auto a = alienSpawner_->getCollidables();  // GroundAlien* cast to ICollidable*
    v.insert(v.end(), a.begin(), a.end());
    return v;  // mixed — CollisionSystem doesn't know or care
}
```

---

### 12.4 Interface Segregation Principle

**Rule:** No class should be forced to implement methods it does not need.

**C problem — all concerns bundled in one header, all callers dragged in all dependencies:**

```c
/* ver1 enemy.h — spawn, update, AND draw in one interface */
/* Any file including this also pulls in Texture2D draw dependencies */
/* even if it only needed health data for collision */
void init_enemy  (Enemy[], int, Texture2D);   /* Texture2D not needed by collision */
void SpawnEnemy  (Enemy*, int, int, int);
void update_enemy(Enemy[], Rectangle, int, int, float);
void draw_enemy  (Enemy[], int, Texture2D);   /* Texture2D not needed by spawner */

/* ver1 booster.h — function name revealed implementation details */
void only_update_boosters(Particle side_boosters[]);
/* "only update" — caller must know internal distinction between emit+update vs update-only */
```

**C++ solution — four minimal interfaces, each consumed independently:**

```cpp
// IDrawable.h — one method, for things that render
class IDrawable  { public: virtual void draw() const = 0; virtual ~IDrawable() = default; };

// IUpdatable.h — one method, for things that move or think
class IUpdatable { public: virtual void update(float dt) = 0; virtual ~IUpdatable() = default; };

// ICollidable.h — for things that participate in hit tests only
class ICollidable {
public:
    virtual Rectangle getBounds()        const = 0;
    virtual bool      isAlive()          const = 0;
    virtual void      takeDamage(float amount) = 0;
    virtual void      kill()                   = 0;
    virtual ~ICollidable() = default;
};

// ISpawnable.h — for things that are spawned and despawned
class ISpawnable {
public:
    virtual void spawn(float x, float y, float speed) = 0;
    virtual void despawn()     = 0;
    virtual bool isActive() const = 0;
    virtual ~ISpawnable() = default;
};

// ThrusterEffect: IDrawable + IUpdatable only. No health. Not spawned. Not collidable.
// SessionStats:   None of the game interfaces. It is a plain data manager.
// Bullet:         ICollidable + IDrawable only. No spawn timer. No AI. Not IUpdatable.

// ThrusterEffect intentional naming vs. original:
void ThrusterEffect::emitRear   (Rectangle ship);   /* explicit intent */
void ThrusterEffect::emitSideUp (Rectangle ship);   /* explicit intent */
void ThrusterEffect::emitSideDown(Rectangle ship);  /* explicit intent */
void ThrusterEffect::coast       ();                /* explicit intent */
/* vs. original "only_update_boosters" — caller had to know internal distinction */
```

---

### 12.5 Dependency Inversion Principle

**Rule:** High-level modules must not depend on low-level modules. Both must depend on abstractions.

**C problem — ~30 globals, `srand()` hidden inside movement, testing impossible:**

```c
/* ver1 main.c — partial list of 30 globals every function silently depended on */
float enemy_timer      = 0;
int   enemy_now        = 0;
int   enemy_speed;
float bullet_timer     = .2;
int   bullet_now       = 0;
float enemy_time_limit = 1;
int   hit_power        = 10;
int   score            = 0;
int   spaceship_health = 100;
float gravity          = 50;
float downfall_velocity = 0;
int   on_spaceship     = 1;
int   once             = 1;
/* ... 17 more ... */

/* ver1 enemy.c — side-effect buried inside movement logic */
void update_enemy(Enemy enemies[], Rectangle char_pos,
                  int cnt, float del_t) {
    for (int i = 0; i < cnt; i++) {
        if (enemies[i].alive == 1) {
            if (enemies[i].tar_locked == 0) {
                srand(time(NULL));  /* BUG: reseeds RNG every frame at 120fps */
                /* produces the same value every frame — not random at all */
            }
        }
    }
}
/* To test update_enemy(): must initialise all 30 globals first.
   Cannot test in isolation. */
```

**C++ solution — everything flows through constructors, no globals, `srand` called once:**

```cpp
// main.cpp — srand called ONCE, at program start
int main() {
    srand(static_cast<unsigned>(time(nullptr)));  // one call, correct randomness
    GameLoop game;
    game.run();
    return 0;
}

// GameLoop.cpp — the composition root: only place concrete types are wired together
GameLoop::GameLoop()
    : world_(resources_)       // GameWorld receives ResourceManager& — not globals
    , menu_(SCREEN_W, SCREEN_H)
{
    InitWindow(SCREEN_W, SCREEN_H, "GeoWorld");
    InitAudioDevice();
    resources_.load();
    PlayMusicStream(resources_.getBGM());
}

// GameWorld.cpp — all dependencies explicit in constructor, none from globals
GameWorld::GameWorld(ResourceManager& res)
    : bulletPool_(50, res.getBullet())
    , spaceship_(res.getSpaceship(), bulletPool_, SCREEN_W, SCREEN_H)
    , astronaut_(res.getAstronaut(), bulletPool_, SCREEN_W)
    , collisionSystem_(stats_, res.getHitEffect())  // SessionStats& injected
    , hud_(SCREEN_W, SCREEN_H)
{ ... }

// CollisionSystem — depends on ICollidable* (abstraction), not HomingEnemy* (concretion)
// To unit-test CollisionSystem: construct a SessionStats, pass a fake Texture2D.
// No game loop needed. No globals to initialise.
```

---

## 13. Detailed Change Log (C → C++)

### 13.1 Structural Changes

| Component | Original C | C++ OOP | Change |
|-----------|-----------|---------|--------|
| Total source files | 7 | 41 | +34 |
| `main()` / entry point lines | ~550 | 5 | −545 |
| Module-level globals | ~30 | 0 | −30 |
| Collision loop blocks | 6 (copy-pasted) | 1 (unified method) | −5 |
| Abstract interfaces | 0 | 5 (IDrawable, IUpdatable, ICollidable, ISpawnable, ISpawner) | +5 |
| Confirmed bugs fixed | — | 2 (double alien_timer, srand in loop) | −2 |
| Unused struct fields | 4 | 0 | −4 |
| Unused function parameters | 1 | 0 | −1 |
| RAII resource management | Manual | Automatic (destructor) | fixed |
| Testable classes | 0 | 8+ | +8 |
| Magic number constants | 0 | 20+ named constants | +20 |
| Documentation | 0 comments | All public classes + methods | added |

### 13.2 Class-to-C Mapping

| Original C artifact | C++ Class / Interface | Notes |
|--------------------|-----------------------|-------|
| `enemy.c/.h` + `Enemy` struct | `HomingEnemy : GameEntity` | `x_co`, `y_co` removed; `srand` bug fixed; `final` |
| `alien1.c/.h` + `Alien1` struct | `GroundAlien : GameEntity` | Renamed from `Alien1`; `x_co`, `y_co` removed; `final` |
| `bullet.c/.h` + `bullet` struct | `Bullet`, `BulletPool` | Split: `Bullet` = state only; `BulletPool` = pool management |
| `booster.c/.h` + `Particle` struct | `ThrusterEffect`, `ThrusterParticle` | Renamed; `only_update_boosters` → `coast()` |
| `ui.c/.h` `button()` | `HUD::ui_button()`, `MenuScreen` | `DrawHealthBar` moved from `main.c` to `HUD` |
| `game_state.c/.h` `GameState` enum | `GameState` enum class | Scoped; `STATE_MENU` → `GameState::Menu` |
| `main.c` `score`, `hit_power`, `spaceship_health` globals | `SessionStats` | Private members, mutation through methods only |
| `main.c` spawn timer globals | `EnemySpawner`, `AlienSpawner` | `double alien_timer` bug fixed; `ISpawner` interface |
| `main.c` spaceship input block (~60 lines) | `Spaceship` class | `handleInput()`, `updateCrash()` private methods |
| `main.c` astronaut block (~50 lines) | `Astronaut` class | Jump, gravity, animation, aimed shooting |
| `main.c` 6 collision loops | `CollisionSystem` | Depends on `ICollidable*` only |
| `main.c` all `LoadTexture` calls | `ResourceManager` | RAII destructor; double-free impossible |
| `main.c` game loop | `GameLoop` | Composition root; `switch` on `GameState` |
| `main.c` + all modules | `GameWorld` | Coordinator; no physics or rendering math |
| *(new)* | `IDrawable`, `IUpdatable`, `ICollidable`, `ISpawnable`, `ISpawner` | Enforced contracts; compiler-checked |
| *(new)* | `GameEntity` | Abstract base class; LSP foundation |

### 13.3 Interface Implementation Matrix

| Class | IDrawable | IUpdatable | ICollidable | ISpawnable | ISpawner |
|-------|-----------|------------|-------------|------------|----------|
| `HomingEnemy` | ✓ | ✓ | ✓ | ✓ | — |
| `GroundAlien` | ✓ | ✓ | ✓ | ✓ | — |
| `Bullet` | ✓ | — | ✓ | — | — |
| `ThrusterEffect` | ✓ | ✓ | — | — | — |
| `Spaceship` | ✓ | ✓ | — | — | — |
| `Astronaut` | ✓ | ✓ | — | — | — |
| `EnemySpawner` | — | — | — | — | ✓ |
| `AlienSpawner` | — | — | — | — | ✓ |
| `SessionStats` | — | — | — | — | — |
| `HUD` | — | — | — | — | — |
| `ResourceManager` | — | — | — | — | — |

The blank cells are the point: `SessionStats` does not implement `IDrawable` because it has nothing to draw. `HUD` does not implement `IUpdatable` because it does not move. `ResourceManager` implements nothing — it is a pure asset owner. ISP means every blank cell represents a dependency that does not exist.

---

## 14. What Was Improved & What Remains

### 14.1 Improvements Achieved

| Area | Original C | C++ OOP |
|------|-----------|---------|
| Entry point size | ~550 lines of mixed logic | 5 lines |
| Global variables | ~30 bare globals in `main.c` | 0 globals; all state private to owning class |
| Collision detection | 6 copy-pasted ~20-line loops | 1 generic `testBulletsVsEntities()` method |
| Spawn timing | Inline timer globals in `main.c` | `EnemySpawner` + `AlienSpawner` classes |
| Spaceship physics | ~60 inline lines in `main.c` | `Spaceship` class — `handleInput()` + `updateCrash()` |
| Astronaut physics | ~50 inline lines in `main.c` | `Astronaut` class — 4 private focused methods |
| Double alien_timer bug | `alien_timer += dt` twice per frame | Fixed: `AlienSpawner::timer_` incremented once in `update()` |
| `srand` in movement loop | Called 120 times/sec inside `update_enemy()` | Called once in `main()` before `GameLoop` construction |
| Resource management | Manual paired Load/Unload, double-free bug | `ResourceManager` RAII destructor |
| Contracts | Convention-only (comments at best) | Compiler-enforced abstract interfaces |
| Testability | Cannot test anything without full game loop | Every class testable by constructing it with injected dependencies |
| Unused struct fields | `x_co`, `y_co` in `Enemy` and `Alien1` | Removed — not present in `HomingEnemy` or `GroundAlien` |
| Unused parameters | `alien_tex` in `init_alien()` | Removed — `GroundAlien` constructor takes `Texture2D` directly |
| Naming | 3+ conventions mixed (`SpawnEnemy`, `Spawn_alien`, `UpdateBoosters`, `update_side_boosters`) | PascalCase classes, `camelCase_` private members, `camelCase()` methods throughout |
| Health bar location | `DrawHealthBar()` defined in `main.c` | `HUD::drawHealthBar()` — one place, one class |

### 14.2 What Still Needs Work

| Area | Current State | Needed Change | Priority |
|------|--------------|---------------|----------|
| Phase 2 background | Same space background in astronaut phase | Load and swap to ground map texture when `!onSpaceship_` | High |
| Level 2 | Empty `else {}` block in original; not implemented in C++ | Design and implement second level in `GameWorld` | High |
| Astronaut health display | `HUD::render()` shows correct health per phase; needs visual distinction | Add astronaut portrait or icon to differentiate from spaceship bar | Medium |
| Game over screen | Returns directly to menu on astronaut death | Add `GameState::GameOver`, `GameOverScreen` class | Medium |
| Score persistence | `SessionStats::reset()` clears score on new game | Write high score to file in `ResourceManager` or a new `SaveSystem` | Low |
| Sound pitch variance | 20 identical hit sounds loaded (memory waste) | Load 1 sound, use `SetSoundPitch()` with random variance | Low |
| Crash sequence slide | Ship should slide left after crash landing | Add x-drift in `Spaceship::updateCrash()` with `SCROLL_SPEED` | Medium |
| Unit tests | No test framework set up | Add Catch2 or doctest; `SessionStats`, `CollisionSystem`, `BulletPool` are immediately testable | Low |

---

## 15. Lessons Learned

### 15.1 About Designing C++ Architecture for Games

Interfaces must be designed before classes. The single most important architectural decision in this project was the four-interface hierarchy (`IDrawable`, `IUpdatable`, `ICollidable`, `ISpawnable`). That decision determined which methods every class must implement, which classes could be stored polymorphically, and how `CollisionSystem` could be written without knowing about `HomingEnemy` or `GroundAlien`. Designing classes before interfaces produces mega-classes with merged responsibilities that are later painful to split.

The composition root pattern prevents dependency leakage. Without a designated composition root, concrete type dependencies spread through every file. `GameLoop` being the only file that imports `ResourceManager.h`, `GameWorld.h`, and `MenuScreen.h` together is not an accident — it is a design constraint enforced by reviewing every include chain. Any class that imports a concrete type it should not know about reveals a DIP violation.

`final` is a design statement. Marking `HomingEnemy` and `GroundAlien` as `final` communicates that no subclassing was intended and prevents accidental LSP violations where a future subclass overrides `takeDamage()` without calling the base implementation (which resets `health_` and sets `alive_ = false`). Every leaf class in a game entity hierarchy should be reviewed for whether `final` is appropriate.

Polymorphic containers require pointer semantics. Storing `HomingEnemy` and `GroundAlien` in the same `std::vector` by value causes object slicing — the derived part is lost and the wrong `draw()` is called. This is a subtle C++ pitfall that does not exist in C (where you store concrete types separately by design). `std::vector<ICollidable*>` or `std::vector<std::unique_ptr<GameEntity>>` are the correct approaches depending on ownership semantics.

### 15.2 About the C-to-C++ Language Transition

Not every C concept has a direct C++ equivalent. `EntityBase` being the first member of a C struct (enabling safe casting via C layout rules) was a clever C technique for LSP simulation. In C++ this pattern is replaced by inheritance and virtual dispatch — but the C technique informed understanding of what LSP means structurally, making the C++ interface hierarchy easier to design correctly.

C++ constructors catch entire classes of C bugs. The `init_alien()` call-after-declaration pattern in C was not just an inconvenience — it was a class of bug. Any code path that constructed an entity array but reached an early return before calling `init_*()` left entities in undefined state. C++ constructors make this entire class of bug structurally impossible: the object does not exist until the constructor completes successfully.

`std::vector` changes the conversation about pool sizes. In C, changing `MAX_ENEMIES` from 10 to 15 required finding and auditing every `for (int i = 0; i < 10; i++)` loop to determine whether the `10` was `MAX_ENEMIES` or some other coincidentally-equal value. `std::vector` sized at construction means the size is named once (in `GameLoop`) and `pool_.size()` is used everywhere else — there is no loop bound to update.

### 15.3 About Using AI for C++ Refactoring

Asking AI to identify what NOT to inherit is as important as asking what to inherit. The question "why should `Bullet` not extend `GameEntity`?" produced the ISP analysis that shaped the entire interface hierarchy — `Bullet` has no health, no spawn timer, no AI state, no `velocityY_`, and forcing it to inherit those from `GameEntity` would mean implementing `spawn()` and `despawn()` in a way that had no meaningful semantics for a projectile. That single question clarified the boundary between `GameEntity`'s domain and `Bullet`'s domain.

Naming the specific C bug before asking for the C++ fix produces better code. "Fix the `srand(time(NULL))` being called inside `update_enemy()` at 120fps" produced `srand()` moved to `main()` with a clear explanation of why it needs to run exactly once. "Improve the random number generation" would have produced a suggestion to use `<random>` — technically better but not the root cause fix.

The composition root pattern must be explicitly specified in a prompt, or AI will distribute wiring across multiple classes. Initial generated code had `GameWorld` constructing `ResourceManager` internally — this violated DIP because `GameWorld` would then own asset loading, not just gameplay. A correction prompt ("GameWorld must receive ResourceManager& through its constructor, not construct it internally") produced the correct design where `GameLoop` is the single composition root.

---

## Appendix A: File Metrics Comparison

| Metric | Original C | C++ OOP | Change |
|--------|-----------|---------|--------|
| `.c` / `.cpp` source files | 7 | 17 | +10 |
| `.h` header files | 6 | 24 | +18 |
| Total files | 13 | 41 | +28 |
| Total lines (code + docs) | ~700 | ~1,200 | +500 |
| `main()` executable lines | ~550 | 5 | −545 |
| Module-level globals | ~30 | 0 | −30 |
| Abstract interfaces | 0 | 5 | +5 |
| Abstract base classes | 0 | 1 (`GameEntity`) | +1 |
| Concrete entity classes | 2 (`Enemy`, `Alien1`) | 2 (`HomingEnemy`, `GroundAlien`) | refactored |
| Collision loop blocks | 6 (copy-pasted) | 1 (method) | −5 |
| Confirmed bugs | 2 | 0 | −2 |
| Unused struct fields | 4 | 0 | −4 |
| Unused function parameters | 1 | 0 | −1 |
| Named constants | 0 | 20+ | +20 |
| RAII resource management | No | Yes | added |
| Testable in isolation | 0 classes | 8+ classes | +8 |
| SOLID principles enforced by language | 0 | 5 | +5 |

---

## Appendix B: Module Dependency Map

### B.1 Include Dependencies

Each class may only include sideways or downward. No class includes a file that depends on it.

| File | Includes | Notes |
|------|----------|-------|
| `main.cpp` | `GameLoop.h` | Entry point only — 5 lines |
| `GameLoop.h/.cpp` | `ResourceManager.h`, `GameWorld.h`, `MenuScreen.h`, `GameState.h` | Composition root — only file that imports concrete top-level types together |
| `GameWorld.h/.cpp` | `ResourceManager.h`, `SessionStats.h`, `BulletPool.h`, `Spaceship.h`, `Astronaut.h`, `EnemySpawner.h`, `AlienSpawner.h`, `CollisionSystem.h`, `HUD.h`, `ISpawner.h` | Coordinator — no raylib calls except `DrawTexturePro` for background |
| `CollisionSystem.h/.cpp` | `ICollidable.h`, `Bullet.h`, `SessionStats.h`, `raylib.h` | No `HomingEnemy.h` or `GroundAlien.h` — uses `ICollidable*` only |
| `EnemySpawner.h/.cpp` | `ISpawner.h`, `HomingEnemy.h`, `raylib.h` | Needs concrete type to fill pool |
| `AlienSpawner.h/.cpp` | `ISpawner.h`, `GroundAlien.h`, `raylib.h` | Needs concrete type to fill pool |
| `HomingEnemy.h/.cpp` | `GameEntity.h`, `raylib.h` | `final` class |
| `GroundAlien.h/.cpp` | `GameEntity.h`, `raylib.h` | `final` class |
| `GameEntity.h/.cpp` | `ICollidable.h`, `IDrawable.h`, `IUpdatable.h`, `ISpawnable.h`, `raylib.h` | Abstract base only |
| `Spaceship.h/.cpp` | `IDrawable.h`, `IUpdatable.h`, `ThrusterEffect.h`, `BulletPool.h`, `raylib.h` | No entity dependency |
| `Astronaut.h/.cpp` | `IDrawable.h`, `IUpdatable.h`, `BulletPool.h`, `raylib.h` | No entity dependency |
| `BulletPool.h/.cpp` | `Bullet.h`, `raylib.h` | Owns pool and cooldown |
| `Bullet.h/.cpp` | `ICollidable.h`, `IDrawable.h`, `raylib.h` | No pool knowledge |
| `ThrusterEffect.h/.cpp` | `IDrawable.h`, `IUpdatable.h`, `raylib.h` | No entity or game state dependency |
| `SessionStats.h/.cpp` | *(none)* | Zero raylib dependency — pure data |
| `HUD.h/.cpp` | `SessionStats.h`, `raylib.h` | Depends on stats abstraction, not raw ints |
| `ResourceManager.h/.cpp` | `raylib.h` | Only class calling Load/Unload functions |
| `MenuScreen.h/.cpp` | `GameState.h`, `SessionStats.h`, `raylib.h` | Depends on stats for score display |
| `IDrawable.h` | *(none)* | Pure interface |
| `IUpdatable.h` | *(none)* | Pure interface |
| `ICollidable.h` | `raylib.h` | Pure interface — needs Rectangle |
| `ISpawnable.h` | *(none)* | Pure interface |
| `ISpawner.h` | `ICollidable.h` | Returns `std::vector<ICollidable*>` |
| `GameState.h` | *(none)* | `enum class` only |

**Circular dependency check:** `CollisionSystem` does not include `HomingEnemy.h` or `GroundAlien.h`. `EnemySpawner` does not include `CollisionSystem.h`. `SessionStats` has zero raylib dependency. `Spaceship` and `Astronaut` do not include any entity headers. No circular includes exist in the project.

### B.2 raylib Function Usage by Class

| raylib Function | Used In (C++ OOP) |
|----------------|-------------------|
| `DrawTexturePro` | `HomingEnemy::draw()`, `GroundAlien::draw()`, `Bullet::draw()`, `Astronaut::draw()`, `Spaceship::draw()`, `GameWorld::scrollBackground()` |
| `DrawRectangle`, `DrawRectangleLines`, `DrawRectangleRec` | `HUD::drawHealthBar()`, `MenuScreen::drawButton()` |
| `DrawText`, `MeasureText`, `TextFormat` | `HUD::drawScore()`, `MenuScreen::draw()` |
| `DrawCircleV` | `ThrusterEffect::draw()` |
| `CheckCollisionRecs` | `CollisionSystem::testBulletsVsEntities()`, `CollisionSystem::testEntitiesVsPlayer()` |
| `CheckCollisionPointRec` | `MenuScreen::drawButton()` |
| `GetMousePosition` | `Astronaut::handleShooting()` |
| `IsMouseButtonDown`, `IsMouseButtonPressed` | `Astronaut::handleShooting()`, `Spaceship::handleInput()`, `MenuScreen::update()` |
| `IsKeyDown`, `IsKeyPressed` | `Spaceship::handleInput()`, `Astronaut::handleMovement()`, `Astronaut::handleJump()`, `CollisionSystem::testEntitiesVsPlayer()` (god-mode) |
| `GetRandomValue` | `ThrusterEffect::emit()`, `EnemySpawner::update()`, `AlienSpawner::randomiseNextSpawn()` |
| `PlaySound`, `UpdateMusicStream`, `PlayMusicStream` | `CollisionSystem::testBulletsVsEntities()` (hit sound), `GameLoop::run()` (music), `Spaceship::handleInput()` (laser) |
| `BeginDrawing`, `EndDrawing`, `ClearBackground` | `GameLoop::handleMenu()`, `GameLoop::handleGame()` |
| `InitWindow`, `CloseWindow`, `SetTargetFPS`, `WindowShouldClose` | `GameLoop::GameLoop()`, `GameLoop::run()` |
| `InitAudioDevice`, `CloseAudioDevice` | `GameLoop::GameLoop()`, `GameLoop::run()` |
| `LoadTexture`, `UnloadTexture`, `LoadMusicStream`, `LoadSound`, `UnloadMusicStream`, `UnloadSound` | `ResourceManager::load()`, `ResourceManager::unload()` — **nowhere else** |

**Note:** Zero raylib calls appear in `SessionStats.cpp` or `GameState.h`. `CollisionSystem` uses only `CheckCollisionRecs`, `DrawTexturePro` (hit effect), and `PlaySound` — no spawn, movement, or input calls. `ResourceManager` is the only class that calls any Load or Unload function — this is enforced by never including `raylib.h` asset functions in any other class that does not need them.
